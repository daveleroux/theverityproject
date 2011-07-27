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

//        DATA_PATH = settings.value(DATA_PATH_SETTING, "/usr/share/verity").toString();
    DATA_PATH = ".";
    qDebug() << "data path:" << DATA_PATH;


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
    connect(searchBrowser, SIGNAL(goToResult(QList<int>, VerseReference)), browser, SLOT(display(QList<int>, VerseReference)));
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

    QString activeTexts = settings.value(ACTIVE_TEXTS).toString();
    if (activeTexts.isNull())
    {
        activeTexts = "net#tisch";
    }
    QStringList activeTextList = activeTexts.split("#");

    QList<QString> textsAvailable;
    textsAvailable.append("net");
    textsAvailable.append("tisch");
    textsAvailable.append("wlc");


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
    QHash<QString, int> *hash;
    hash = new QHash<QString, int>;

    hash->insert("genesis", 1   );
    hash->insert("gen", 1   );
    hash->insert("exodus", 2    );
    hash->insert("ex", 2    );
    hash->insert("leviticus",3    );
    hash->insert("lev",3    );
    hash->insert("numbers",4    );
    hash->insert("num",4    );
    hash->insert("deuteronomy",5    );
    hash->insert("deut",5    );
    hash->insert("joshua",6    );
    hash->insert("josh",6    );
    hash->insert("judges",7    );
    hash->insert("jud",7    );
    hash->insert("ruth",8    );
    hash->insert("rut",8    );
    hash->insert("1 samuel",9    );
    hash->insert("1sam",9    );
    hash->insert("2 samuel",10    );
    hash->insert("2sam",10    );
    hash->insert("1 kings",11    );
    hash->insert("1ki",11    );
    hash->insert("2 kings",12    );
    hash->insert("2ki",12    );
    hash->insert("1 chronicles",13    );
    hash->insert("1ch",13    );
    hash->insert("2 chronicles",14    );
    hash->insert("2ch",14    );
    hash->insert("ezra",15    );
    hash->insert("ez",15    );
    hash->insert("nehemiah",16    );
    hash->insert("neh",16    );
    hash->insert("esther",17    );
    hash->insert("est",17    );
    hash->insert("job",18    );
    hash->insert("psalms",19    );
    hash->insert("ps",19    );
    hash->insert("proverbs",20    );
    hash->insert("pr",20    );
    hash->insert("ecclesiastes",21    );
    hash->insert("ecc",21    );
    hash->insert("song of songs",22    );
    hash->insert("ss",22    );
    hash->insert("isaiah",23    );
    hash->insert("is",23    );
    hash->insert("jeremiah",24    );
    hash->insert("jer",24    );
    hash->insert("lamentations",25    );
    hash->insert("lam",25    );
    hash->insert("ezekiel",26    );
    hash->insert("ez",26    );
    hash->insert("daniel",27    );
    hash->insert("dan",27    );
    hash->insert("hosea",28    );
    hash->insert("hos",28    );
    hash->insert("joel",29    );
    hash->insert("amos",30    );
    hash->insert("obadiah",31    );
    hash->insert("ob",31    );
    hash->insert("jonah",32    );
    hash->insert("micah",33    );
    hash->insert("mic",33    );
    hash->insert("nahum",34    );
    hash->insert("nah",34    );
    hash->insert("habakkuk",35    );
    hash->insert("hab",35    );
    hash->insert("zephaniah",36    );
    hash->insert("zep",36    );
    hash->insert("haggai",37    );
    hash->insert("hag",37    );
    hash->insert("zechariah",38    );
    hash->insert("zec",38    );
    hash->insert("malachi",39    );
    hash->insert("mal",39    );


    hash->insert( "matthew" ,  40 );
    hash->insert( "matt" ,  40 );
    hash->insert( "mat" ,  40 );
    hash->insert( "mt" ,  40 );
    hash->insert( "mark" ,  41 );
    hash->insert( "mrk" ,  41 );
    hash->insert( "mk" ,  41 );
    hash->insert( "mr" ,  41 );
    hash->insert( "luke" ,  42 );
    hash->insert( "luk" ,  42 );
    hash->insert( "lk" ,  42 );
    hash->insert( "john" ,  43 );
    hash->insert( "joh" ,  43 );
    hash->insert( "jn" ,  43 );
    hash->insert( "jhn" ,  43 );
    hash->insert( "acts" ,  44 );
    hash->insert( "ac" ,  44 );
    hash->insert( "romans" ,  45 );
    hash->insert( "rom" ,  45 );
    hash->insert( "ro" ,  45 );
    hash->insert( "rm" ,  45 );
    hash->insert( "1 corinthians" ,  46 );
    hash->insert( "1 cor" ,  46 );
    hash->insert( "1 co" ,  46 );
    hash->insert( "i co" ,  46 );
    hash->insert( "1co" ,  46 );
    hash->insert( "i cor" ,  46 );
    hash->insert( "1cor" ,  46 );
    hash->insert( "i corinthians" ,  46 );
    hash->insert( "1corinthians" ,  46 );
    hash->insert( "1st corinthians" ,  46 );
    hash->insert( "first corinthians" ,  46 );
    hash->insert( "2 corinthians" ,  47 );
    hash->insert( "2 cor" ,  47 );
    hash->insert( "2 co" ,  47 );
    hash->insert( "ii co" ,  47 );
    hash->insert( "2co" ,  47 );
    hash->insert( "ii cor" ,  47 );
    hash->insert( "2cor" ,  47 );
    hash->insert( "ii corinthians" ,  47 );
    hash->insert( "2corinthians" ,  47 );
    hash->insert( "2nd corinthians" ,  47 );
    hash->insert( "second corinthians" ,  47 );
    hash->insert( "galatians" ,  48 );
    hash->insert( "gal" ,  48 );
    hash->insert( "ga" ,  48 );
    hash->insert( "ephesians" ,  49 );
    hash->insert( "ephes" ,  49 );
    hash->insert( "eph" ,  49 );
    hash->insert( "philippians" ,  50 );
    hash->insert( "phil" ,  50 );
    hash->insert( "php" ,  50 );
    hash->insert( "colossians" ,  51 );
    hash->insert( "col" ,  51 );
    hash->insert( "1 thessalonians" ,  52 );
    hash->insert( "1 thess" ,  52 );
    hash->insert( "1 thes" ,  52 );
    hash->insert( "1 th" ,  52 );
    hash->insert( "i th" ,  52 );
    hash->insert( "1th" ,  52 );
    hash->insert( "i thes" ,  52 );
    hash->insert( "1thes" ,  52 );
    hash->insert( "i thess" ,  52 );
    hash->insert( "1thess" ,  52 );
    hash->insert( "i thessalonians" ,  52 );
    hash->insert( "1thessalonians" ,  52 );
    hash->insert( "1st thessalonians" ,  52 );
    hash->insert( "first thessalonians" ,  52 );
    hash->insert( "2 thessalonians" ,  53 );
    hash->insert( "2 thess" ,  53 );
    hash->insert( "2 th" ,  53 );
    hash->insert( "ii th" ,  53 );
    hash->insert( "2th" ,  53 );
    hash->insert( "ii thes" ,  53 );
    hash->insert( "2thes" ,  53 );
    hash->insert( "ii thess" ,  53 );
    hash->insert( "2thess" ,  53 );
    hash->insert( "ii thessalonians" ,  53 );
    hash->insert( "2thessalonians" ,  53 );
    hash->insert( "2nd thessalonians" ,  53 );
    hash->insert( "second thessalonians" ,  53 );
    hash->insert( "1 timothy" ,  54 );
    hash->insert( "1 tim" ,  54 );
    hash->insert( "1 ti" ,  54 );
    hash->insert( "i ti" ,  54 );
    hash->insert( "1ti" ,  54 );
    hash->insert( "i tim" ,  54 );
    hash->insert( "1tim" ,  54 );
    hash->insert( "i timothy" ,  54 );
    hash->insert( "1timothy" ,  54 );
    hash->insert( "1st timothy" ,  54 );
    hash->insert( "first timothy" ,  54 );
    hash->insert( "2 timothy" ,  55 );
    hash->insert( "2 tim" ,  55 );
    hash->insert( "2 ti" ,  55 );
    hash->insert( "ii ti" ,  55 );
    hash->insert( "2ti" ,  55 );
    hash->insert( "ii tim" ,  55 );
    hash->insert( "2tim" ,  55 );
    hash->insert( "ii timothy" ,  55 );
    hash->insert( "2timothy" ,  55 );
    hash->insert( "2nd timothy" ,  55 );
    hash->insert( "second timothy" ,  55 );
    hash->insert( "titus" ,  56 );
    hash->insert( "tit" ,  56 );
    hash->insert( "philemon" ,  57 );
    hash->insert( "philem" ,  57 );
    hash->insert( "phm" ,  57 );
    hash->insert( "hebrews" ,  58 );
    hash->insert( "heb" ,  58 );
    hash->insert( "james" ,  59 );
    hash->insert( "jas" ,  59 );
    hash->insert( "jm" ,  59 );
    hash->insert( "1 peter" ,  60 );
    hash->insert( "1 pet" ,  60 );
    hash->insert( "1 pe" ,  60 );
    hash->insert( "i pe" ,  60 );
    hash->insert( "1pe" ,  60 );
    hash->insert( "i pet" ,  60 );
    hash->insert( "1pet" ,  60 );
    hash->insert( "i pt" ,  60 );
    hash->insert( "1 pt" ,  60 );
    hash->insert( "1pt" ,  60 );
    hash->insert( "i peter" ,  60 );
    hash->insert( "1peter" ,  60 );
    hash->insert( "1st peter" ,  60 );
    hash->insert( "first peter" ,  60 );
    hash->insert( "2 peter" ,  61 );
    hash->insert( "2 pet" ,  61 );
    hash->insert( "2 pe" ,  61 );
    hash->insert( "ii pe" ,  61 );
    hash->insert( "2pe" ,  61 );
    hash->insert( "ii pet" ,  61 );
    hash->insert( "2pet" ,  61 );
    hash->insert( "ii pt" ,  61 );
    hash->insert( "2 pt" ,  61 );
    hash->insert( "2pt" ,  61 );
    hash->insert( "ii peter" ,  61 );
    hash->insert( "2peter" ,  61 );
    hash->insert( "2nd peter" ,  61 );
    hash->insert( "second peter" ,  61 );
    hash->insert( "1 john" ,  62 );
    hash->insert( "1 jn" ,  62 );
    hash->insert( "i jn" ,  62 );
    hash->insert( "1jn" ,  62 );
    hash->insert( "i jo" ,  62 );
    hash->insert( "1jo" ,  62 );
    hash->insert( "i joh" ,  62 );
    hash->insert( "1joh" ,  62 );
    hash->insert( "i jhn" ,  62 );
    hash->insert( "1 jhn" ,  62 );
    hash->insert( "1jhn" ,  62 );
    hash->insert( "i john" ,  62 );
    hash->insert( "1john" ,  62 );
    hash->insert( "1st john" ,  62 );
    hash->insert( "first john" ,  62 );
    hash->insert( "2 john" ,  63 );
    hash->insert( "2 jn" ,  63 );
    hash->insert( "ii jn" ,  63 );
    hash->insert( "2jn" ,  63 );
    hash->insert( "ii jo" ,  63 );
    hash->insert( "2jo" ,  63 );
    hash->insert( "ii joh" ,  63 );
    hash->insert( "2joh" ,  63 );
    hash->insert( "ii jhn" ,  63 );
    hash->insert( "2 jhn" ,  63 );
    hash->insert( "2jhn" ,  63 );
    hash->insert( "ii john" ,  63 );
    hash->insert( "2john" ,  63 );
    hash->insert( "2nd john" ,  63 );
    hash->insert( "second john" ,  63 );
    hash->insert( "3 john" ,  64 );
    hash->insert( "3 jn" ,  64 );
    hash->insert( "iii jn" ,  64 );
    hash->insert( "3jn" ,  64 );
    hash->insert( "iii jo" ,  64 );
    hash->insert( "3jo" ,  64 );
    hash->insert( "iii joh" ,  64 );
    hash->insert( "3joh" ,  64 );
    hash->insert( "iii jhn" ,  64 );
    hash->insert( "3 jhn" ,  64 );
    hash->insert( "3jhn" ,  64 );
    hash->insert( "iii john" ,  64 );
    hash->insert( "3john" ,  64 );
    hash->insert( "3rd john" ,  64 );
    hash->insert( "third john" ,  64 );
    hash->insert( "jude" ,  65 );
    hash->insert( "jud" ,  65 );
    hash->insert( "revelation" ,  66);
    hash->insert( "rev" ,  66 );
    hash->insert( "re" ,  66 );
    QStringList *books = new QStringList();
    *books << "Genesis" << "Exodus" << "Leviticus" << "Numbers" << "Deuteronomy" << "Joshua" << "Judges" << "Ruth" << "1 Samuel" << "2 Samuel" << "1 Kings" << "2 Kings" << "1 Chronicles" << "2 Chronicles" << "Ezra" << "Nehemiah" << "Esther" << "Job" << "Psalms" << "Proverbs" << "Ecclesiastes" << "Song of Songs" << "Isaiah" << "Jeremiah" << "Lamentations" << "Ezekiel" << "Daniel" << "Hosea" << "Joel" << "Amos" << "Obadiah" << "Jonah" << "Micah" << "Nahum" << "Habakkuk" << "Zephaniah" << "Haggai" << "Zechariah" << "Malachi";
    *books << "Matthew" << "Mark" << "Luke" << "John" << "Acts" << "Romans" << "1 Corinthians" << "2 Corinthians" << "Galatians" << "Ephesians" << "Philippians" << "Colossians" << "1 Thessalonians" << "2 Thessalonians" << "1 Timothy" << "2 Timothy" << "Titus" << "Philemon" << "Hebrews" << "James" << "1 Peter" << "2 Peter" << "1 John" << "2 John" << "3 John" << "Jude" << "Revelation";
    qDebug() << books->at(0);
    //must be initialised before the toggleButtons get toggled since it receives focus whenever they're toggled
    verseLineEdit = new LocationLineEdit(books, hash, bookChapterRange, 15, this);
    verseLineEdit->setMaximumWidth(300);

    for(int i=0; i< textsAvailable.size(); i++)
    {
        QPushButton* toggleButton = new QPushButton("&" + textsAvailable.at(i));
        toggleButton->setCheckable(true);
        connect(toggleButton, SIGNAL(toggled(bool)), this, SLOT(textToggled(bool)));
        toolbar->addWidget(toggleButton);
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
        if (activeTextList.contains(textsAvailable.at(i)))
            toggleButton->setChecked(true);

    }

    this->setStyleSheet("QMainWindow {background-color: qlineargradient(x1: 0.25, y1: 0, x2: 0.55, y2: 1, stop: 0 #f0ebe2, stop: 1 #ccc8c0);}");

    verseLineOutput = new QLabel();


    connect(verseLineEdit, SIGNAL(textEdited(QString)), this, SLOT(verseLineEditChanged(QString)));
    connect(verseLineEdit, SIGNAL(returnPressed()), this, SLOT(performVerseLineEdit()));

    toolbar->addWidget(verseLineEdit);
    toolbar->addWidget(verseLineOutput);


    addToolBar(toolbar);
    this->setWindowIcon(QIcon(DATA_PATH + "/verity.ico"));
    settings.endGroup();
}

void MainWindow::textToggled(bool checked)
{
    QPushButton* sender = (QPushButton*)QObject::sender();

    //when you clean your filthy code that uses the caption on the button you can clean this up too.
    QString text = sender->text().mid(1);

    QMap<QString, int> textToBibletextIdMap;
    textToBibletextIdMap.insert("net", 1);
    textToBibletextIdMap.insert("tisch", 2);
    textToBibletextIdMap.insert("wlc", 3);

    if(checked)
    {
        texts.append(textToBibletextIdMap.value(text));
    }
    else
    {
        texts.removeOne(textToBibletextIdMap.value(text));
    }
    searchBrowser->setTextsAvaiable(texts);

    verseLineEdit->setFocus(Qt::OtherFocusReason);
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
    QString t;
    for (int i = 0; i < texts.count(); i++)
    {
        t.append(texts.at(i));
        t.append("#");
    }
    settings.setValue(ACTIVE_TEXTS, t.left(t.length() - 1));
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
        VerseReference verseReference = VerseReferenceParser::parse(term);

//        QMap<QString, int> textToBibletextIdMap;
//        textToBibletextIdMap.insert("net", 1);
//        textToBibletextIdMap.insert("tisch", 2);
//        textToBibletextIdMap.insert("wlc", 3);

//        QList<int> bibletextIds;
//        QString text;
//        foreach(text, texts)
//        {
//            bibletextIds.append(textToBibletextIdMap.value(text));
//        }

        browser->display(texts, verseReference);
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
            //this is what used to be here:
//           verseLineOutput->setText(VerseReferenceParser::parse(string).stringRepresentation);

           /*
    //this code is ripped from the CompleteLineEdit but it could be useful here

    QStringList words = QStringList()  << "Genesis" << "Exodus" << "Leviticus" << "Numbers" << "Deuteronomy" << "Joshua" << "Judges" << "Ruth" << "1 Samuel" << "2 Samuel" << "1 Kings" << "2 Kings" << "1 Chronicles" << "2 Chronicles" << "Ezra" << "Nehemiah" << "Esther" << "Job" << "Psalms" << "Proverbs" << "Ecclesiastes" << "Song of Songs" << "Isaiah" << "Jeremiah" << "Lamentations" << "Ezekiel" << "Daniel" << "Hosea" << "Joel" << "Amos" << "Obadiah" << "Jonah" << "Micah" << "Nahum" << "Habakkuk" << "Zephaniah" << "Haggai" << "Zechariah" << "Malachi";
    words << "Matthew" << "Mark" << "Luke" << "John" << "Acts" << "Romans" << "1 Corinthians" << "2 Corinthians" << "Galatians" << "Ephesians" << "Philippians" << "Colossians" << "1 Thessalonians" << "2 Thessalonians" << "1 Timothy" << "2 Timothy" << "Titus" << "Philemon" << "Hebrews" << "James" << "1 Peter" << "2 Peter" << "1 John" << "2 John" << "3 John" << "Jude" << "Revelation";

    QRegExp rx = QRegExp("^(\\d\\ )?[a-zA-Z]*\\b");
    rx.exactMatch(string);
    if (words.contains(rx.cap(0)))
    {
        QString book = rx.cap(0);
        QString textWithoutBook = string.mid(book.length());
        rx = QRegExp("^\\ ?(\\d{1,3})\\D(\\d{1,3})$");
        rx.exactMatch(textWithoutBook);
        qDebug() << (rx.cap(1).length() > 0 && rx.cap(2) > 0);
        if (rx.cap(1).length() > 0 && rx.cap(2) > 0)
        {
            verseLineOutput->setText("Destination: " + book + " " + rx.cap(1) + ":" + rx.cap(2));
        }
    }
*/
        }
    }
    else
    {
        verseLineOutput->setText("");
    }
}
