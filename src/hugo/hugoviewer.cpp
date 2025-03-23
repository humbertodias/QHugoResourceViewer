#include "hugo/hugoviewer.h"
#include <QMenuBar>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QImageReader>
#include <QDir>
#include <QGraphicsPixmapItem>
#include <string>
#include <algorithm>

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
    }

    void HugoViewer::openArchive()
    {
        QString fileName = QFileDialog::getOpenFileName(this, "Open Archive", "", "Hugo archive files (*.res *.big *.dat)");
        if (!fileName.isEmpty()) {
            // Load file (e.g., populate listWidget with items)
            listWidget->setEnabled(true);
        }
    }

    void HugoViewer::openFolder()
    {
        QString folderPath = QFileDialog::getExistingDirectory(this, "Open Folder", "");
        if (!folderPath.isEmpty()) {
            QDir dir(folderPath);
            QStringList files = dir.entryList(QStringList() << "*.res" << "*.big" << "*.dat", QDir::Files);
            listWidget->clear();
            listWidget->addItems(files);
        }
    }

    void HugoViewer::onListItemSelected()
    {
        // Handle selection change in list
        QString selectedItem = listWidget->currentItem()->text();
        // Example: Load the image (implement based on your archive)
        loadImage(selectedItem);
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
        // Implement extraction logic here
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
}