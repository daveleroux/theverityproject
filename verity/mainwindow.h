#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QList>
#include "vbibleinterface.h"
#include "qsnapscrollbar.h"
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>

#include "listener.h"
#include "wordclickedlistener.h"

class MainWindow : public QMainWindow, public Listener
{
    Q_OBJECT

private:
    static QString MAIN_WINDOW_SETTING_GROUP;
    static QString GEOMETRY_SETTING;
    static QString WINDOW_STATE_SETTING;
    static QString LAST_TEXT_SETTING;

public:

    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void display(int wordNumber);

protected:

    WordClickedListener* wordClickedListener;

    VBibleInterface* bibleInterface;
    QLabel* verseLineOutput;
//    void keyPressEvent(QKeyEvent* keyEvent);

    void closeEvent(QCloseEvent *event);

    void handleEvent(Event* event);

public slots:
    void netCopyright();
    void wlcCopyright();
    void tischCopyright();
    void strongsHebrewCopyright();
    void strongsGreekCopyright();

    void verityAbout();
    void help();

};

#endif // MAINWINDOW_H
