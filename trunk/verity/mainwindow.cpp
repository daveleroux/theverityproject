#include "mainwindow.h"
#include "versereferenceparser.h"
#include "parsingdecoder.h"
#include "timer.h"
#include "globalvariables.h"
#include "dictionarybrowser.h"
#include "parsingdisplaybrowser.h"
#include "textinfo.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QToolBar>
#include <QLineEdit>
#include <QToolButton>
#include <QDockWidget>
#include <QSettings>
#include <QLayout>
#include <QComboBox>

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

//        DATA_PATH = settings.value(DATA_PATH_SETTING, "/usr/share/verity").toString();
    DATA_PATH = ".";
    qDebug() << "data path:" << DATA_PATH;

    settings.endGroup();

    setWindowTitle(PROGRAM_NAME);
    setMinimumSize(1000, 700);
    browser = new BibleTextBrowser();
    setCentralWidget(browser);

    QDockWidget* selectedDock = new QDockWidget("Parsing", this);
    selectedDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    ParsingDisplayBrowser* selectedBrowser = new ParsingDisplayBrowser(selectedDock);
    selectedDock->setWidget(selectedBrowser);
    connect(browser, SIGNAL(wordClicked(TextInfo*)), selectedBrowser, SLOT(display(TextInfo*)));
    addDockWidget(Qt::RightDockWidgetArea, selectedDock);

    QDockWidget* dictionaryDock = new QDockWidget("Dictionary", this);
    dictionaryDock->setAllowedAreas(Qt::AllDockWidgetAreas);

    QWidget* layoutWidget = new QWidget(dictionaryDock);
    QToolBar* dictionaryToolBar = new QToolBar();
    QToolButton* backButton = new QToolButton();
    backButton->setArrowType(Qt::LeftArrow);
    backButton->setEnabled(false);
    dictionaryToolBar->addWidget(backButton);
    QToolButton* forwardButton = new QToolButton();
    forwardButton->setArrowType(Qt::RightArrow);
    forwardButton->setEnabled(false);
    dictionaryToolBar->addWidget(forwardButton);
    DictionaryBrowser* dictionaryBrowser = new DictionaryBrowser();    

    connect(dictionaryBrowser, SIGNAL(backwardAvailable(bool)), backButton, SLOT(setEnabled(bool)));
    connect(dictionaryBrowser, SIGNAL(forwardAvailable(bool)), forwardButton, SLOT(setEnabled(bool)));
    connect(backButton, SIGNAL(clicked()), dictionaryBrowser, SLOT(backward()));
    connect(forwardButton, SIGNAL(clicked()), dictionaryBrowser, SLOT(forward()));

    QLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(dictionaryToolBar);
    layout->addWidget(dictionaryBrowser);

    layoutWidget->setLayout(layout);

    dictionaryDock->setWidget(layoutWidget);
    connect(browser, SIGNAL(wordClicked(TextInfo*)), dictionaryBrowser, SLOT(display(TextInfo*)));
    addDockWidget(Qt::RightDockWidgetArea, dictionaryDock);



    QToolBar* toolbar = new QToolBar();
    toolbar->layout()->setSpacing(3);

    QList<QString> textsAvailable;
    textsAvailable.append("esv");
    textsAvailable.append("kjv");
    textsAvailable.append("tisch");
    textsAvailable.append("wlc");

    for(int i=0; i< textsAvailable.size(); i++)
    {
        QPushButton* toggleButton = new QPushButton(textsAvailable.at(i));
        toggleButton->setCheckable(true);
        connect(toggleButton, SIGNAL(toggled(bool)), this, SLOT(textToggled(bool)));
        if(i == 0)
            toggleButton->setChecked(true);
        toolbar->addWidget(toggleButton);
    }

    verseLineEdit = new QLineEdit();
    verseLineEdit->setMaximumWidth(300);

    verseLineOutput = new QLabel();

    connect(verseLineEdit, SIGNAL(textEdited(QString)), this, SLOT(verseLineEditChanged(QString)));
    connect(verseLineEdit, SIGNAL(returnPressed()), this, SLOT(lookupVerse()));

    toolbar->addWidget(verseLineEdit);
    toolbar->addWidget(verseLineOutput);


    addToolBar(toolbar);
    this->setWindowIcon(QIcon("verity.ico"));
}

void MainWindow::textToggled(bool checked)
{
    QPushButton* sender = (QPushButton*)QObject::sender();
    QString text = sender->text();
    if(checked)
    {
        texts.append(text);
    }
    else
    {
        texts.removeAt(texts.indexOf(text));
    }
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
//        settings.setValue(DATA_PATH_SETTING, DATA_PATH);
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

        browser->display(texts, verseReference);

        cout << "total: " << t << endl;
    }
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

void MainWindow::verseLineEditChanged(QString string)
{
    if(string.length() > 0)
        verseLineOutput->setText(VerseReferenceParser::parse(string).stringRepresentation);
}
