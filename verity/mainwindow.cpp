#include "mainwindow.h"
#include "versereferenceparser.h"
#include "parsingdecoder.h"
#include "timer.h"
#include "globalvariables.h"
#include "dictionarybrowser.h"
#include "parsingdisplaybrowser.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QToolBar>
#include <QLineEdit>
#include <QPushButton>
#include <QDockWidget>
#include <QSettings>
#include <QLayout>

#include <iostream>
using namespace std;


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QSettings settings(PROGRAM_NAME, PROGRAM_NAME);
    settings.beginGroup(MAIN_WINDOW_SETTING_GROUP);


    resize(settings.value(SIZE_SETTING, QSize(1000, 700)).toSize());
    move(settings.value(POS_SETTING, QPoint(QApplication::desktop()->width()/2-500, QApplication::desktop()->height()/2-350)).toPoint());

    if(settings.value(WINDOW_STATE_SETTING, true).toBool())
        setWindowState(Qt::WindowMaximized);

    //    DATA_PATH = settings.value(DATA_PATH_SETTING, "/usr/share/verity").toString();
    DATA_PATH = ".";
    qDebug() << "data path:" << DATA_PATH;

    settings.endGroup();

    QString blah = (QString)"hi";

    setWindowTitle(PROGRAM_NAME);
    setMinimumSize(1000, 700);
    browser = new BibleTextBrowser();
    //    vscroll = new QSnapScrollBar(browser);
    //    vscroll->setParent(browser);
    //    browser->setVerticalScrollBar(vscroll);
    //    connect(browser, SIGNAL(chapterStarts(QList<int>)), vscroll, SLOT(defineSnapPoints(QList<int>)));
    setCentralWidget(browser);

    //    QDockWidget* hoverDock = new QDockWidget("Parsing", this);
    //    hoverDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    //
    //    hoverBrowser = new ParsingDisplayBrowser(hoverDock);
    //    hoverDock->setWidget(hoverBrowser);
    //    connect(browser, SIGNAL(wordHoveredOver(TextInfo)), this, SLOT(wordHoveredOver(TextInfo)));
    //
    //    addDockWidget(Qt::BottomDockWidgetArea, hoverDock);


    QDockWidget* selectedDock = new QDockWidget("Parsing", this);
    selectedDock->setAllowedAreas(Qt::RightDockWidgetArea);
    ParsingDisplayBrowser* selectedBrowser = new ParsingDisplayBrowser(selectedDock);
    selectedDock->setWidget(selectedBrowser);
    connect(browser, SIGNAL(wordClicked(TextInfo)), selectedBrowser, SLOT(display(TextInfo)));
    addDockWidget(Qt::RightDockWidgetArea, selectedDock);

    QDockWidget* dictionaryDock = new QDockWidget("Dictionary", this);
    dictionaryDock->setAllowedAreas(Qt::RightDockWidgetArea);    
    DictionaryBrowser* dictionaryBrowser = new DictionaryBrowser(dictionaryDock);
    dictionaryDock->setWidget(dictionaryBrowser);
    connect(browser, SIGNAL(wordClicked(TextInfo)), dictionaryBrowser, SLOT(display(TextInfo)));
    addDockWidget(Qt::RightDockWidgetArea, dictionaryDock);



    QToolBar* toolbar = new QToolBar();
    toolbar->layout()->setSpacing(3);

    verseLineEdit = new QLineEdit();
    verseLineOutput = new QLabel();
    connect(verseLineEdit, SIGNAL(textEdited(QString)), this, SLOT(verseLineEditChanged(QString)));
    connect(verseLineEdit, SIGNAL(returnPressed()), this, SLOT(lookupVerse()));
    verseLineEdit->setMaximumWidth(300);
    toolbar->addWidget(verseLineEdit);
    toolbar->addWidget(verseLineOutput);
    addToolBar(toolbar);
    this->setWindowIcon(QIcon("verity.ico"));
}

void MainWindow::afterShown()
{
    verseLineEdit->setFocus();
}

void MainWindow::writeOutSettings()
{
    browser->writeOutSettings();

    QSettings settings(PROGRAM_NAME, PROGRAM_NAME);

    settings.beginGroup(MAIN_WINDOW_SETTING_GROUP);
    settings.setValue(SIZE_SETTING, size());
    settings.setValue(POS_SETTING, pos());
    settings.setValue(WINDOW_STATE_SETTING, (windowState() & Qt::WindowMaximized) > 0);
    //    settings.setValue(DATA_PATH_SETTING, DATA_PATH);
    settings.endGroup();

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeOutSettings();
    event->accept();
}

void MainWindow::lookupVerse()
{
    QString verse = verseLineEdit->text();
    if(verse.length() > 0)
    {
        timer t;
        t.start();

        VerseReference verseReference = VerseReferenceParser::parse(verse);

        browser->display(verseReference);

        cout << "total: " << t << endl;
    }
}

//void MainWindow::wordHoveredOver(TextInfo textInfo)
//{
//    hoverBrowser->display(textInfo);
//}


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

void MainWindow::verseLineEditChanged(QString text)
{
    if(text.length() > 0)
        verseLineOutput->setText(VerseReferenceParser::parse(text).stringRepresentation);
}
