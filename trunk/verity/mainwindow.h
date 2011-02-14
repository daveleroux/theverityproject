#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QList>
#include "bibletextbrowser.h"
#include "searchbrowser.h"
#include "qsnapscrollbar.h"
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>


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
    SearchBrowser* searchBrowser;
    QLineEdit* verseLineEdit;
    QLabel* verseLineOutput;
    void keyPressEvent(QKeyEvent* keyEvent);
    QList<QString> texts;

    void closeEvent(QCloseEvent *event);
    void writeOutSettings();

public slots:
    void performVerserLineEdit();
    void verseLineEditChanged(QString string);
    void textToggled(bool);

};

#endif // MAINWINDOW_H
