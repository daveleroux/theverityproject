#include "mainwindow.h"
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QtSql>
#include <QXmlQuery>
#include <QWebElement>
#include "timer.h"
#include <iostream>
using namespace std;

QString MainWindow::replaceAll(QString wholeChapter, QMap<QString, QString> replaceMap)
{
    for(int i=0; i<replaceMap.keys().size(); i++)
    {
        QString key = replaceMap.keys().at(i);
        QString value = replaceMap.value(key);

        wholeChapter.replace("<"+key+">", "<span class=\""+value+"\">");
        wholeChapter.replace("</"+key+">", "</span>");
    }
    return wholeChapter;
}

QString MainWindow::getParallelHtml(int normalisedChapter)
{
    static QString fake = 0;
    if(fake == 0)
    {
        QFile tmp("test.html");
        if(!tmp.open(QIODevice::ReadOnly))
            exit(1);

        QByteArray byteArray = tmp.readAll();
        fake = QString::fromUtf8(byteArray.data());

    }
    return fake;


    timer t;
    t.start();

    QSqlQuery query;

    if(!query.exec("select b1.text, b2.text from bibles as b1, bibles as b2 where b1.normalised_chapter=" + QString().setNum(normalisedChapter) +
                   " and b1.bibletext_id=1 and b1.parallel=b2.parallel and b2.bibletext_id=2"))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }


    QString wholeChapter = "<normalisedChapter>";
    wholeChapter += "<table>";
    QString colour = "1";
    while(query.next())
    {
        wholeChapter += "<tr>";

        wholeChapter += "<td>";
        wholeChapter += query.value(0).toString();
        wholeChapter += "</td>";

        wholeChapter += "<td>";
        wholeChapter += query.value(1).toString();
        wholeChapter += "</td>";

        wholeChapter += "</tr>";

        if(colour == "1")
            colour = "2";
        else
            colour = "1";
    }
    wholeChapter += "</table>";
    wholeChapter += "</normalisedChapter>";

    cout << "query " << t << endl;
    t.restart();

    QMap<QString, QString> replaceMap;

    replaceMap.insert("preferredVerse", "preferredVerse");
    replaceMap.insert("alternateVerse", "alternateVerse");
    replaceMap.insert("note", "note");
    replaceMap.insert("smallCaps", "smallCaps");
    replaceMap.insert("b", "b");
    replaceMap.insert("i", "i");
    replaceMap.insert("hebrew", "hebrew");
    replaceMap.insert("paragraphTitle", "paragraphTitle");
    replaceMap.insert("hebrewParagraph", "paragraphTitle");
    replaceMap.insert("section", "section");
    replaceMap.insert("psalmSuperscription", "psalmSuperscription");
    replaceMap.insert("speakerHeading", "speakerHeading");
    replaceMap.insert("bodyText", "bodyText");
    replaceMap.insert("poetry", "poetry");
    replaceMap.insert("bodyBlock", "bodyBlock");
    replaceMap.insert("blockQuote", "blockQuote");
    replaceMap.insert("chapter", "chapter");
    replaceMap.insert("bookName", "bookName");
    replaceMap.insert("verse", "verse");
    replaceMap.insert("normalisedChapter", "normalisedChapter");
    replaceMap.insert("normalisedChapter", "normalisedChapter");

    wholeChapter = replaceAll(wholeChapter, replaceMap );


    //    QXmlQuery xmlQuery(QXmlQuery::XSLT20);

    //    QByteArray byteArray(wholeChapter.toUtf8());

    //    QBuffer buffer(&byteArray);
    //    buffer.open(QBuffer::ReadWrite);

    //    xmlQuery.setFocus(&buffer);

    //    xmlQuery.setQuery(QUrl("qrc:/xsl/bible.xsl"));

    //    QString result;
    //    xmlQuery.evaluateTo(&result);

    cout << "xsl " << t << endl;
    //    result = result.simplified();


    return wholeChapter;


}

QString MainWindow::getHtml(int normalisedChapter)
{
    return getParallelHtml(normalisedChapter);

    QSqlQuery query;

    if(!query.exec("select text from bibles where bibletext_id=2 and normalised_chapter=" + QString().setNum(normalisedChapter)))
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

    connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
    connect(webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(javaScriptWindowObjectCleared()));

    frameTop = "<html>"
               "<head>"
               "<style type=\"text/css\">\n";

    QFile tmp(":/style/bible.css");
    if(!tmp.open(QIODevice::ReadOnly))
        exit(1);

    QByteArray byteArray = tmp.readAll();

    frameTop += QString::fromUtf8(byteArray.data());

    frameTop += "</style>"
                "</head>"
                "<body>"
                "<div>";

    frameBottom =  "</div>"
                   "</body>"
                   "</html>";

    clickListener = new ClickListener();
    scrollListener = new ScrollListener();

    int firstChapterToView = 1005;
    append(firstChapterToView, getParallelHtml(firstChapterToView));
    checkCanScroll();
    QWebElement w = webView->page()->mainFrame()->documentElement().findFirst("span[id=\"1\"]");
    qDebug() << w.geometry().y();

//    float perc = w.geometry().y()/(float)webView->page()->mainFrame()->geometry().height();
    float perc = w.geometry().y()/(float)webView->page()->mainFrame()->documentElement().firstChild().nextSibling().firstChild().geometry().height();
    qDebug() << perc; //+ webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height()
    float result = perc * (webView->page()->mainFrame()->scrollBarMaximum(Qt::Vertical) ) ;
    qDebug() << "setting to " << result;
    webView->page()->mainFrame()->setScrollBarValue(Qt::Vertical,result );
    qDebug() << w.geometry().y();
    qDebug() << "scroll value " << webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);
    qDebug() << w.tagName();

    webView->findText("serpent");
}

bool MainWindow::mustAppend()
{
    if(webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height() > 0)
    {
        int value =  webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);
        int height =  webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height();
        int max =  webView->page()->mainFrame()->scrollBarMaximum(Qt::Vertical);

        return max - value < height;
    }
    else
        return true;
}

bool MainWindow::mustPrepend()
{
    if(webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height() > 0)
    {
        int value =  webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);
        int height =  webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height();

        return value < height;
    }
    else
        return true;
}

bool MainWindow::validChapter(int proposedChapter)
{
    return proposedChapter <= 1189 && proposedChapter >= 1;
}


bool MainWindow::canUnloadFirstChapter()
{
    if(webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height() > 0)
    {
        //fake remove it and see if 'mustPrepend', if so then don't remove it

        int totalDocumentHeight = getDocumentHeight();
        int firstChapterHeight = normalisedChapters.first()->height;


        int scrollbarValueNow =  webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);
        int scrollbarHeightNow =  webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height();
        int scrollbarMaxNow =  webView->page()->mainFrame()->scrollBarMaximum(Qt::Vertical);

        float ratio = totalDocumentHeight/((float)(scrollbarMaxNow + scrollbarHeightNow));

        return !(ratio*scrollbarValueNow - firstChapterHeight < ratio*scrollbarHeightNow);
    }
    return false;
}

bool MainWindow::canUnloadLastChapter()
{
    if(webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height() > 0)
    {
        //fake remove it and see if 'mustAppend', if so then don't remove it

        int totalDocumentHeight = getDocumentHeight();
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
    qDebug() << "unloading first";
    int unloadingHeight = normalisedChapters.first()->height;
    delete normalisedChapters.first();
    normalisedChapters.removeFirst();

    int orig = webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);

    QString html2 = frameTop + getHtmlFromChapterInfos() + frameBottom;
    //    qDebug() << html2;
    webView->setHtml(html2);
    webView->page()->mainFrame()->setScrollBarValue(Qt::Vertical, orig-unloadingHeight);
}

void MainWindow::unloadLastChapter()
{
    qDebug() << "unloading last";
    delete normalisedChapters.last();
    normalisedChapters.removeLast();

    int orig = webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);
    QString html2 = frameTop + getHtmlFromChapterInfos() + frameBottom;
    //    qDebug() << html2;
    webView->setHtml(html2);
    webView->page()->mainFrame()->setScrollBarValue(Qt::Vertical, orig);

}

int MainWindow::getDocumentHeight()
{
    //    QVariant result =  webView->page()->mainFrame()->evaluateJavaScript("document.getElementById(983).scrollHeight");
    //    return result.toInt();

//        QVariant result =  webView->page()->mainFrame()->evaluateJavaScript("document.body.firstChild.scrollHeight");
//        return result.toInt();

    QWebElement element = webView->page()->mainFrame()->documentElement().firstChild().nextSibling().firstChild();
    //    QString widthProperty = documentElement.styleProperty("scrollHeight",QWebElement::ComputedStyle); //forces stuff to finish see https://bugreports.qt.nokia.com//browse/QTBUG-18034
    //    qDebug() << "width property" << widthProperty;
    //    QString heightProperty = documentElement.styleProperty("scrollHeight",QWebElement::ComputedStyle);
    //    qDebug() << heightProperty;
    return element.geometry().height();
    //   return webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height() +
    //           webView->page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
}

void MainWindow::checkCanScroll()
{        
    while(mustPrepend())
    {
        if(validChapter(normalisedChapters.first()->normalisedChapter-1))
        {
            int originalHeight = getDocumentHeight();
            int originalScrollValue = webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);
            prependChapter();
            int newHeight = getDocumentHeight();
            webView->page()->mainFrame()->setScrollBarValue(Qt::Vertical, originalScrollValue + newHeight - originalHeight);
        }
        else
        {
            break;
        }
    }

    while(mustAppend())
    {
        if(validChapter(normalisedChapters.last()->normalisedChapter+1))
        {
            int originalScrollValue = webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);
            appendChapter();
            webView->page()->mainFrame()->setScrollBarValue(Qt::Vertical, originalScrollValue);
        }
        else
        {
            break;
        }
    }

    while(canUnloadFirstChapter())
        unloadFirstChapter();

    while(canUnloadLastChapter())
        unloadLastChapter();

}



void MainWindow::append(int normalisedChapter, QString html)
{
    ChapterInfo* chapterInfo = new ChapterInfo(normalisedChapter, 0, html);
    normalisedChapters.append(chapterInfo);

    int originalHeight = getDocumentHeight();

    webView->setHtml(frameTop + getHtmlFromChapterInfos() + frameBottom);

    int newHeight = getDocumentHeight();

    chapterInfo->height = newHeight - originalHeight;
}

void MainWindow::appendChapter()
{
    qDebug() << "appending";
    int chapter = normalisedChapters.last()->normalisedChapter + 1;
    append(chapter, getHtml(chapter));
}

QString MainWindow::getHtmlFromChapterInfos()
{
    QString result;
    for(int i=0; i< normalisedChapters.size(); i++)
        result += normalisedChapters.at(i)->html;

    return result;
}

void MainWindow::prepend(int normalisedChapter, QString html)
{
    ChapterInfo* chapterInfo = new ChapterInfo(normalisedChapter, 0, html);
    normalisedChapters.prepend(chapterInfo);

    int originalHeight = getDocumentHeight();
    webView->setHtml(frameTop + getHtmlFromChapterInfos() + frameBottom);
    int newHeight = getDocumentHeight();

    chapterInfo->height = newHeight - originalHeight;

    qDebug() << "height " << newHeight - originalHeight;
}

void MainWindow::prependChapter()
{
    qDebug() << "prepending";
    int chapter = normalisedChapters.first()->normalisedChapter - 1;
    prepend(chapter, getHtml(chapter));
}


void MainWindow::scrolled()
{
    checkCanScroll();
}

void MainWindow::wordClicked(int id)
{
    QMessageBox msgBox;
    msgBox.setSizeGripEnabled(true);

    msgBox.setText(QString().setNum(id));
    msgBox.exec();
}

void MainWindow::javaScriptWindowObjectCleared()
{
}

void MainWindow::loadFinished(bool b)
{

    webView->page()->mainFrame()->addToJavaScriptWindowObject("clickListener", clickListener);

    connect(scrollListener, SIGNAL(scrolledSignal()), this, SLOT(scrolled()));

    webView->page()->mainFrame()->addToJavaScriptWindowObject("scrollListener", scrollListener);


    //        webView->page()->mainFrame()->evaluateJavaScript("window.onscroll = function () { "
    //                                                         "scrollListener.scrolled(); "
    //                                                         "}");
    webView->page()->mainFrame()->evaluateJavaScript("document.onmousewheel = function(){ scrollListener.scrolled(); }");
    webView->page()->mainFrame()->evaluateJavaScript("document.onkeydown = function(evt){ if(evt.keyCode==38 || evt.keyCode==40) { scrollListener.scrolled();} }");

}
