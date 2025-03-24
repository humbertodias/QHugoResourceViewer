#ifndef HUGOVIEWER_H
#define HUGOVIEWER_H

#include <QMainWindow>
#include <QMenuBar>
#include <QListWidget>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QImage>
#include <QPixmap>
#include <QFileDialog>
#include <QGraphicsView>
#include <QGraphicsScene>

class QAudioOutput;
class QMediaPlayer;

namespace hugo {
    class HugoViewer : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit HugoViewer(QWidget *parent = nullptr);
        ~HugoViewer();
        static auto testExt(const std::string &fn) -> bool;
        static auto testPal(const std::string& fn, uint32_t palOffset) -> bool;

    private slots:
        void openArchive();

        static QStringList toQStringList(const std::vector<std::string> &vec);

        void openFolder();

        void resetSelection();

        void onListItemSelected();

        void loadImage(const std::string &fp, uint32_t myOffset);
        void loadWav(const std::string &filePath, uint32_t myOffset);

        QByteArray toQByteArray(const std::vector<uint8_t> &vec);

        void onComboBoxNumberOfFrameChanged();
        void onComboBoxPaletteChanged();
        void extractFile();
        void saveToPNGs();


    private:
        void setupUi();
        void loadImage(const QString &imagePath);

        QListWidget *listWidget;
        QComboBox *comboNumberOfFrame;
        QComboBox *comboPalette;
        QLabel *label;
        QGraphicsView *pictureView;
        QPushButton *extractButton;
        QPushButton *saveButton;
        QGraphicsScene *scene;

        QMediaPlayer *player;
        QAudioOutput *audioOutput;

    };
}
#endif // HUGOVIEWER_H
