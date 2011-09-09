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
#include "eventmanager.h"
#include "wordclickedevent.h"
#include "wordclickedlistener.h"
#include "strongsevent.h"
#include "netnotebrowser.h"
#include "basicevent.h"

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

QString MainWindow::MAIN_WINDOW_SETTING_GROUP = "MainWindowSettingGroup";
QString MainWindow::GEOMETRY_SETTING = "GeometrySetting";
QString MainWindow::WINDOW_STATE_SETTING = "WindowStateSetting";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), Listener()
{
    QSettings settings(PROGRAM_NAME, PROGRAM_NAME);
    settings.beginGroup(MAIN_WINDOW_SETTING_GROUP);


//    resize(settings.value(SIZE_SETTING, QSize(1000, 700)).toSize());
//    move(settings.value(POS_SETTING, QPoint(QApplication::desktop()->width()/2-500, QApplication::desktop()->height()/2-350)).toPoint());

//    if(settings.value(WINDOW_STATE_SETTING, true).toBool())
//        setWindowState(Qt::WindowMaximized);


    restoreGeometry(settings.value(GEOMETRY_SETTING).toByteArray());




    //        DATA_PATH = settings.value(DATA_PATH_SETTING, "/usr/share/verity").toString();
    DATA_PATH = ".";
    qDebug() << "data path:" << DATA_PATH;


    setWindowTitle(PROGRAM_NAME);
    setMinimumSize(1000, 700);
    bibleInterface = new vBibleInterface(this);
    setCentralWidget(bibleInterface);
    qDebug() << "central widget:" << DATA_PATH;

    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    QDockWidget* selectedDock = new QDockWidget("Parsing", this);
    selectedDock->setObjectName(selectedDock->windowTitle());
    selectedDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    ParsingDisplayBrowser* selectedBrowser = new ParsingDisplayBrowser(selectedDock);
    selectedDock->setWidget(selectedBrowser);
    //    connect(browser, SIGNAL(wordClicked(TextInfo*)), selectedBrowser, SLOT(display(TextInfo*)));
    addDockWidget(Qt::RightDockWidgetArea, selectedDock);

    QDockWidget* netNoteDock = new QDockWidget("Net Notes", this);
    netNoteDock->setObjectName(netNoteDock->windowTitle());
    netNoteDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    NetNoteBrowser* netNoteBrowser = new NetNoteBrowser(netNoteDock);
    netNoteDock->setWidget(netNoteBrowser);
    addDockWidget(Qt::BottomDockWidgetArea, netNoteDock);

    QDockWidget* dictionaryDock = new QDockWidget("Dictionary", this);
    dictionaryDock->setObjectName(dictionaryDock->windowTitle());
    dictionaryDock->setAllowedAreas(Qt::AllDockWidgetAreas);

    /*QDockWidget* searchResultsDock = new QDockWidget("Search Results", this);
    searchResultsDock->setObjectName(searchResultsDock->windowTitle());
    searchResultsDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    searchBrowser = new SearchBrowser();
    searchResultsDock->setWidget(searchBrowser);
    addDockWidget(Qt::LeftDockWidgetArea, searchResultsDock);
    connect(searchBrowser, SIGNAL(goToResult(QList<int>, VerseReference)), browser, SLOT(display(QList<int>, VerseReference)));
    //    searchResultsDock->setFloating(true);*/

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
    //    connect(browser, SIGNAL(wordClicked(TextInfo*)), dictionaryBrowser, SLOT(display(TextInfo*)));
    addDockWidget(Qt::RightDockWidgetArea, dictionaryDock);


//    QToolBar* toolbar = new QToolBar();
//    toolbar->setObjectName("toolbar");
//    toolbar->layout()->setSpacing(3);

    //    QString activeTexts = settings.value(ACTIVE_TEXTS).toString();
    //    if (activeTexts.isNull())
    //    {
    //        activeTexts = "net#tisch#wlc";
    //    }
//    QString activeTexts = "net#tisch#wlc";

//    QStringList activeTextList = activeTexts.split("#");

//    QList<QString> textsAvailable;
//    textsAvailable.append("net");
//    textsAvailable.append("tisch");
//    textsAvailable.append("wlc");


    QVector<int> *bookChapterRange = new QVector<int>;
    bookChapterRange->append(50);
    bookChapterRange->append(40);
    bookChapterRange->append(27);
    bookChapterRange->append(36);
    bookChapterRange->append(34);
    bookChapterRange->append(24);
    bookChapterRange->append(21);
    bookChapterRange->append(4);
    bookChapterRange->append(31);
    bookChapterRange->append(24);
    bookChapterRange->append(22);
    bookChapterRange->append(25);
    bookChapterRange->append(29);
    bookChapterRange->append(36);
    bookChapterRange->append(10);
    bookChapterRange->append(13);
    bookChapterRange->append(10);
    bookChapterRange->append(42);
    bookChapterRange->append(150);
    bookChapterRange->append(31);
    bookChapterRange->append(12);
    bookChapterRange->append(8);
    bookChapterRange->append(66);
    bookChapterRange->append(52);
    bookChapterRange->append(5);
    bookChapterRange->append(48);
    bookChapterRange->append(12);
    bookChapterRange->append(14);
    bookChapterRange->append(3);
    bookChapterRange->append(9);
    bookChapterRange->append(1);
    bookChapterRange->append(4);
    bookChapterRange->append(7);
    bookChapterRange->append(3);
    bookChapterRange->append(3);
    bookChapterRange->append(3);
    bookChapterRange->append(2);
    bookChapterRange->append(14);
    bookChapterRange->append(4);
    bookChapterRange->append(28);
    bookChapterRange->append(16);
    bookChapterRange->append(24);
    bookChapterRange->append(21);
    bookChapterRange->append(28);
    bookChapterRange->append(16);
    bookChapterRange->append(16);
    bookChapterRange->append(13);
    bookChapterRange->append(6);
    bookChapterRange->append(6);
    bookChapterRange->append(4);
    bookChapterRange->append(4);
    bookChapterRange->append(5);
    bookChapterRange->append(3);
    bookChapterRange->append(6);
    bookChapterRange->append(4);
    bookChapterRange->append(3);
    bookChapterRange->append(1);
    bookChapterRange->append(13);
    bookChapterRange->append(5);
    bookChapterRange->append(5);
    bookChapterRange->append(3);
    bookChapterRange->append(5);
    bookChapterRange->append(1);
    bookChapterRange->append(1);
    bookChapterRange->append(1);
    bookChapterRange->append(22);

    QStringList *books = new QStringList();
    *books << "Genesis" << "Exodus" << "Leviticus" << "Numbers" << "Deuteronomy" << "Joshua" << "Judges" << "Ruth" << "1 Samuel" << "2 Samuel" << "1 Kings" << "2 Kings" << "1 Chronicles" << "2 Chronicles" << "Ezra" << "Nehemiah" << "Esther" << "Job" << "Psalms" << "Proverbs" << "Ecclesiastes" << "Song of Songs" << "Isaiah" << "Jeremiah" << "Lamentations" << "Ezekiel" << "Daniel" << "Hosea" << "Joel" << "Amos" << "Obadiah" << "Jonah" << "Micah" << "Nahum" << "Habakkuk" << "Zephaniah" << "Haggai" << "Zechariah" << "Malachi";
    *books << "Matthew" << "Mark" << "Luke" << "John" << "Acts" << "Romans" << "1 Corinthians" << "2 Corinthians" << "Galatians" << "Ephesians" << "Philippians" << "Colossians" << "1 Thessalonians" << "2 Thessalonians" << "1 Timothy" << "2 Timothy" << "Titus" << "Philemon" << "Hebrews" << "James" << "1 Peter" << "2 Peter" << "1 John" << "2 John" << "3 John" << "Jude" << "Revelation";
    qDebug() << books->at(0);
    //must be initialised before the toggleButtons get toggled since it receives focus whenever they're toggled
//    verseLineEdit = new LocationLineEdit(books, hash, bookChapterRange, 15, this);
//    verseLineEdit->setMaximumWidth(300);

//    for(int i=0; i< textsAvailable.size(); i++)
//    {
//        QPushButton* toggleButton = new QPushButton("&" + textsAvailable.at(i));
//        toggleButton->setCheckable(true);
//        connect(toggleButton, SIGNAL(toggled(bool)), this, SLOT(textToggled(bool)));
//        toolbar->addWidget(toggleButton);
//        toggleButton->setStyleSheet(
//                "QPushButton:hover {"
//                "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f3f0ef, stop: 1 #9f9fa8);"
//                "}"
//                "QPushButton:pressed {"
//                "padding-left: 5px;"
//                "padding-top: 5px;"
//                "border-top: 1px solid #6f6f71;"
//                "border-left: 1px solid #6f6f71;"
//                "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #dadbde, stop: 1 #f6f7fa);"
//                "}"
//                "QPushButton:checked {"
//                "padding-left: 3px;"
//                "padding-top: 4px;"
//                "border-top: 1px solid #6f6f71;"
//                "border-left: 1px solid #6f6f71;"
//                "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #d8d2c2, stop: 1 #62528d);"
//                "}"
//                "QPushButton:flat {"
//                "border: none;"
//                "}"
//                "QPushButton {"
//                "border-width: 1px;"
//                "border-color: #8f8f91;"
//                "border-top: 1px solid #afafb1;"
//                "border-left: 1px solid #afafb1;"
//                "border-style: solid;"
//                "margin: 2px;"
//                "border-radius: 5px; padding: 3px; left: -3px;"
//                "width: 90px;"
//                "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e3e0cd, stop: 1 #9797a0);"
//                "}"
//                );
//        if (activeTextList.contains(textsAvailable.at(i)))
//            toggleButton->setChecked(true);

//    }

        this->setStyleSheet("QMainWindow {background-color: qlineargradient(x1: 0.25, y1: 0, x2: 0.55, y2: 1, stop: 0 #f0ebe2, stop: 1 #ccc8c0);}");

//    verseLineOutput = new QLabel();


//    connect(verseLineEdit, SIGNAL(textEdited(QString)), this, SLOT(verseLineEditChanged(QString)));
//    connect(verseLineEdit, SIGNAL(returnPressed()), this, SLOT(performVerseLineEdit()));

//    toolbar->addWidget(verseLineEdit);
//    toolbar->addWidget(verseLineOutput);


//    addToolBar(toolbar);
    this->setWindowIcon(QIcon(DATA_PATH + "/verity.ico"));

    restoreState(settings.value(WINDOW_STATE_SETTING).toByteArray());

    settings.endGroup();



    EventManager::addListener(EventType::WORD_CLICKED, new WordClickedListener());

    EventManager::addListener(EventType::CLOSING, this);
}

//void MainWindow::textToggled(bool checked)
//{
//    QPushButton* sender = (QPushButton*)QObject::sender();

//    //when you clean your filthy code that uses the caption on the button you can clean this up too.
//    QString text = sender->text().mid(1);

//    QMap<QString, int> textToBibletextIdMap;
//    textToBibletextIdMap.insert("net", 1);
//    textToBibletextIdMap.insert("tisch", 2);
//    textToBibletextIdMap.insert("wlc", 3);

//    if(checked)
//    {
//        texts.append(textToBibletextIdMap.value(text));
//    }
//    else
//    {
//        texts.removeOne(textToBibletextIdMap.value(text));
//    }
//    searchBrowser->setTextsAvaiable(texts);

////    verseLineEdit->setFocus(Qt::OtherFocusReason);
//}

void MainWindow::afterShown()
{
//    verseLineEdit->setFocus();

//    VerseReference verseReference = VerseReferenceParser::parse("Mt1");
//    texts.append(1);
//    texts.append(2);
//    texts.append(3);
//    qDebug() << "mainwindow::aftershown - " << verseReference.stringRepresentation << " :: texts() " << texts.count();
//    bibleInterface->bibleWebView->display(this->texts, verseReference);
//    qDebug() << "mainwindow::aftershown - bada bing bada boom baby";
}

void MainWindow::handleEvent(Event* event)
{
    if(event->getEventType() == EventType::CLOSING)
    {
        QSettings settings(PROGRAM_NAME, PROGRAM_NAME);

        settings.beginGroup(MAIN_WINDOW_SETTING_GROUP);
        //        settings.setValue(SIZE_SETTING, size());
        //        settings.setValue(POS_SETTING, pos());

        //        QString t;
        //        for (int i = 0; i < texts.count(); i++)
        //        {
        //            t.append(texts.at(i));
        //            t.append("#");
        //        }
        //        settings.setValue(ACTIVE_TEXTS, t.left(t.length() - 1));

        //        settings.setValue(WINDOW_STATE_SETTING, (windowState() & Qt::WindowMaximized) > 0);

        settings.setValue(GEOMETRY_SETTING, saveGeometry());
        settings.setValue(WINDOW_STATE_SETTING, saveState());

        //        settings.setValue(DATA_PATH_SETTING, DATA_PATH);

        settings.endGroup();

    }
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    (new BasicEvent(EventType::CLOSING))->fire();

    event->accept();
}

//void MainWindow::performVerseLineEdit()
//{
//    QString term = verseLineEdit->text();
//    if (term.left(2) == "s.")
//    {
//        searchBrowser->performSearch(term);
//    }
//    else if(term.length() > 0)
//    {
//        VerseReference verseReference = VerseReferenceParser::parse(term);

//        //        QMap<QString, int> textToBibletextIdMap;
//        //        textToBibletextIdMap.insert("net", 1);
//        //        textToBibletextIdMap.insert("tisch", 2);
//        //        textToBibletextIdMap.insert("wlc", 3);

//        //        QList<int> bibletextIds;
//        //        QString text;
//        //        foreach(text, texts)
//        //        {
//        //            bibletextIds.append(textToBibletextIdMap.value(text));
//        //        }

//        browser->display(texts, verseReference);
//    }
//}


MainWindow::~MainWindow()
{

}

void MainWindow::keyPressEvent(QKeyEvent* keyEvent)
{
    if(keyEvent->modifiers() & Qt::ControlModifier && keyEvent->key() == Qt::Key_L)
    {
//        verseLineEdit->setFocus();
//        verseLineEdit->selectAll();
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

//void MainWindow::verseLineEditChanged(QString string)
//{
//    if(string.length() > 0)
//    {
//        if (string.left(2) == "s.")
//        {
//            if (verseLineOutput->text() != "will search...")
//            {
//                verseLineOutput->setText("will search...");
//            }
//        }
//        else
//        {
//            //this is what used to be here:
//            //           verseLineOutput->setText(VerseReferenceParser::parse(string).stringRepresentation);

//            /*
//    //this code is ripped from the CompleteLineEdit but it could be useful here

//    QStringList words = QStringList()  << "Genesis" << "Exodus" << "Leviticus" << "Numbers" << "Deuteronomy" << "Joshua" << "Judges" << "Ruth" << "1 Samuel" << "2 Samuel" << "1 Kings" << "2 Kings" << "1 Chronicles" << "2 Chronicles" << "Ezra" << "Nehemiah" << "Esther" << "Job" << "Psalms" << "Proverbs" << "Ecclesiastes" << "Song of Songs" << "Isaiah" << "Jeremiah" << "Lamentations" << "Ezekiel" << "Daniel" << "Hosea" << "Joel" << "Amos" << "Obadiah" << "Jonah" << "Micah" << "Nahum" << "Habakkuk" << "Zephaniah" << "Haggai" << "Zechariah" << "Malachi";
//    words << "Matthew" << "Mark" << "Luke" << "John" << "Acts" << "Romans" << "1 Corinthians" << "2 Corinthians" << "Galatians" << "Ephesians" << "Philippians" << "Colossians" << "1 Thessalonians" << "2 Thessalonians" << "1 Timothy" << "2 Timothy" << "Titus" << "Philemon" << "Hebrews" << "James" << "1 Peter" << "2 Peter" << "1 John" << "2 John" << "3 John" << "Jude" << "Revelation";

//    QRegExp rx = QRegExp("^(\\d\\ )?[a-zA-Z]*\\b");
//    rx.exactMatch(string);
//    if (words.contains(rx.cap(0)))
//    {
//        QString book = rx.cap(0);
//        QString textWithoutBook = string.mid(book.length());
//        rx = QRegExp("^\\ ?(\\d{1,3})\\D(\\d{1,3})$");
//        rx.exactMatch(textWithoutBook);
//        qDebug() << (rx.cap(1).length() > 0 && rx.cap(2) > 0);
//        if (rx.cap(1).length() > 0 && rx.cap(2) > 0)
//        {
//            verseLineOutput->setText("Destination: " + book + " " + rx.cap(1) + ":" + rx.cap(2));
//        }
//    }
//*/
//        }
//    }
//    else
//    {
//        verseLineOutput->setText("");
//    }
//}
