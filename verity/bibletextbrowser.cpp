#include <QDebug>
#include <QtSql>

#include <QTextCursor>
#include <QApplication>

#include "bibletextbrowser.h"
#include "timer.h"
#include "globalvariables.h"
#include "biblequerier.h"
#include "paralleltextchapterdisplayer.h"
#include "singletextchapterdisplayer.h"

#include <iostream>
using namespace std;

BibleTextBrowser::BibleTextBrowser() : QTextBrowser()
{
    QSettings settings(PROGRAM_NAME, PROGRAM_NAME);
    settings.beginGroup(BIBLE_TEXT_BROWSER_SETTING_GROUP);


    fontFamily = settings.value(FONT_FAMILY_SETTING,QApplication::font().family()).toString();

    settings.endGroup();


    setUndoRedoEnabled(false);
    setMouseTracking(true);
    zoomIn(3);    

//    documentRepresentation = new DocumentRepresentation(document(), this, fontFamily);
//    connect(documentRepresentation, SIGNAL(selectionRequest(int,int)), this, SLOT(select(int,int)));

//    connect(documentRepresentation, SIGNAL(chapterStarts(QList<int>)), this, SIGNAL(chapterStarts(QList<int>)));

    markedScrollBar = new QSnapScrollBar(this);
    setVerticalScrollBar(markedScrollBar);

    connect(this, SIGNAL(chapterStarts(QList<int>)), markedScrollBar, SLOT(defineSnapPoints(QList<int>)));
//    connect(this, SIGNAL(chapterStarts(QList<int>)), this, SLOT(tmp(QList<int>)));
    chapterDisplayer = 0;
}

void BibleTextBrowser::writeOutSettings()
{
    QSettings settings(PROGRAM_NAME, PROGRAM_NAME);

    settings.beginGroup(BIBLE_TEXT_BROWSER_SETTING_GROUP);
    settings.setValue(FONT_FAMILY_SETTING, fontFamily);
    settings.endGroup();
}

void BibleTextBrowser::display(QList<QString> texts, int idLocation, int normalisedChapterLocaction)
{
    document()->clear();

    if(chapterDisplayer != 0)
    {
        delete chapterDisplayer;
    }

    if(texts.size() > 1)
    {
        chapterDisplayer = new ParallelTextChapterDisplayer(this, texts);
    }
    else
    {
        chapterDisplayer = new SingleTextChapterDisplayer(this, texts);
    }

    connect(chapterDisplayer, SIGNAL(wordClicked(TextInfo*)), this, SIGNAL(wordClicked(TextInfo*)));

    chapterDisplayer->display(idLocation, normalisedChapterLocaction);
}

void BibleTextBrowser::display(QList<QString> texts, VerseReference verseReference)
{
    if(texts.size() > 0)
    {
        VerseLocation* newVerseLocation = BibleQuerier::getVerseLocation(texts.at(0), verseReference);

        if(newVerseLocation != 0)
        {
            int idLocation = newVerseLocation->id;
            int normalisedChapterLocation = newVerseLocation->normalisedChapter;
            delete newVerseLocation;

            window()->setWindowTitle(PROGRAM_NAME + " - " + verseReference.stringRepresentation);

            display(texts, idLocation, normalisedChapterLocation);
        }
    }
}


void BibleTextBrowser::mouseMoveEvent ( QMouseEvent * e )
{
    QTextBrowser::mouseMoveEvent(e);
}

void BibleTextBrowser::mousePressEvent(QMouseEvent* e)
{
    chapterDisplayer->mousePressed(e->pos());
    QTextBrowser::mousePressEvent(e);
}

//void BibleTextBrowser::tmp(QList<int> pixelStarts)
//{
//    markedScrollBar->removeAllMarks();

//    for(int i=0;i<pixelStarts.size();i++)
//    {
//        if(i!=0)
//            markedScrollBar->addMark(pixelStarts.at(i),QColor(Qt::black),"");
//    }
//}

void BibleTextBrowser::wheelEvent ( QWheelEvent * e )
{
    QTextBrowser::wheelEvent(e);
    chapterDisplayer->checkCanScroll();
}

//void BibleTextBrowser::select(int startPos, int endPos)
//{
//    QTextCursor cursor = textCursor();
//    cursor.setPosition(startPos);
//    while(cursor.position() < endPos)
//    {
//        cursor.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor);
//    }
//    setTextCursor(cursor);
//}
