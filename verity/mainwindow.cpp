#include "mainwindow.h"
#include "versereferenceparser.h"
#include "parsingdecoder.h"
#include "timer.h"
#include "biblequerier.h"
#include "globalvariables.h"
#include "dictionarybrowser.h"
#include "parsingdisplaybrowser.h"
#include "textinfo.h"
#include "referencefilterproxymodel.h"
#include "referencefilter.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QToolBar>
#include <QLineEdit>
#include <QToolButton>
#include <QDockWidget>
#include <QSettings>
#include <QLayout>
#include <QStringList>
#include <QComboBox>
#include <QCompleter>

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

        DATA_PATH = settings.value(DATA_PATH_SETTING, "/usr/share/verity").toString();
//    DATA_PATH = ".";
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

    QDockWidget* searchResultsDock = new QDockWidget("Search Results", this);
    searchResultsDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    searchBrowser = new SearchBrowser();
    searchResultsDock->setWidget(searchBrowser);
    addDockWidget(Qt::LeftDockWidgetArea, searchResultsDock);
    connect(searchBrowser, SIGNAL(goToResult(QList<QString>, VerseReference)), browser, SLOT(display(QList<QString>, VerseReference)));
//    searchResultsDock->setFloating(true);

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
        toggleButton->setIcon(QIcon("/home/j3frea/Pictures/ico.svg"));
        connect(toggleButton, SIGNAL(toggled(bool)), this, SLOT(textToggled(bool)));
        toolbar->addWidget(toggleButton);
//        toggleButton->setStyleSheet("border: 2px solid #8f8f91; border-radius: 6px; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f6f7fa, stop: 1 #dadbde); min-width: 80px;");
        toggleButton->setStyleSheet(
                "QPushButton:hover {"
                    "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f3f0ef, stop: 1 #9f9fa8);"
                "}"
                "QPushButton:pressed {"
                    "padding-left: 5px;"
                    "padding-top: 5px;"
                    "border-top: 1px solid #6f6f71;"
                    "border-left: 1px solid #6f6f71;"
                    "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #dadbde, stop: 1 #f6f7fa);"
                "}"
                "QPushButton:checked {"
                    "padding-left: 3px;"
                    "padding-top: 4px;"
                    "border-top: 1px solid #6f6f71;"
                    "border-left: 1px solid #6f6f71;"
                    "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #d8d2c2, stop: 1 #62528d);"
                "}"
                "QPushButton:flat {"
                    "border: none;"
                "}"
                "QPushButton {"
                    "border-width: 1px;"
                    "border-color: #8f8f91;"
                    "border-top: 1px solid #afafb1;"
                    "border-left: 1px solid #afafb1;"
                    "border-style: solid;"
                    "margin: 2px;"
                    "border-radius: 5px; padding: 3px; left: -3px;"
                    "width: 90px;"
                    "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e3e0cd, stop: 1 #9797a0);"
                "}"
            );
        if (i == 0 || i == 2)
            toggleButton->setChecked(true);

    }

        this->setStyleSheet("QMainWindow {background-color: qlineargradient(x1: 0.25, y1: 0, x2: 0.55, y2: 1, stop: 0 #f0ebe2, stop: 1 #ccc8c0);}");
//    searchBrowser->setTextsAvaiable(textsAvailable);

    verseLineEdit = new QLineEdit();
    verseLineEdit->setMaximumWidth(300);

    verseLineOutput = new QLabel();

    QComboBox *cmb = new QComboBox(this);
    cmb->setEditable(true);
    QStringList books;
    books << "Genesis" << "Exodus" << "Leviticus" << "Numbers" << "Deuteronomy" << "Joshua" << "Judges" << "Ruth" << "1 Samuel" << "2 Samuel" << "1 Kings" << "2 Kings" << "1 Chronicles" << "2 Chronicles" << "Ezra" << "Nehemiah" << "Esther" << "Job" << "Psalms" << "Proverbs" << "Ecclesiastes" << "Song of Songs" << "Isaiah" << "Jeremiah" << "Lamentations" << "Ezekiel" << "Daniel" << "Hosea" << "Joel" << "Amos" << "Obadiah" << "Jonah" << "Micah" << "Nahum" << "Habakkuk" << "Zephaniah" << "Haggai" << "Zechariah" << "Malachi";
    books << "Matthew" << "Mark" << "Luke" << "John" << "Acts" << "Romans" << "1 Corinthians" << "2 Corinthians" << "Galatians" << "Ephesians" << "Philippians" << "Colossians" << "1 Thessalonians" << "2 Thessalonians" << "1 Timothy" << "2 Timothy" << "Titus" << "Philemon" << "Hebrews" << "James" << "1 Peter" << "2 Peter" << "1 John" << "2 John" << "3 John" << "Jude" << "Revelation";

    cmb->addItems(books);
    QCompleter *completer = new QCompleter(this);
    completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion); // always show all completions
    ReferenceFilterProxyModel *pFilterModel = new ReferenceFilterProxyModel(this);
    pFilterModel->setSourceModel(cmb->model());
    connect(cmb, SIGNAL(editTextChanged(const QString &)), pFilterModel, SLOT(setFilterFixedString(const QString &)));
    completer->setModel(pFilterModel); // use the filtered proxy model to do the substring matching

    cmb->setCompleter(completer);

    //    QAbstractItemModel *tmp = completer->model();

//    ReferenceFilter *rf;
//    rf->setItemList(books);
//
//    connect(cmb, SIGNAL(textEdited(QString)), rf, SLOT(setFilter(QString)));

    connect(verseLineEdit, SIGNAL(textEdited(QString)), this, SLOT(verseLineEditChanged(QString)));
    connect(verseLineEdit, SIGNAL(returnPressed()), this, SLOT(performVerseLineEdit()));

    toolbar->addWidget(verseLineEdit);
    toolbar->addWidget(verseLineOutput);
    toolbar->addWidget(cmb);


    addToolBar(toolbar);
    this->setWindowIcon(QIcon(DATA_PATH + "/verity.ico"));
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
    searchBrowser->setTextsAvaiable(texts);
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

void MainWindow::performVerseLineEdit()
{
    QString term = verseLineEdit->text();
    if (term.left(2) == "s.")
    {
        searchBrowser->performSearch(term);
    }
    else if(term.length() > 0)
    {
//        timer t;
//        t.start();

        VerseReference verseReference = VerseReferenceParser::parse(term);

        browser->display(texts, verseReference);

//        cout << "total: " << t << endl;
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
    {
        if (string.left(2) == "s.")
        {
            if (verseLineOutput->text() != "will search...")
            {
                verseLineOutput->setText("will search...");
            }
        }
        else
        {
           verseLineOutput->setText(VerseReferenceParser::parse(string).stringRepresentation);
        }
    }
    else
    {
        verseLineOutput->setText("");
    }

}
