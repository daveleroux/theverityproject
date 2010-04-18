#include <QDebug>
#include <QtSql>

#include <QTextCursor>

#include "bibletextbrowser.h"
#include "timer.h"
#include <iostream>
using namespace std;

BibleTextBrowser::BibleTextBrowser() : QTextBrowser()
{
    setUndoRedoEnabled(false);
    setMouseTracking(true);
    zoomIn(3);

    documentRepresentation = new DocumentRepresentation(document(), this);
    connect(documentRepresentation, SIGNAL(selectionRequest(int,int)), this, SLOT(select(int,int)));
    connect(documentRepresentation, SIGNAL(wordClicked(TextInfo)), this, SIGNAL(wordClicked(TextInfo)));
    connect(documentRepresentation, SIGNAL(chapterStarts(QList<int>)), this, SIGNAL(chapterStarts(QList<int>)));

    markedScrollBar = new MarkedScrollBar(this);
    setVerticalScrollBar(markedScrollBar);

    connect(this, SIGNAL(chapterStarts(QList<int>)), this, SLOT(tmp(QList<int>)));
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


