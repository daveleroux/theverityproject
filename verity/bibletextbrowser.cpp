#include <QDebug>
#include <QtSql>

#include <QTextCursor>
#include <QApplication>

#include "bibletextbrowser.h"
#include "timer.h"
#include "globalvariables.h"

#include <iostream>
using namespace std;

BibleTextBrowser::BibleTextBrowser() : QTextBrowser()
{
    QSettings settings(PROGRAM_NAME, PROGRAM_NAME);
    settings.beginGroup(BIBLE_TEXT_BROWSER_SETTING_GROUP);


    QString fontFamily = settings.value(FONT_FAMILY_SETTING,QApplication::font().family()).toString();

    settings.endGroup();


    setUndoRedoEnabled(false);
    setMouseTracking(true);
    zoomIn(3);    

    documentRepresentation = new DocumentRepresentation(document(), this, fontFamily);
    connect(documentRepresentation, SIGNAL(selectionRequest(int,int)), this, SLOT(select(int,int)));
    connect(documentRepresentation, SIGNAL(wordClicked(TextInfo)), this, SIGNAL(wordClicked(TextInfo)));
    connect(documentRepresentation, SIGNAL(chapterStarts(QList<int>)), this, SIGNAL(chapterStarts(QList<int>)));

    markedScrollBar = new QSnapScrollBar(this);
    setVerticalScrollBar(markedScrollBar);

    connect(this, SIGNAL(chapterStarts(QList<int>)), markedScrollBar, SLOT(defineSnapPoints(QList<int>)));
    connect(this, SIGNAL(chapterStarts(QList<int>)), this, SLOT(tmp(QList<int>)));
}

void BibleTextBrowser::writeOutSettings()
{
    QSettings settings(PROGRAM_NAME, PROGRAM_NAME);

    settings.beginGroup(BIBLE_TEXT_BROWSER_SETTING_GROUP);
    settings.setValue(FONT_FAMILY_SETTING, documentRepresentation->defaultFormat.fontFamily());
    settings.endGroup();
}

void BibleTextBrowser::display(VerseReference verseReference)
{
    documentRepresentation->display(verseReference);
}


void BibleTextBrowser::mouseMoveEvent ( QMouseEvent * e )
{
    //    qDebug() << "mouse at position: " << cursorForPosition(e->pos()).position();
    //    BaseTextUnit keyTextUnit = key(e->pos());
    //    if(textUnits.contains(keyTextUnit))
    //    {
    //        emit wordHoveredOver(textUnits.value(keyTextUnit));
    //    }

    QTextBrowser::mouseMoveEvent(e);
}

void BibleTextBrowser::mousePressEvent(QMouseEvent* e)
{
    //    BaseTextUnit keyTextUnit = key(e->pos());
    //    if(textUnits.contains(keyTextUnit))
    //    {
    //        emit wordClicked(textUnits.value(keyTextUnit));
    //    }

    documentRepresentation->mousePressed(e->pos());

    QTextBrowser::mousePressEvent(e);
}

void BibleTextBrowser::tmp(QList<int> pixelStarts)
{
    markedScrollBar->removeAllMarks();

    for(int i=0;i<pixelStarts.size();i++)
    {
        if(i!=0)
            markedScrollBar->addMark(pixelStarts.at(i),QColor(Qt::black),"");
    }
}
void BibleTextBrowser::wheelEvent ( QWheelEvent * e )
{
    QTextBrowser::wheelEvent(e);
    documentRepresentation->checkCanScroll();
}

void BibleTextBrowser::select(int startPos, int endPos)
{
    QTextCursor cursor = textCursor();
    cursor.setPosition(startPos);
    while(cursor.position() < endPos)
    {
        cursor.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor);
    }
    setTextCursor(cursor);
}



