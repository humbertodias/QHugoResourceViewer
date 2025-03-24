#include "hugo/hugoviewer.h"
#include <QMenuBar>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QImageReader>
#include <QDir>
#include <QGraphicsPixmapItem>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QBuffer>
#include <QIODevice>
#include <string>
#include <algorithm>
#include <fstream>
#include <formats/cgffile.h>
#include "formats/rawfile.h"
#include "formats/wavfile.h"

#include "hugo/hugoresource.h"
#include "hugo/hugo_archive_processor.h"

namespace hugo {
    HugoViewer::HugoViewer(QWidget *parent)
        : QMainWindow(parent), scene(new QGraphicsScene(this))
    {
        setupUi();
    }

    HugoViewer::~HugoViewer()
    {
    }

    void HugoViewer::setupUi()
    {
        setWindowTitle("Hugo Resource Viewer");
        resize(1145, 568);

        // Create main layout and widgets
        listWidget = new QListWidget(this);
        listWidget->setEnabled(true);
        connect(listWidget, &QListWidget::itemSelectionChanged, this, &HugoViewer::onListItemSelected);

        pictureView = new QGraphicsView(scene, this);
        pictureView->setVisible(true);

        label = new QLabel("Number of frame:", this);
        comboNumberOfFrame = new QComboBox(this);
        comboPalette = new QComboBox(this);

        extractButton = new QPushButton("Extract file", this);
        extractButton->setEnabled(false);
        connect(extractButton, &QPushButton::clicked, this, &HugoViewer::extractFile);

        saveButton = new QPushButton("Save to PNGs", this);
        saveButton->setEnabled(false);
        connect(saveButton, &QPushButton::clicked, this, &HugoViewer::saveToPNGs);

        // Create menu bar
        QMenu *fileMenu = menuBar()->addMenu("File");
        QAction *openArchiveAction = fileMenu->addAction("Open Hugo archive");
        connect(openArchiveAction, &QAction::triggered, this, &HugoViewer::openArchive);

        QAction *openFolderAction = fileMenu->addAction("Open folder");
        connect(openFolderAction, &QAction::triggered, this, &HugoViewer::openFolder);

        QMenu *helpMenu = menuBar()->addMenu("Help");
        QAction *aboutAction = helpMenu->addAction("About");
        connect(aboutAction, &QAction::triggered, this, [](){
            QMessageBox::information(nullptr, "About", "Hugo Resource Viewer - Qt Version");
        });

        // Layout for listWidget and pictureView side by side
        QHBoxLayout *horizontalLayout = new QHBoxLayout;
        horizontalLayout->addWidget(listWidget);
        horizontalLayout->addWidget(pictureView);

        // Vertical layout for the rest of the widgets
        QVBoxLayout *verticalLayout = new QVBoxLayout;
        verticalLayout->addWidget(label);
        verticalLayout->addWidget(comboNumberOfFrame);
        verticalLayout->addWidget(comboPalette);
        verticalLayout->addWidget(extractButton);
        verticalLayout->addWidget(saveButton);

        // Main layout
        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addLayout(horizontalLayout);  // Add side-by-side layout first
        mainLayout->addLayout(verticalLayout);    // Then add the vertical layout for other widgets

        QWidget *centralWidget = new QWidget(this);
        centralWidget->setLayout(mainLayout);
        setCentralWidget(centralWidget);

        // multimedia
        player = new QMediaPlayer;
        audioOutput = new QAudioOutput;
        player->setAudioOutput(audioOutput);
    }

    HugoResource hugo_resource;
    HugoArchiveProcessor archiveProcessor;


    void HugoViewer::openArchive()
    {
        QString fileName = QFileDialog::getOpenFileName(this, "Open Archive", "", "Hugo archive files (*.res *.big *.dat *.RES *.BIG *.DAT);;All Files (*.*)");
        if (!fileName.isEmpty()) {
            // Load file (e.g., populate listWidget with items)
            listWidget->setEnabled(true);
            extractButton->setEnabled(false);
            saveButton->setEnabled(false);

            resetSelection();

            archiveProcessor.OpenArchive(fileName.toStdString());
            listWidget->addItems(toQStringList(archiveProcessor.GetFileEntries() ) );
            comboPalette->addItems(toQStringList(archiveProcessor.GetPaletteEntries() ) );

        }
    }

    QStringList HugoViewer::toQStringList(const std::vector<std::string> &vec) {
        QStringList list;
        for (const auto &s : vec) {
            list.append(QString::fromStdString(s));
        }
        return list;
    }

    void HugoViewer::openFolder()
    {
        QString folderPath = QFileDialog::getExistingDirectory(this, "Open Folder", "");
        if (!folderPath.isEmpty()) {
            resetSelection();
            QDir dir(folderPath);
            QStringList files = dir.entryList(QStringList() << "*.res" << "*.big" << "*.dat" << "*.RES" << "*.BIG" << "*.DAT", QDir::Files);
            listWidget->clear();
            listWidget->addItems(files);
        }
    }

    void HugoViewer::resetSelection() {
        listWidget->clear();
        comboNumberOfFrame->clear();
        comboPalette->clear();
    }

    void HugoViewer::onListItemSelected()
    {
        extractButton->setEnabled(false);
        saveButton->setEnabled(false);

        // Handle selection change in list
        QString selectedItem = listWidget->currentItem()->text();

        int index = listWidget->currentRow();
        auto myOffset = archiveProcessor.get_offsetsInFile(index);

        std::string ext = selectedItem.toStdString().substr(selectedItem.length() - 4);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if ((ext == ".raw") || (ext == ".blk") || (ext == ".pic")) {
            // loadImage(selectedItem);
            loadImage(archiveProcessor.filePath, myOffset);
            extractButton->setEnabled(true);
            saveButton->setEnabled(true);
        }

        if (ext == ".wav") {
            loadWav(archiveProcessor.filePath, myOffset);
            extractButton->setEnabled(true);
        }

    }

    void HugoViewer::loadImage(const std::string &fp, uint32_t myOffset) {
        formats::rawFile c(fp, myOffset);
        if (c.getStatus()) {
            int w, h;
            std::vector<uint8_t> pic = c.getPicture(0, w, h);

            QImage image(w, h, QImage::Format_ARGB32);
            auto palette = c.getPal();
            for (int i = 0; i < h; ++i) {
                for (int j = 0; j < w; ++j) {
                    int index = pic[i * w + j] * 3;
                    int a = 255;
                    int r = palette[index + 2];
                    int g = palette[index + 1];
                    int b = palette[index];

                    image.setPixel(j, i, qRgba(r, g, b, a));
                }
            }
            // TODO
            scene->clear();
            scene->addPixmap(QPixmap::fromImage(image));
            pictureView->setVisible(true);
        }
    }

    void HugoViewer::loadWav(const std::string &filePath, uint32_t myOffset) {

        formats::wavFile wav (filePath, myOffset);
        if (wav.getStatus()) {

            std::vector<uint8_t> data = wav.getFileContent();
            QByteArray audioData = toQByteArray(data);

            // Criar buffer para armazenar os bytes do áudio
            QBuffer *audioBuffer = new QBuffer;
            audioBuffer->setData(audioData);
            audioBuffer->open(QIODevice::ReadOnly);

            player->setSourceDevice(audioBuffer);
            player->play();

        }
    }

    QByteArray HugoViewer::toQByteArray(const std::vector<uint8_t> &vec) {
        return {reinterpret_cast<const char*>(vec.data()), static_cast<int>(vec.size())};
    }

    void HugoViewer::onComboBoxNumberOfFrameChanged()
    {
        // Handle comboBox1 changes
    }

    void HugoViewer::onComboBoxPaletteChanged()
    {
        // Handle comboBox2 changes
    }

    void HugoViewer::extractFile()
    {

        QString selectedItem = listWidget->currentItem()->text();
        int index = listWidget->currentRow();
        auto myOffset = archiveProcessor.get_offsetsInFile(index);
        std::string ext = selectedItem.toStdString().substr(selectedItem.length() - 4);

        if (ext == ".wav") {
            formats::wavFile wav (archiveProcessor.filePath, myOffset);
            QByteArray audioData = toQByteArray(wav.getFileContent());
            QFileDialog::saveFileContent(audioData, selectedItem);
        } else {
            QFileDialog::saveFileContent(nullptr, selectedItem);
        }
    }

    void HugoViewer::saveToPNGs()
    {
        // Implement saving logic here
    }

    void HugoViewer::loadImage(const QString &imagePath)
    {
        QImage image(imagePath);
        if (image.isNull()) {
            QMessageBox::warning(this, "Error", "Failed to load image.");
        } else {
            scene->clear();
            scene->addPixmap(QPixmap::fromImage(image));
            pictureView->setVisible(true);
        }
    }

    void fillImage(QImage &image) {
        int width = image.width();
        int height = image.height();

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int red = x % 256;      // Gradiente horizontal
                int green = y % 256;    // Gradiente vertical
                int blue = (x + y) % 256; // Combinação dos dois

                image.setPixel(x, y, qRgb(red, green, blue));
            }
        }
    }

    bool HugoViewer::testExt(const std::string& fn) {
        if (fn.length() < 4) {
            return false;
        }

        std::string ext = fn.substr(fn.length() - 4);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        return (ext == ".cgf" || ext == ".raw" || ext == ".lzp" || ext == ".pal" || ext == ".ti2" ||
                ext == ".ti4" || ext == ".til" || ext == ".cbr" || ext == ".blk" || ext == ".pbr" ||
                ext == ".pic" || ext == ".brs");
    }

    bool HugoViewer::testPal(const std::string& fn, uint32_t palOffset) {
        // Extract the file extension (last 4 characters) and convert to lowercase
        std::string ext = fn.substr(fn.size() - 4);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        // Check if the extension is in the valid list
        if (ext == ".raw" || ext == ".lzp" || ext == ".pal" || ext == ".til" || ext == ".ti2" || ext == ".ti4" || ext == ".blk" || ext == ".pic") {
            return true;
        }
        // Check if the extension is ".cgf"
        else if (ext == ".cgf") {
            // cgfFile tmp;
            // if (archive) {
            //     tmp = cgfFile(myfld, palOffset);
            // } else {
            //     tmp = cgfFile(myfld + '\\' + fn, palOffset);
            // }
            // // Check if the number of palettes is non-zero
            // if (tmp.getNumPal() != 0) {
            //     return true;
            // }
        }

        return false;
    }

}
