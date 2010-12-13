#include "documentrepresentation.h"
#include <QtSql>
#include <QTextCursor>
#include <QDebug>
#include <QScrollBar>
#include "timer.h"
#include "globalvariables.h"
#include "versereferenceparser.h"
#include <iostream>
using namespace std;

//DocumentRepresentation::DocumentRepresentation(QTextDocument* textDocument, QTextBrowser* textBrowser, QString fontFamily) : QObject()
//{
//    this->textDocument = textDocument;
//    this->textBrowser = textBrowser;
//    defaultFormat.setFontFamily(fontFamily);

//    verseLocation = 0;
//}

//void DocumentRepresentation::calculateAndSendChapterStarts()
//{
//    QList<int> chStartPositions = chapterStartPositions();
//    QList<int> chStartPixels;
//    for(int i=0; i<chStartPositions.size(); i++)
//    {
//        QTextCursor textCursor(textDocument);
//        textCursor.setPosition(chStartPositions.at(i));
//        QRect rect = textBrowser->cursorRect(textCursor); //viewport co-ords

//        chStartPixels.append(textBrowser->verticalScrollBar()->value()+rect.bottom());
//    }

//    emit chapterStarts(chStartPixels);
//}




//int DocumentRepresentation::getCurrentChapter()
//{
//    QPoint centre(textBrowser->viewport()->width()/2, textBrowser->viewport()->height()/2);
//    QTextCursor centreCursor = textBrowser->cursorForPosition(centre);
//    int centralPos = centreCursor.position();

//    int chContainingCentralPos = 0;

//    for(int i=0; i<chapters.values().size(); i++)
//    {
//        ChapterRepresentation chRep = chapters.values().at(i);

//        int globalPos = convertPosToGlobal(chRep.normalisedChapter, 0);
//        if(centralPos < globalPos)
//            break;

//        chContainingCentralPos = chRep.normalisedChapter;
//    }

//    return chContainingCentralPos;
//}








