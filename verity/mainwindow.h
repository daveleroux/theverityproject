#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QList>
#include "bibletextbrowser.h"
#include "parsingdisplaybrowser.h"
#include "qsnapscrollbar.h"
#include <QLineEdit>



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
    void keyPressEvent(QKeyEvent* keyEvent);

    //     ParsingDisplayBrowser* hoverBrowser;
    ParsingDisplayBrowser* selectedBrowser;

    void closeEvent(QCloseEvent *event);

public slots:
    void lookupVerse();
    //    void wordHoveredOver(TextInfo textInfo);
    void wordClicked(TextInfo textInfo);

};

#endif // MAINWINDOW_H
