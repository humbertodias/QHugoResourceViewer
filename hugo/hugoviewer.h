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

namespace hugo {
    class HugoViewer : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit HugoViewer(QWidget *parent = nullptr);
        ~HugoViewer();

        private slots:
            void openArchive();
        void openFolder();
        void onListItemSelected();
        void onComboBox1Changed();
        void onComboBox2Changed();
        void extractFile();
        void saveToPNGs();

    private:
        void setupUi();
        void loadImage(const QString &imagePath);

        QListWidget *listWidget;
        QComboBox *comboBox1;
        QComboBox *comboBox2;
        QLabel *label;
        QGraphicsView *pictureView;
        QPushButton *extractButton;
        QPushButton *saveButton;
        QGraphicsScene *scene;
    };
}
#endif // HUGOVIEWER_H
