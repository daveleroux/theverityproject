#include "mainwindow.h"
#include "versereferenceparser.h"
#include "parsingdecoder.h"
#include "timer.h"
#include "globalvariables.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QToolBar>
#include <QLineEdit>
#include <QPushButton>
#include <QDockWidget>
#include <QSettings>

#include <iostream>
using namespace std;


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QSettings settings(PROGRAM_NAME, PROGRAM_NAME);
    settings.beginGroup(MAIN_WINDOW_SETTING_GROUP);

    resize(settings.value(SIZE_SETTING, QSize(1000, 700)).toSize());
    move(settings.value(POS_SETTING, QPoint(QApplication::desktop()->width()/2-500, QApplication::desktop()->height()/2-350)).toPoint());

//    DATA_PATH = settings.value(DATA_PATH_SETTING, "/usr/share/verity").toString();
    DATA_PATH = ".";
    qDebug() << "data path:" << DATA_PATH;

    settings.endGroup();



    setWindowTitle(PROGRAM_NAME);
    setMinimumSize(1000, 700);
    browser = new BibleTextBrowser();
    setCentralWidget(browser);

    //    QDockWidget* hoverDock = new QDockWidget("Parsing", this);
    //    hoverDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    //
    //    hoverBrowser = new ParsingDisplayBrowser(hoverDock);
    //    hoverDock->setWidget(hoverBrowser);
    //    connect(browser, SIGNAL(wordHoveredOver(TextInfo)), this, SLOT(wordHoveredOver(TextInfo)));
    //
    //    addDockWidget(Qt::BottomDockWidgetArea, hoverDock);


    QDockWidget* selectedDock = new QDockWidget("Selected Parsing", this);
    selectedDock->setAllowedAreas(Qt::RightDockWidgetArea);

    selectedBrowser = new ParsingDisplayBrowser(selectedDock);
    selectedDock->setWidget(selectedBrowser);
    connect(browser, SIGNAL(wordClicked(TextInfo)), this, SLOT(wordClicked(TextInfo)));

    addDockWidget(Qt::RightDockWidgetArea, selectedDock);


    QToolBar* toolbar = new QToolBar();
    verseLineEdit = new QLineEdit();
    connect(verseLineEdit, SIGNAL(returnPressed()), this, SLOT(lookupVerse()));
    verseLineEdit->setMaximumWidth(300);
    toolbar->addWidget(verseLineEdit);
    addToolBar(toolbar);
    verseLineEdit->setFocus();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings(PROGRAM_NAME, PROGRAM_NAME);

    settings.beginGroup(MAIN_WINDOW_SETTING_GROUP);
    settings.setValue(SIZE_SETTING, size());
    settings.setValue(POS_SETTING, pos());
//    settings.setValue(DATA_PATH_SETTING, DATA_PATH);
    settings.endGroup();

    event->accept();
}

void MainWindow::lookupVerse()
{
    timer t;
    t.start();

    QString verse = verseLineEdit->text();
    VerseReference verseReference = VerseReferenceParser::parse(verse);   

    browser->display(verseReference);

    cout << "total: " << t << endl;
}

//void MainWindow::wordHoveredOver(TextInfo textInfo)
//{
//    hoverBrowser->display(textInfo);
//}

void MainWindow::wordClicked(TextInfo textInfo)
{
    selectedBrowser->display(textInfo, true);
}

MainWindow::~MainWindow()
{

}

void MainWindow::keyPressEvent(QKeyEvent* keyEvent)
{
    if(keyEvent->modifiers() & Qt::ControlModifier && keyEvent->key() == Qt::Key_L)
    {
        verseLineEdit->setFocus();
        verseLineEdit->selectAll();
    }
    else if(keyEvent->key() == Qt::Key_Q)
    {
        window()->close();
    }
    else
    {
        QWidget::keyPressEvent(keyEvent);
    }
}
