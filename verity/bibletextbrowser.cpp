#include <QDebug>
#include <QtSql>

#include <QTextCursor>
#include <QApplication>
#include <QMessageBox>

#include "bibletextbrowser.h"
#include "timer.h"
#include "globalvariables.h"
#include "biblequerier.h"
#include "paralleltextchapterdisplayer.h"
#include "singletextchapterdisplayer.h"
#include "biblereferenceevent.h"
#include "eventmanager.h"


#include <iostream>
using namespace std;

BibleTextBrowser::BibleTextBrowser(QWidget* parent) : VWebView(parent)
{
    webHistory = new VWebHistory();
    connect(webHistory, SIGNAL(backwardAvailable(bool)), this, SIGNAL(backwardAvailable(bool)));
    connect(webHistory, SIGNAL(forwardAvailable(bool)), this, SIGNAL(forwardAvailable(bool)));


    EventManager::addListener(EventType::BIBLE_REFERENCE, this);

    QSettings settings(PROGRAM_NAME, PROGRAM_NAME);
    settings.beginGroup(BIBLE_TEXT_BROWSER_SETTING_GROUP);

    //    int size = settings.beginReadArray(FONT_FAMILY_SETTINGS);
    //    for (int i = 0; i < size; i++)
    //    {
    //        settings.setArrayIndex(i);
    //        QString text = settings.value(TEXT_SETTING).toString();
    //        QString fontFamily = settings.value(FONT_SETTING).toString();
    //        fontFamilies.insert(text, fontFamily);
    //    }
    //    settings.endArray();

    //    if (size == 0)
    //    {
    //        fontFamilies.insert("esv", "DejaVuSans");
    //        fontFamilies.insert("kjv", "DejaVuSans");
    //        fontFamilies.insert("tisch", "DejaVuSans");
    //        fontFamilies.insert("wlc", "SBL Hebrew");
    //    }

    //    if (!settings.contains("zoomIn"))
    //    {
    //        settings.setValue("zoomIn", 1);
    //    }

    //    int zoomInSetting = settings.value("zoomIn", 1).toInt();

    //    settings.endGroup();



    page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);


    scrollListener = new ScrollListener();
    javascriptClickListener = new JavascriptClickListener();

    connect(scrollListener, SIGNAL(scrolledSignal()), this, SLOT(scrolled()));
    connect(scrollListener, SIGNAL(movedSignal()), this, SLOT(moved()));

    page()->mainFrame()->addToJavaScriptWindowObject("scrollListener", scrollListener);
    page()->mainFrame()->addToJavaScriptWindowObject("javascriptClickListener", javascriptClickListener);


    connect(page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this , SLOT(javaScriptWindowObjectClearedSlot()));

}

BibleTextBrowser::~BibleTextBrowser()
{
    delete webHistory;
    delete scrollListener;
    delete javascriptClickListener;
}

//void BibleTextBrowser::writeOutSettings()
//{
//    QSettings settings(PROGRAM_NAME, PROGRAM_NAME);

//    settings.beginGroup(BIBLE_TEXT_BROWSER_SETTING_GROUP);

//    //    settings.beginWriteArray(FONT_FAMILY_SETTINGS);
//    //    for (int i = 0; i < fontFamilies.size(); i++)
//    //    {
//    //        settings.setArrayIndex(i);
//    //        settings.setValue(TEXT_SETTING, fontFamilies.keys().at(i));
//    //        settings.setValue(FONT_SETTING, fontFamilies.value(fontFamilies.keys().at(i)));
//    //    }
//    //    settings.endArray();

//    settings.endGroup();
//}

void BibleTextBrowser::display(QList<int> bibletextIds, int idLocation, int normalisedChapterLocation)
{
    ChapterDisplayer* item;

    if(bibletextIds.size() > 1)
    {
        item = new ParallelTextChapterDisplayer(this, bibletextIds, idLocation, normalisedChapterLocation);
    }
    else
    {
        item = new SingleTextChapterDisplayer(this, bibletextIds, idLocation, normalisedChapterLocation);
    }

    webHistory->display(item);
}

void BibleTextBrowser::handleEvent(Event* event)
{
    if(event->getEventType() == EventType::BIBLE_REFERENCE)
    {
        BibleReferenceEvent* bibleReferenceEvent = static_cast<BibleReferenceEvent*>(event);
        QList<int> texts;
        texts.append(1);
        texts.append(2);
        texts.append(3);
        display(texts, bibleReferenceEvent->verseReference);
        setFocus();
    }
    else
    {
        VWebView::handleEvent(event);
    }
}

void BibleTextBrowser::display(QList<int> bibletextIds, VerseReference verseReference)
{
    if(bibletextIds.size() > 0)
    {
        VerseLocation* newVerseLocation = BibleQuerier::getVerseLocation(bibletextIds.at(0), verseReference);

        if(newVerseLocation != 0)
        {
            int idLocation = newVerseLocation->id;
            int normalisedChapterLocation = newVerseLocation->normalisedChapter;
            delete newVerseLocation;           

            display(bibletextIds, idLocation, normalisedChapterLocation);
        }
        else
        {
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText(verseReference.stringRepresentation + " could not be found in the primary text");
            msgBox.exec();
        }
    }
}


void BibleTextBrowser::resizeEvent(QResizeEvent* event)
{
    ChapterDisplayer* currentItem = (ChapterDisplayer*)webHistory->getCurrentOrNull();
    if(currentItem != 0)
    {
        currentItem->moved();
    }
    QWebView::resizeEvent(event);
}

void BibleTextBrowser::scrolled()
{
    ChapterDisplayer* currentItem = (ChapterDisplayer*)webHistory->getCurrentOrNull();
    if(currentItem != 0)
    {
        currentItem->scrolled();
    }
}

void BibleTextBrowser::moved()
{
    ChapterDisplayer* currentItem = (ChapterDisplayer*)webHistory->getCurrentOrNull();
    if(currentItem != 0)
    {
        currentItem->moved();
    }
}

void BibleTextBrowser::zoomed()
{
    ChapterDisplayer* currentItem = (ChapterDisplayer*)webHistory->getCurrentOrNull();
    if(currentItem != 0)
    {
        currentItem->moved();
    }
    VWebView::zoomed();
}

void BibleTextBrowser::backward()
{
    webHistory->backward();
}

void BibleTextBrowser::forward()
{
    webHistory->forward();
}

void BibleTextBrowser::javaScriptWindowObjectClearedSlot()
{
    page()->mainFrame()->addToJavaScriptWindowObject("scrollListener", scrollListener);

    page()->mainFrame()->evaluateJavaScript("document.onmousewheel = function(){ scrollListener.scrolled(); }");
    page()->mainFrame()->evaluateJavaScript("document.onkeydown = function(evt){ if(evt.keyCode==38 || evt.keyCode==40) { scrollListener.scrolled();} }");

    page()->mainFrame()->evaluateJavaScript("document.onscroll = function(){ scrollListener.moved(); }");

    page()->mainFrame()->addToJavaScriptWindowObject("javascriptClickListener", javascriptClickListener);
}



//QMimeData* BibleTextBrowser::createMimeDataFromSelection() const
//{
//    QMimeData* mimeData = new QMimeData();
//    mimeData->setText(textCursor().selectedText());
//    return mimeData;
//}
