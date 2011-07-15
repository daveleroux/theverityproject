#include "mainwindow.h"
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QFile>
#include <QTextStream>
#include "clicklistener.h"
#include "scrolllistener.h"
#include <QMessageBox>
#include <QtSql>
#include <QXmlQuery>
#include <QWebElement>


QString MainWindow::getHtml(int normalisedChapter)
{
    QSqlQuery query;

    if(!query.exec("select text from net_bible where normalised_chapter=" + QString().setNum(normalisedChapter)))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

    QString wholeChapter = "<normalisedChapter>";
    while(query.next())
    {
        QString text = query.value(0).toString();
        wholeChapter = wholeChapter + text;
    }
    wholeChapter = wholeChapter + "</normalisedChapter>";


    QXmlQuery xmlQuery(QXmlQuery::XSLT20);

    QByteArray byteArray(wholeChapter.toUtf8());

    QBuffer buffer(&byteArray);
    buffer.open(QBuffer::ReadWrite);

    xmlQuery.setFocus(&buffer);

    xmlQuery.setQuery(QUrl("qrc:/xsl/bible.xsl"));

    QString result;
    xmlQuery.evaluateTo(&result);

    return result.simplified();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("verity.sqlite");
    if (!db.open())
    {
        qDebug() << "couldn't open db" << endl;
        exit(1);
    }

    webView = new QWebView();
    setCentralWidget(webView);
    webView->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

    waitingUntilScrollbarMaxChanged = false;
    mustScrollDown = false;
    mustScrollToPoint = false;

    connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));

    QString htmlFrame = "<html>"
                        "<head>"
                        "<link href=\"qrc:/style/bible.css\" rel=\"stylesheet\" type=\"text/css\"/>"
                        "<script type=\"text/javascript\">"
                        //                        "function create(htmlStr) "
                        //                        "{"
                        //                        "   var frag = document.createDocumentFragment(),"
                        //                        "       temp = document.createElement('div');"
                        //                        "   temp.innerHTML = htmlStr;"
                        //                        "   while (temp.firstChild) {"
                        //                        "       frag.appendChild(temp.firstChild);"
                        //                        "   }"
                        //                        "   return frag;"
                        //                        "}"
                        "</script>"
                        "</head>"
                        "<body>"
                        "</body>"
                        "</html>";


    webView->setHtml(htmlFrame);

}

bool MainWindow::mustAppend()
{
    int value =  webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);
    int height =  webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height();
    int max =  webView->page()->mainFrame()->scrollBarMaximum(Qt::Vertical);

    return max - value < height;
}

bool MainWindow::mustPrepend()
{
    int value =  webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);
    int height =  webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height();

    return value < height;
}

bool MainWindow::validChapter(int proposedChapter)
{
    return proposedChapter <= 1189 && proposedChapter >= 1;
}

void MainWindow::timerEvent (QTimerEvent * event)
{
    if(waitingUntilScrollbarMaxChanged)
    {
        int maxNow = webView->page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
        if(maxNow != currentScrollMax)
        {
            basicTimer.stop();
            if(mustScrollDown)
            {
                webView->page()->mainFrame()->scroll(0, maxNow-currentScrollMax);
                mustScrollDown = false;
            }

            if(mustScrollToPoint)
            {
//                webView->page()->mainFrame()->scroll(0, maxNow-currentScrollMax);
                webView->page()->mainFrame()->setScrollPosition(QPoint(0,pointToScrollTo+40));
                mustScrollToPoint = false;
            }

            int heightNow = webView->page()->mainFrame()->contentsSize().height();
            if(heightNow > currentHeight)
            {
                lastAddedChapterInfo->height = heightNow - currentHeight;
            }

            waitingUntilScrollbarMaxChanged = false;
            checkCanScroll();
        }
    }
}

bool MainWindow::canUnloadFirstChapter()
{
    if(webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height() > 0)
    {
        //fake remove it and see if 'mustPrepend', if so then don't remove it

        int totalDocumentHeight = webView->page()->mainFrame()->contentsSize().height();
        int firstChapterHeight = normalisedChapters.first()->height;


        int scrollbarValueNow =  webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);
        int scrollbarHeightNow =  webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height();
        int scrollbarMaxNow =  webView->page()->mainFrame()->scrollBarMaximum(Qt::Vertical);

        float ratio = totalDocumentHeight/((float)(scrollbarMaxNow + scrollbarHeightNow));

        pointToScrollTo = scrollbarValueNow - firstChapterHeight/ratio;

        return !(ratio*scrollbarValueNow - firstChapterHeight < ratio*scrollbarHeightNow);
    }
    return false;
}

bool MainWindow::canUnloadLastChapter()
{
    if(webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height() > 0)
    {
        //fake remove it and see if 'mustAppend', if so then don't remove it

        int totalDocumentHeight = webView->page()->mainFrame()->contentsSize().height();
        int lastChapterHeight = normalisedChapters.last()->height;
        int proposedDocumentHeight = totalDocumentHeight-lastChapterHeight;

        int scrollbarValueNow =  webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);
        int scrollbarHeightNow =  webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height();
        int scrollbarMaxNow =  webView->page()->mainFrame()->scrollBarMaximum(Qt::Vertical);

        int proposedMax = ((scrollbarMaxNow+scrollbarHeightNow)*proposedDocumentHeight)/((float)totalDocumentHeight) - scrollbarHeightNow;

        return !(proposedMax - scrollbarValueNow < scrollbarHeightNow);
    }
    return false;
}

void MainWindow::unloadFirstChapter()
{
    qDebug() << "first chapter unloaded";
    delete normalisedChapters.first();
    normalisedChapters.removeFirst();

    QWebElement doc = webView->page()->mainFrame()->documentElement();
    QWebElement body = doc.firstChild().nextSibling();
    body.firstChild().removeFromDocument();
}

void MainWindow::unloadLastChapter()
{
    qDebug() << "last chapter unloaded";
    delete normalisedChapters.last();
    normalisedChapters.removeLast();

    QWebElement doc = webView->page()->mainFrame()->documentElement();
    QWebElement body = doc.firstChild().nextSibling();
    body.lastChild().removeFromDocument();
}

void MainWindow::checkCanScroll()
{
    if(!waitingUntilScrollbarMaxChanged)
    {
        if(mustPrepend())
        {
            if(validChapter(normalisedChapters.first()->normalisedChapter-1))
            {
                currentScrollMax = webView->page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
                currentHeight = webView->page()->mainFrame()->contentsSize().height();
                prependChapter();
                waitingUntilScrollbarMaxChanged = true;
                mustScrollDown = true;
                basicTimer.start(10, this);
                return;
            }
        }

        if(mustAppend())
        {
            if(validChapter(normalisedChapters.last()->normalisedChapter+1))
            {
                currentScrollMax = webView->page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
                currentHeight = webView->page()->mainFrame()->contentsSize().height();
                appendChapter();
                waitingUntilScrollbarMaxChanged = true;
                basicTimer.start(10, this);
                return;
            }
        }

        if(canUnloadFirstChapter())
        {
            currentScrollMax = webView->page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
            currentHeight = webView->page()->mainFrame()->contentsSize().height();
            unloadFirstChapter();
            waitingUntilScrollbarMaxChanged = true;
            mustScrollToPoint = true;
            basicTimer.start(10, this);
            return;
        }

        if(canUnloadLastChapter())
        {
            currentScrollMax = webView->page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
            currentHeight = webView->page()->mainFrame()->contentsSize().height();
            unloadLastChapter();
            waitingUntilScrollbarMaxChanged = true;
            basicTimer.start(10, this);
            return;
        }

    }
}

void MainWindow::append(int normalisedChapter, QString html)
{
    lastAddedChapterInfo = new ChapterInfo(normalisedChapter, 0);
    normalisedChapters.append(lastAddedChapterInfo);

    //    webView->page()->mainFrame()->evaluateJavaScript("var fragment = create('"+ html + "');"
    //                                                 "document.body.appendChild(fragment);");


    QWebElement doc = webView->page()->mainFrame()->documentElement();
    QWebElement body = doc.firstChild().nextSibling();
    body.appendInside(html);
}

void MainWindow::appendChapter()
{
    qDebug() << "chapter appended";
    int chapter = normalisedChapters.last()->normalisedChapter + 1;
    append(chapter, getHtml(chapter));
}

void MainWindow::prepend(int normalisedChapter, QString html)
{
    lastAddedChapterInfo = new ChapterInfo(normalisedChapter, 0);
    normalisedChapters.prepend(lastAddedChapterInfo);

    //    webView->page()->mainFrame()->evaluateJavaScript("var fragment = create('"+ html + "');"
    //                                                 "document.body.insertBefore(fragment,document.body.childNodes[0]);");

    QWebElement doc = webView->page()->mainFrame()->documentElement();
    QWebElement body = doc.firstChild().nextSibling();
    body.prependInside(html);
}

void MainWindow::prependChapter()
{
    qDebug() << "chapter prepended";
    int chapter = normalisedChapters.first()->normalisedChapter - 1;
    prepend(chapter, getHtml(chapter));
}

void MainWindow::display(int normalisedChapter)
{
    for(int i=0; i<normalisedChapters.size(); i++)
        delete normalisedChapters.at(i);
    normalisedChapters.clear();

    currentScrollMax = 0;
    currentHeight = 0;
    append(normalisedChapter, getHtml(normalisedChapter));
    waitingUntilScrollbarMaxChanged = true;
    basicTimer.start(10, this);
}

void MainWindow::scrolled()
{
//    qDebug() << webView->page()->mainFrame()->scrollPosition();
    checkCanScroll();
}

void MainWindow::wordClicked(int id)
{
    QMessageBox msgBox;
    msgBox.setSizeGripEnabled(true);

    msgBox.setText(QString().setNum(id) );
    msgBox.exec();
}

MainWindow::~MainWindow()
{

}

class SleeperThread : public QThread
{
public:
    static void msleep(unsigned long msecs)
    {
        QThread::msleep(msecs);
    }
};

void MainWindow::loadFinished(bool b)
{
    qDebug() << "load finished";
    ClickListener* clickListener = new ClickListener();
    webView->page()->mainFrame()->addToJavaScriptWindowObject("clickListener", clickListener);


    ScrollListener* scrollListener = new ScrollListener();
    connect(scrollListener, SIGNAL(scrolledSignal()), this, SLOT(scrolled()));

    webView->page()->mainFrame()->addToJavaScriptWindowObject("scrollListener", scrollListener);



    //        webView->page()->mainFrame()->evaluateJavaScript("window.onscroll = function () { "
    //                                                         "scrollListener.scrolled(); "
    //                                                         "}");
    webView->page()->mainFrame()->evaluateJavaScript("document.onmousewheel = function(){ scrollListener.scrolled(); }");
    webView->page()->mainFrame()->evaluateJavaScript("document.onkeydown = function(evt){ if(evt.keyCode==38 || evt.keyCode==40) { scrollListener.scrolled();} }");

    display(10);
}
