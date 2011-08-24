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

ChapterDisplayer::ChapterDisplayer(QWebView* webView, QList<int> bibletextIds)
{
    this->webView = webView;
    this->bibletextIds = bibletextIds;

    ignoreScrollEvents = false;

    frameTop = "<html>"
               "<head>"
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

    scrollListener = new ScrollListener();
    javascriptClickListener = new JavascriptClickListener();

    connect(scrollListener, SIGNAL(scrolledSignal()), this, SLOT(scrolled()));

        webView->page()->mainFrame()->addToJavaScriptWindowObject("scrollListener", scrollListener);
        webView->page()->mainFrame()->addToJavaScriptWindowObject("javascriptClickListener", javascriptClickListener);


    connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
    connect(webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this , SLOT(javaScriptWindowObjectClearedSlot()));
}

ChapterDisplayer::~ChapterDisplayer()
{
    delete scrollListener;
    delete javascriptClickListener;
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


    for(int i=0; i<replaceMap.keys().size(); i++)
    {
        QString key = replaceMap.keys().at(i);
        QString value = replaceMap.value(key);

        xml.replace("<"+key+">", "<span class=\""+value+"\">");
        xml.replace("</"+key+">", "</span>");
    }

    xml.replace(QRegExp("<word bibleTextId=\"([0-9]*)\" wordId=\"([0-9]*)\">([^<]*)</word>"), "<span class=\"word\" onclick=\"javascriptClickListener.wordClicked(\\1,\\2)\">\\3</span>");

    xml.replace(QRegExp("<netNote id=\"([0-9]*)\">([^<]*)</netNote>"), "<span class=\"netNote\" onclick=\"javascriptClickListener.netNoteClicked(\\1)\">\\2</span>");

    return xml;
}

void ChapterDisplayer::display(int id, int normalisedChapter)
{
    for(int i=0; i<chapters.values().size(); i++)
    {
        delete chapters.values().at(i);
    }
    chapters.clear();

    webView->setHtml(frameTop + getHtmlFromChapterRepresentations() + frameBottom);

    insertFirstChapter(normalisedChapter, id);

    checkCanScroll();

    //    scrollToCentre();

    //    while(mustPrepend())
    //    {
    //        if(validChapter(getFirstNormChapter()-1))
    //        {
    //            prependChapter();
    //            scrollToCentre();
    //        }
    //        else
    //        {
    //            break;
    //        }
    //    }

    //    while(mustAppend())
    //    {
    //        if(validChapter(getLastNormChapter()+1))
    //        {
    //            appendChapter();
    //            scrollToCentre();
    //        }
    //        else
    //        {
    //            break;
    //        }
    //    }
}

//bool ChapterDisplayer::mustAppend(int min, int max, int value, int pageStep)
//{
//    return value >= (max - pageStep);
//}

//bool ChapterDisplayer::mustAppend()
//{
//    QScrollBar* scrollBar = textBrowser->verticalScrollBar();
//    return mustAppend(scrollBar->minimum(), scrollBar->maximum(), scrollBar->value(), scrollBar->pageStep());
//}


//bool ChapterDisplayer::mustPrepend(int min, int max, int value, int pageStep)
//{
//    return value <= (min + pageStep);
//}

//bool ChapterDisplayer::mustPrepend()
//{
//    QScrollBar* scrollBar = textBrowser->verticalScrollBar();
//    return mustPrepend(scrollBar->minimum(), scrollBar->maximum(), scrollBar->value(), scrollBar->pageStep());
//}

//void ChapterDisplayer::addChapter(ChapterRepresentation* chapterRepresentation)
//{
//    chapters.insert(chapterRepresentation->getNormalisedChapter(), chapterRepresentation);
//}

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


void ChapterDisplayer::prependChapter()
{
//    qDebug() << "prepending";
    add(constructChapterRepresentation(getFirstNormChapter()-1));
}

void ChapterDisplayer::appendChapter()
{
//    qDebug() << "appending";
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
//    qDebug() << "unloading first";
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
//    qDebug() << "unloading last";

    ChapterRepresentation* chRep = chapters.values().at(chapters.values().size()-1);
    chapters.remove(chRep->getNormalisedChapter());
    delete chRep;

    int orig = webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);
    webView->setHtml(frameTop + getHtmlFromChapterRepresentations() + frameBottom);
    scrollTo(orig);
}

bool ChapterDisplayer::canUnloadFirstChapter()
{
    if(webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height() > 0)
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
    if(webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height() > 0)
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

//        printOutHeights("start: ");

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

//        printOutHeights("end: ");
        ignoreScrollEvents = false;
    }
}

//void ChapterDisplayer::scrollDown(int pixels)
//{
//    QScrollBar* scrollBar = textBrowser->verticalScrollBar();
//    if(scrollBar->isVisible())
//    {
//        scrollBar->setValue(scrollBar->value() + pixels);
//    }
//}

//void ChapterDisplayer::scrollUp(int pixels)
//{
//    scrollDown(-pixels);
//}


//bool ChapterDisplayer::canUnloadLastChapter()
//{
//    QScrollBar* scrollBar = textBrowser->verticalScrollBar();
//    if(scrollBar->isVisible())
//    {
//        int chContainingCentralPos = getCurrentChapter();

//        ChapterRepresentation* chRep = chapters.values().at(chapters.values().size()-1);


//        if( chRep->getNormalisedChapter() > chContainingCentralPos + 1 )
//        {
//            //fake remove it and see if 'mustAppend', if so then don't remove it

//            int firstGlobalPos = convertPosToGlobal(chRep->getNormalisedChapter(), chRep->firstPosInFragment());

//            QTextCursor textCursor(textBrowser->document());
//            textCursor.setPosition(firstGlobalPos);


//            QRect rect = textBrowser->cursorRect(textCursor);//viewport co-ords

//            QScrollBar* scrollBar = textBrowser->verticalScrollBar();

//            return !mustAppend(scrollBar->minimum(), scrollBar->value()+ rect.top()-scrollBar->pageStep(), scrollBar->value(), scrollBar->pageStep());
//        }
//    }
//    return false;
//}


//bool ChapterDisplayer::canUnloadFirstChapter()
//{
//    QScrollBar* scrollBar = textBrowser->verticalScrollBar();
//    if(scrollBar->isVisible())
//    {
//        int chContainingCentralPos = getCurrentChapter();

//        ChapterRepresentation* chRep = chapters.values().at(0);

//        if(chRep->getNormalisedChapter() < chContainingCentralPos-1)
//        {
//            //fake remove it and see if 'mustPrepend', if so then don't remove it

//            int endGlobalPos = convertPosToGlobal(chRep->getNormalisedChapter(), chRep->lastPosInFragment()+2);

//            QTextCursor textCursor(textBrowser->document());
//            textCursor.setPosition(endGlobalPos);

//            QRect rect = textBrowser->cursorRect(textCursor); //viewport co-ords

//            QScrollBar* scrollBar = textBrowser->verticalScrollBar();

//            return !mustPrepend(scrollBar->value() + rect.bottom(), scrollBar->maximum(), scrollBar->value(), scrollBar->pageStep());
//        }
//    }
//    return false;
//}

//void ChapterDisplayer::unloadLastChapter()
//{

//    ChapterRepresentation* chRep = chapters.values().at(chapters.values().size()-1);

//    QTextCursor textCursor(textBrowser->document());

//    textCursor.beginEditBlock();

//    textCursor.movePosition(QTextCursor::End);

//    //    int startPos = convertPosToGlobal(chRep.getNormalisedChapter(), chRep.firstPosInFragment());
//    int startPos = convertPosToGlobal(chRep->getNormalisedChapter(), 3);

//    while(textCursor.position() >  startPos)
//    {
//        textCursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::KeepAnchor);
//    }

//    textCursor.deletePreviousChar();

//    textCursor.endEditBlock();

//    chapters.remove(chRep->getNormalisedChapter());
//    delete chRep;

//    calculateAndSendChapterStarts();
//}

//void ChapterDisplayer::unloadFirstChapter()
//{
//    QScrollBar* scrollBar = textBrowser->verticalScrollBar();

//    int originalHeight = textBrowser->document()->size().height();
//    int originalValue = scrollBar->value();

//    ChapterRepresentation* chRep = chapters.values().at(0);

//    int endPos = chRep->lastPosInFragment()+2;

//    QTextCursor textCursor(textBrowser->document());

//    textCursor.beginEditBlock();

//    textCursor.setPosition(0);

//    while(textCursor.position() < endPos )
//    {
//        textCursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
//    }
//    textCursor.deletePreviousChar();

//    textCursor.endEditBlock();

//    int newHeight = textBrowser->document()->size().height();

//    textBrowser->verticalScrollBar()->setValue(originalValue - (originalHeight-newHeight));

//    chapters.remove(chRep->getNormalisedChapter());
//    delete chRep;

//    calculateAndSendChapterStarts();
//}


void ChapterDisplayer::highlight()
{
}

void ChapterDisplayer::insertFirstChapter(int normalisedChapter, int idLocation)
{
    add(constructChapterRepresentation(normalisedChapter, idLocation));
}

//ChapterRepresentation* ChapterDisplayer::appendChapter()
//{
//    ChapterRepresentation* chapter =  constructChapterRepresentation(getLastNormChapter()+1);
//    addChapter(chapter, true);
//    return chapter;
//}

//ChapterRepresentation* ChapterDisplayer::prependChapter()
//{
//    ChapterRepresentation* chapter =  constructChapterRepresentation(getFirstNormChapter()-1);
//    addChapter(chapter, false);
//    return chapter;
//}

void ChapterDisplayer::scrolled()
{
    checkCanScroll();
}

void ChapterDisplayer::javaScriptWindowObjectClearedSlot()
{
    webView->page()->mainFrame()->addToJavaScriptWindowObject("scrollListener", scrollListener);
    webView->page()->mainFrame()->evaluateJavaScript("document.onmousewheel = function(){ scrollListener.scrolled(); }");
    webView->page()->mainFrame()->evaluateJavaScript("document.onkeydown = function(evt){ if(evt.keyCode==38 || evt.keyCode==40) { scrollListener.scrolled();} }");

    webView->page()->mainFrame()->addToJavaScriptWindowObject("javascriptClickListener", javascriptClickListener);
}

void ChapterDisplayer::loadFinished(bool b)
{
}


