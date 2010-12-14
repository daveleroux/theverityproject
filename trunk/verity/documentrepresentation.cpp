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












