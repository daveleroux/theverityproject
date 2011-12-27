#include "chapterdisplayer.h"
#include "biblequerier.h"
#include "textinfo.h"
#include <QScrollBar>
#include <QTextBlock>
#include <QDebug>
#include <QBrush>
#include <QThread>
#include "scrolllistener.h"
#include "globalvariables.h"
#include <QFile>
#include "versereferenceparser.h"

ChapterDisplayer::ChapterDisplayer(VWebView* webView, QList<int> bibletextIds, int id, int normalisedChapter) : VWebHistoryItem(webView)
{

    this->webView = webView;
    this->bibletextIds = bibletextIds;
    this->id = id;
    this->normalisedChapter = normalisedChapter;

    ignoreScrollEvents = false;

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

ChapterDisplayer::~ChapterDisplayer()
{
    for(int i=0; i<chapters.values().size(); i++)
    {
        delete chapters.values().at(i);
    }
}

QString ChapterDisplayer::transformToHtml(QString xml)
{
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
    replaceMap.insert("rtl", "rtl");
    replaceMap.insert("greek", "greek");
    replaceMap.insert("chunk", "chunk");


    for(int i=0; i<replaceMap.keys().size(); i++)
    {
        QString key = replaceMap.keys().at(i);
        QString value = replaceMap.value(key);

        xml.replace("<"+key+">", "<span class=\""+value+"\">");
        xml.replace("</"+key+">", "</span>");
    }

    xml.replace("<selectedId>", "<a id=\"selectedId\">");
    xml.replace("</selectedId>", "</a>");

    xml.replace("<chunkDetails", "<span class=\"chunkDetails\"");
    xml.replace("</chunkDetails>", "</span>");

    xml.replace(QRegExp("<word bibleTextId=\"([0-9]*)\" wordId=\"([0-9]*)\">([^<]*)</word>"), "<span class=\"word\" onclick=\"javascriptClickListener.wordClicked(\\1,\\2)\">\\3</span>");

    xml.replace(QRegExp("<netNote id=\"([0-9]*)\">([^<]*)</netNote>"), "<span class=\"netNote\" onclick=\"javascriptClickListener.netNoteClicked(\\1)\">\\2</span>");

    return xml;
}

void ChapterDisplayer::display()
{
    webView->setHtml(frameTop + getHtmlFromChapterRepresentations() + frameBottom); //just clears the view so the heights are right

    insertFirstChapter(normalisedChapter, id);

    ignoreScrollEvents = true;

    scrollToCentre();

    while(mustPrepend())
    {
        if(validChapter(getFirstNormChapter()-1))
        {
            prependChapter();
            scrollToCentre();
        }
        else
        {
            break;
        }
    }

    while(mustAppend())
    {
        if(validChapter(getLastNormChapter()+1))
        {
            appendChapter();
            scrollToCentre();
        }
        else
        {
            break;
        }
    }

    ignoreScrollEvents = false;
}


bool ChapterDisplayer::validChapter(int proposedChapter)
{
    TextSpecificData* textSpecificData = BibleQuerier::getTextSpecificData(bibletextIds.at(0));
    return proposedChapter <= textSpecificData->maxChapter && proposedChapter >= textSpecificData->minChapter;
}

int ChapterDisplayer::getFirstNormChapter()
{
    QMap<int, ChapterRepresentation*>::iterator it = chapters.begin();
    return it.key();
}

int ChapterDisplayer::getLastNormChapter()
{
    QMap<int, ChapterRepresentation*>::iterator it = chapters.end();
    it--;
    return it.key();
}


void ChapterDisplayer::scrollToCentre()
{
    webView->page()->mainFrame()->scrollToAnchor("selectedId");
    if(webView->page()->mainFrame()->scrollBarValue(Qt::Vertical) < webView->page()->mainFrame()->scrollBarMaximum(Qt::Vertical))
    {
        webView->page()->mainFrame()->scroll(0, -(webView->page()->viewportSize().height()/4));
    }
}

QString ChapterDisplayer::getHtmlFromChapterRepresentations()
{
    QString result;

    QMap<int, ChapterRepresentation*>::Iterator it;
    for(it = chapters.begin(); it != chapters.end(); it++)
    {
        result += it.value()->getHtml();
    }

    return result;
}

void ChapterDisplayer::add(ChapterRepresentation* chapterRepresentation)
{
    chapters.insert(chapterRepresentation->getNormalisedChapter(), chapterRepresentation);

    int originalHeight = getDocumentHeight();
    webView->setHtml(frameTop + getHtmlFromChapterRepresentations() + frameBottom);
    int newHeight = getDocumentHeight();

    chapterRepresentation->setHeight(newHeight - originalHeight);
}

void ChapterDisplayer::saveState()
{
    savedScrollValue = webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);
}

void ChapterDisplayer::restore()
{
    webView->setHtml(frameTop + getHtmlFromChapterRepresentations() + frameBottom);
    scrollTo(savedScrollValue);
}

void ChapterDisplayer::prependChapter()
{
    add(constructChapterRepresentation(getFirstNormChapter()-1));
}

void ChapterDisplayer::appendChapter()
{
    add(constructChapterRepresentation(getLastNormChapter()+1));
}


bool ChapterDisplayer::mustPrepend()
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

bool ChapterDisplayer::mustAppend()
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

int ChapterDisplayer::getDocumentHeight()
{
    QWebElement documentElement = webView->page()->mainFrame()->documentElement().firstChild().nextSibling().firstChild();
    return documentElement.geometry().height();
}

void ChapterDisplayer::scrollTo(int value)
{
    webView->page()->mainFrame()->setScrollBarValue(Qt::Vertical, value);
}

void ChapterDisplayer::unloadFirstChapter()
{
    ChapterRepresentation* chRep = chapters.values().at(0);
    int unloadingHeight = chRep->getHeight();

    chapters.remove(chRep->getNormalisedChapter());
    delete chRep;

    int orig = webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);

    webView->setHtml(frameTop + getHtmlFromChapterRepresentations() + frameBottom);

    scrollTo(orig-unloadingHeight);
}

void ChapterDisplayer::unloadLastChapter()
{
    ChapterRepresentation* chRep = chapters.values().at(chapters.values().size()-1);
    chapters.remove(chRep->getNormalisedChapter());
    delete chRep;

    int orig = webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);
    webView->setHtml(frameTop + getHtmlFromChapterRepresentations() + frameBottom);
    scrollTo(orig);
}

bool ChapterDisplayer::canUnloadFirstChapter()
{
    if(webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height() > 0 && chapters.size() > 1)
    {
        //fake remove it and see if 'mustPrepend', if so then don't remove it

        int totalDocumentHeight = getDocumentHeight();
        int firstChapterHeight = chapters.values().at(0)->getHeight();


        int scrollbarValueNow =  webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);
        int scrollbarHeightNow =  webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height();
        int scrollbarMaxNow =  webView->page()->mainFrame()->scrollBarMaximum(Qt::Vertical);

        float ratio = totalDocumentHeight/((float)(scrollbarMaxNow + scrollbarHeightNow));

        return !(ratio*scrollbarValueNow - firstChapterHeight < ratio*scrollbarHeightNow);
    }
    return false;
}

bool ChapterDisplayer::canUnloadLastChapter()
{
    if(webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height() > 0 && chapters.size() > 1)
    {
        //fake remove it and see if 'mustAppend', if so then don't remove it

        int totalDocumentHeight = getDocumentHeight();
        int lastChapterHeight = chapters.values().at(chapters.values().size()-1)->getHeight();

        int proposedDocumentHeight = totalDocumentHeight-lastChapterHeight;

        int scrollbarValueNow =  webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);
        int scrollbarHeightNow =  webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height();
        int scrollbarMaxNow =  webView->page()->mainFrame()->scrollBarMaximum(Qt::Vertical);

        int proposedMax = ((scrollbarMaxNow+scrollbarHeightNow)*proposedDocumentHeight)/((float)totalDocumentHeight) - scrollbarHeightNow;

        return !(proposedMax - scrollbarValueNow < scrollbarHeightNow);
    }
    return false;
}

void ChapterDisplayer::printOutHeights(QString toPrint)
{
    for(int i=0; i<chapters.keys().size(); i++)
    {
        int chapter = chapters.keys().at(i);
        ChapterRepresentation* c = chapters.value(chapter);
        toPrint += QString().setNum(chapter) + ": " + QString().setNum(c->getHeight()) + ", ";
    }
    qDebug() << toPrint;
}

void ChapterDisplayer::checkCanScroll()
{
    if(!ignoreScrollEvents)
    {
        ignoreScrollEvents = true;

        while(mustPrepend())
        {
            if(validChapter(getFirstNormChapter()-1))
            {
                int originalHeight = getDocumentHeight();
                int originalScrollValue = webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);
                prependChapter();
                int newHeight = getDocumentHeight();
                scrollTo(originalScrollValue + newHeight - originalHeight);
            }
            else
            {
                break;
            }
        }

        while(mustAppend())
        {
            if(validChapter(getLastNormChapter()+1))
            {
                int originalScrollValue = webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);
                appendChapter();
                scrollTo(originalScrollValue);
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

        ignoreScrollEvents = false;
    }
}


void ChapterDisplayer::insertFirstChapter(int normalisedChapter, int idLocation)
{
    add(constructChapterRepresentation(normalisedChapter, idLocation));
}


void ChapterDisplayer::scrolled()
{
    checkCanScroll();
}


void ChapterDisplayer::moved()
{
    QWebElementCollection collection = webView->page()->mainFrame()->findAllElements("span[class=\"chunkDetails\"][bibletextId=\"" + QString().setNum(bibletextIds.at(0)) +"\"]");

    int scroll = webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);

    QList<QWebElement> visibleElements;

    foreach(QWebElement chunkDetailsElement, collection)
    {
        int top = chunkDetailsElement.geometry().top();
        int height = chunkDetailsElement.geometry().height();

        if(top+8 >= scroll && top+15 <= scroll+webView->height()) //numbers are a bit magicy, should maybe use the height of the verse numbers instead
        {
            if(chunkDetailsElement.attribute("verse").toInt() > 0)
                visibleElements.append(chunkDetailsElement);
        }
    }

    int minId;
    int maxId;
    QString from = "";
    QString to = "";

    int minBook;
    int minChapter;
    int minVerse;

    int maxBook;
    int maxChapter;
    int maxVerse;

    if(visibleElements.size() > 0)
    {
        minId = visibleElements.at(0).attribute("id").toInt();

        minBook = visibleElements.at(0).attribute("bookNumber").toInt();
        minChapter = visibleElements.at(0).attribute("chapter").toInt();
        minVerse = visibleElements.at(0).attribute("verse").toInt();

        maxId = minId;

        maxBook = minBook;
        maxChapter = minChapter;
        maxVerse = minVerse;

        foreach(QWebElement element, visibleElements)
        {
            int id = element.attribute("id").toInt();
            if(id < minId)
            {
                minId = id;
                minBook = element.attribute("bookNumber").toInt();
                minChapter = element.attribute("chapter").toInt();
                minVerse = element.attribute("verse").toInt();
            }

            if(id > maxId)
            {
                maxId = id;
                maxBook = element.attribute("bookNumber").toInt();
                maxChapter = element.attribute("chapter").toInt();
                maxVerse = element.attribute("verse").toInt();
            }
        }

        from = VerseReferenceParser::calculateStringRepresentation(minBook, minChapter, minVerse);
        to = VerseReferenceParser::calculateStringRepresentation(maxBook, maxChapter, maxVerse);
    }

    QString title = PROGRAM_NAME;
    if(from.length() > 0)
    {
        title.append(" - ");
        title.append(from);
        if(to.length() > 0)
        {
            title.append(" - ");
            title.append(to);
        }
    }
    webView->window()->setWindowTitle(title);
}


