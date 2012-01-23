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
#include "searchdisplaybrowser.h"

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
#include <QMenuBar>
#include <QMessageBox>

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
    bibleInterface = new VBibleInterface(this);
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

    QDockWidget* searchResultsDock = new QDockWidget("Search Results", this);
    searchResultsDock->setObjectName(searchResultsDock->windowTitle());
    searchResultsDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    SearchDisplayBrowser* searchDisplayBrowser = new SearchDisplayBrowser(searchResultsDock);
    searchResultsDock->setWidget(searchDisplayBrowser);
    addDockWidget(Qt::BottomDockWidgetArea, searchResultsDock);

    QDockWidget* netNoteDock = new QDockWidget("Net Notes", this);
    netNoteDock->setObjectName(netNoteDock->windowTitle());
    netNoteDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    NetNoteBrowser* netNoteBrowser = new NetNoteBrowser(netNoteDock);
    netNoteDock->setWidget(netNoteBrowser);
    addDockWidget(Qt::BottomDockWidgetArea, netNoteDock);

    tabifyDockWidget(netNoteDock, searchResultsDock);
    netNoteDock->raise();

    QDockWidget* dictionaryDock = new QDockWidget("Dictionary", this);
    dictionaryDock->setObjectName(dictionaryDock->windowTitle());
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
    //    connect(browser, SIGNAL(wordClicked(TextInfo*)), dictionaryBrowser, SLOT(display(TextInfo*)));
    addDockWidget(Qt::RightDockWidgetArea, dictionaryDock);

    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(selectedDock->toggleViewAction());
    viewMenu->addAction(netNoteDock->toggleViewAction());
    viewMenu->addAction(dictionaryDock->toggleViewAction());
    viewMenu->addAction(searchResultsDock->toggleViewAction());

    QMenu* copyrightMenu = menuBar()->addMenu(tr("&Copyright")); //this should perhaps be done some other way so texts can be added without a recompile e.g. in the db

    QAction* netAction = new QAction("NET Bible", this);
    connect(netAction, SIGNAL(triggered()), this, SLOT(netCopyright()));
    copyrightMenu->addAction(netAction);

    QAction* wlcAction = new QAction(tr("Westminster Leningrad Codex (Hebrew Old Testament)"), this);
    connect(wlcAction, SIGNAL(triggered()), this, SLOT(wlcCopyright()));
    copyrightMenu->addAction(wlcAction);

    QAction* tischAction = new QAction("Tischendorf (Greek New Testament)", this);
    connect(tischAction, SIGNAL(triggered()), this, SLOT(tischCopyright()));
    copyrightMenu->addAction(tischAction);

    QAction* strongsHebrewAction = new QAction("Strong's Hebrew Dictionary", this);
    connect(strongsHebrewAction, SIGNAL(triggered()), this, SLOT(strongsHebrewCopyright()));
    copyrightMenu->addAction(strongsHebrewAction);

    QAction* strongsGreekAction = new QAction("Strong's Greek Dictionary", this);
    connect(strongsGreekAction, SIGNAL(triggered()), this, SLOT(strongsGreekCopyright()));
    copyrightMenu->addAction(strongsGreekAction);

    QMenu* aboutMenu = menuBar()->addMenu(tr("&About"));

    QAction* verityAction = new QAction("The Verity Project", this);
    connect(verityAction, SIGNAL(triggered()), this, SLOT(verityAbout()));
    aboutMenu->addAction(verityAction);



    this->setStyleSheet("QMainWindow {background-color: qlineargradient(x1: 0.25, y1: 0, x2: 0.55, y2: 1, stop: 0 #f0ebe2, stop: 1 #ccc8c0);}");

    this->setWindowIcon(QIcon(DATA_PATH + "/verity.ico"));

    restoreState(settings.value(WINDOW_STATE_SETTING).toByteArray());

    settings.endGroup();



    EventManager::addListener(EventType::WORD_CLICKED, new WordClickedListener());

    EventManager::addListener(EventType::CLOSING, this);
}

void MainWindow::netCopyright()
{
    QMessageBox::information(this, "NET Bible", "<html>"
"<h4>NET Bible&reg;</h4>"
"copyright &copy;1996-2006 by Biblical Studies Press, L.L.C. "
"<a href=\"http://bible.org\">http://bible.org</a><br/>All rights reserved.<br/><br/>"
"The NET Bible is available in its entirety as a free download or online web use at <a href=\"http://bible.org\">http://bible.org</a>"
"</html>");
}

void MainWindow::wlcCopyright()
{
    QMessageBox::information(this, "Westminster Leningrad Codex",
"<html>"
"<h4>Westminster Leningrad Codex</h4>"
"Contributing Editors: Daniel Owens, David Troidl, Christopher V. Kimball"
"<br/><a href=\"http://www.tanach.us/Tanach.xml\">http://www.tanach.us/Tanach.xml</a>"
"<br/>Public Domain"

"<h4>BHS unaccented with Strongs</h4>"
"Unaccented Biblia Hebraica Stuttgartensia Hebrew with Strong's numbers by Rev. Graham."
"<br/>Contributing Editors: Joseph Planeta, Rev. Graham Nind"
"<br/><a href=\"http://www.faithofgod.net/\">http://www.faithofgod.net/</a>"
"<br/><a href=\"http://www.grahamnind.dsl.pipex.com/files\">http://www.grahamnind.dsl.pipex.com/files</a>"
"<br/>Public Domain"
"</html>");
}

void MainWindow::tischCopyright()
{
    QMessageBox::information(this, "Tischendorf's 8th edition",
"<html>"
"<h4>Tischendorf's 8th edition</h4>"
"Greek New Testament with morphological tags"
"<br/>Based on G. Clint Yale's Tischendorf text and on Dr. Maurice A. Robinson's Westcott-Hort text"
"<br/>Edited by Ulrik Sandborg-Petersen"
"<br/>Public Domain"
"</html>");
}

void MainWindow::strongsHebrewCopyright()
{
    QMessageBox::information(this, "Strong's Hebrew Dictionary",
"<html>"
"<h4>A Concise Dictionary of the Words in the Hebrew Bible</h4>"
"with their Renderings in the King James Version"
"<br/><br/>Contributing Editors: David Troidl, David Instone-Brewer"
"<br/>Author: James Strong, LL.D., S.T.D. 1894"
"<br/><a href=\"www.2LetterLookup.com\">www.2LetterLookup.com</a>"
"<br/>Public Domain"
"</html>");
}

void MainWindow::strongsGreekCopyright()
{
    QMessageBox::information(this, "Strong's Greek Dictionary",
"<html>"
"<h4>Dictionary of Greek Words</h4>"
"Taken from Strong's Exhaustive Concordance"
"<br/>Author: James Strong, S.T.D., LL.D. 1890"
"<br/>Contributing Editor: Ulrik Petersen 2006"
"<br/><a href=\"http://ulrikp.org\">http://ulrikp.org</a>"
"<br/>based on work by Michael Grier"
"<br/><a href=\"http://www.bf.org/\">http://www.bf.org/</a>"
"<br/>Public Domain"
"</html>");
}

void MainWindow::verityAbout()
{
    QMessageBox::information(this, "The Verity Project",
"<html>"
"<h4>The Verity Project</h4>"
"Soli Deo Gloria"
"<br/><br/>Written by David le Roux and James Cu&eacute;nod"
"<br/><br/><a href=\"http://www.theverityproject.com/\">http://www.theverityproject.com/</a>"
"<br/><br/>GNU General Public License Version 3"
"</html>");
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
