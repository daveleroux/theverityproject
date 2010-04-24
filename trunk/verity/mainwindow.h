#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QList>
#include "bibletextbrowser.h"
#include "qsnapscrollbar.h"
#include <QLineEdit>
#include <QLabel>


class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:

    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void afterShown();

    void display(int wordNumber);

protected:

    BibleTextBrowser* browser;
    QLineEdit* verseLineEdit;
    QLabel* verseLineOutput;
    void keyPressEvent(QKeyEvent* keyEvent);

    //     ParsingDisplayBrowser* hoverBrowser;
    //ParsingDisplayBrowser* selectedBrowser;

    void closeEvent(QCloseEvent *event);
    void writeOutSettings();

public slots:
    void lookupVerse();
    //    void wordHoveredOver(TextInfo textInfo);
//    void wordClicked(TextInfo textInfo);
    void verseLineEditChanged(QString text);

};

#endif // MAINWINDOW_H
