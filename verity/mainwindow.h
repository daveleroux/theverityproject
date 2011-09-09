#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QList>
#include "vbibleinterface.h"
#include "searchbrowser.h"
#include "qsnapscrollbar.h"
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>

#include "listener.h"

class MainWindow : public QMainWindow, public Listener
{
    Q_OBJECT

private:
    static QString MAIN_WINDOW_SETTING_GROUP;
    static QString GEOMETRY_SETTING;
    static QString WINDOW_STATE_SETTING;

public:

    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void afterShown();

    void display(int wordNumber);

protected:

    vBibleInterface* bibleInterface;
    SearchBrowser* searchBrowser;
//    LocationLineEdit* verseLineEdit;
    QLabel* verseLineOutput;
    void keyPressEvent(QKeyEvent* keyEvent);
//    QList<int> texts;

    void closeEvent(QCloseEvent *event);

    void handleEvent(Event* event);

public slots:
//    void performVerseLineEdit();
//    void verseLineEditChanged(QString string);
//    void textToggled(bool);

};

#endif // MAINWINDOW_H
