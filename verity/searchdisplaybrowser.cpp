#include "searchdisplaybrowser.h"
#include <QDebug>
#include "eventmanager.h"
#include "searchevent.h"
#include "versereferenceparser.h"
#include "netanalyzer.h"
#include "globalvariables.h"
#include <QWebFrame>

#include "CLucene.h"
#include "CLucene/analysis/Analyzers.h"

#include <QFile>


using namespace lucene::analysis;
using namespace lucene::queryParser;
using namespace lucene::search;

wchar_t* convert(QString input)
{
    using namespace lucene::queryParser;
    wchar_t* arr = new wchar_t[input.length()+1];
    int size = input.toWCharArray(arr);
    arr[size] = '\0';
    return arr;
}

SearchDisplayBrowser::SearchDisplayBrowser(QWidget* parent) : VWebView(parent)
{
    EventManager::addListener(EventType::SEARCH, this);

    javascriptClickListener = new JavascriptClickListener();
    page()->mainFrame()->addToJavaScriptWindowObject("javascriptClickListener", javascriptClickListener);
    connect(page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this , SLOT(javaScriptWindowObjectClearedSlot()));

    //should share this with bibletextbrowser?

    frameTop = "<html>"
               "<head>"
               "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
               "<style type=\"text/css\">\n";

    QFile tmp(DATA_PATH + "/bible.css");
    if(!tmp.open(QIODevice::ReadOnly))
        exit(1);

    QByteArray byteArray = tmp.readAll();

    frameTop += QString::fromUtf8(byteArray.data());

    tmp.close();

    frameTop += "</style>"
                "</head>"
                "<body>"
                "<div>";

    frameBottom =  "</div>"
                   "</body>"
                   "</html>";

}
void SearchDisplayBrowser::javaScriptWindowObjectClearedSlot()
{
    page()->mainFrame()->addToJavaScriptWindowObject("javascriptClickListener", javascriptClickListener);
}

SearchDisplayBrowser::~SearchDisplayBrowser()
{
    delete javascriptClickListener;
}

void SearchDisplayBrowser::handleEvent(Event* event)
{
    if(event->getEventType() == EventType::SEARCH)
    {
        SearchEvent* searchEvent = static_cast<SearchEvent*>(event);
        display(searchEvent->searchTerm);
        parentWidget()->raise();
    }
    else
    {
        VWebView::handleEvent(event);
    }
}

QSize SearchDisplayBrowser::sizeHint() const
{
    return QSize(width(), 200);
}

void SearchDisplayBrowser::display(QString searchTerm)
{
    QString table = "<table class=\"searchResults\">";

    QList<SearchResult> listOfSearchResults = doActualSearch(searchTerm);
    foreach (SearchResult hit, listOfSearchResults)// (int i = 0; i < listOfSearchResults.count(); i++)
    {
        table += "<tr onclick=\"javascriptClickListener.bibleReferenceClicked(" + QString().setNum(hit.verseReference.book)
                 + "," + QString().setNum(hit.verseReference.chapter)
                 + "," + QString().setNum(hit.verseReference.verse)
                 + ")\"><td><b>" + hit.verseReference.stringRepresentation.replace(" ", "&nbsp;") + "</b></td><td>" + hit.verseContents + "</td></tr></a>";
    }
    table += "</table>";

    setHtml(frameTop + table + frameBottom);
}

QList<SearchResult> SearchDisplayBrowser::doActualSearch(QString searchTerm)
{
    QList<SearchResult> result;

    searchTerm = searchTerm.toLower();

    NetAnalyzer* netAnalyzer = new NetAnalyzer();

    Document doc;
    try
    {
        wchar_t *convertedTerm = convert(QString(searchTerm));
        Query* query = QueryParser::parse(convertedTerm, _T("contentsToSearch"),netAnalyzer);
        IndexSearcher searcher(INDEX_NAME.toLatin1().data());
        Hits* hits = searcher.search(query);

        int max = hits->length() > 100? 100 : hits->length();

        qDebug() << max;

        for(int i=0; i<max; i++)
        {
            doc = hits->doc(i);
            result.append(SearchResult(VerseReference(QString::fromWCharArray(doc.get(_T("book"))).toInt(),
                                        QString::fromWCharArray(doc.get(_T("chapter"))).toInt(),
                                        QString::fromWCharArray(doc.get(_T("verse"))).toInt(),
                                        VerseReferenceParser::calculateStringRepresentation(QString::fromWCharArray(doc.get(_T("book"))).toInt(),
                                                                                            QString::fromWCharArray(doc.get(_T("chapter"))).toInt(),
                                                                                            QString::fromWCharArray(doc.get(_T("verse"))).toInt())),
                    QString::fromWCharArray(doc.get(_T("contentsToDisplay")))));
        }
        delete[] convertedTerm;
    }
    catch (CLuceneError e)
    {
        qDebug() << e.what();
    }
    delete netAnalyzer;
    return result;
}
