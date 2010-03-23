#include <QDebug>
#include <QtSql>

#include "bibletextbrowser.h"
#include "timer.h"
#include <iostream>
using namespace std;

BibleTextBrowser::BibleTextBrowser() : QTextBrowser()
{
    setUndoRedoEnabled(false);
    setMouseTracking(true);
    currentTextDocument = 0;

     QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
     db.setDatabaseName("bibles.sqlite");
     if (!db.open())
     {
         qDebug() << "couldn't open db" << endl;
         exit(1);
     }

}

QList<TextInfo> BibleTextBrowser::readInParagraph(int paragraphNumber)
{
    QList<TextInfo> textInfos;

    QSqlQuery query;
    query.setForwardOnly(true);

     if(!query.exec("select id, book, chapter, verse, number_in_verse, text "
                    ", morphological_tag, normalised_morph_tag, strongs_number, strongs_lemma, friberg_lemma "
                    "from bible where normalised_chapter = "
                    + QString().setNum(paragraphNumber) +
                    " order by id asc"))


     {
         qDebug() << "failed: " << query.lastError() << endl;
         exit(1);
     }

     while(query.next())
     {
         int id = query.value(0).toInt();         
         QString book = query.value(1).toString();
         int chapter = query.value(2).toInt();
         int verse = query.value(3).toInt();
         int numberInVerse = query.value(4).toInt();
         QString text = query.value(5).toString();
         QString morphologicalTag = query.value(6).toString();


         QByteArray normalisedMorphTagBytes = QByteArray::fromBase64(query.value(7).toByteArray());
         QDataStream stream(normalisedMorphTagBytes);
         QBitArray normalisedMorphTag(81);
         stream >> normalisedMorphTag;

         int strongsNumber = query.value(8).toInt();
         QString strongsLemma = query.value(9).toString();
         QString fribergLemma = query.value(10).toString();

         TextInfo textInfo(id, book, chapter, verse, numberInVerse, text, morphologicalTag, normalisedMorphTag, strongsNumber, strongsLemma, fribergLemma);
         textInfos.append(textInfo);
     }

    return textInfos;
}

int BibleTextBrowser::insertParagraphIntoDocument(int id, QList<TextInfo> textInfos)
{
     QTextCharFormat defaultFormat;
     defaultFormat.setFontPointSize(14);

     QTextCharFormat lightGrayFormat;
     lightGrayFormat.setFontPointSize(14);
     lightGrayFormat.setBackground(QBrush(Qt::lightGray));

     QTextCharFormat superscriptFormat;
     superscriptFormat.setForeground(QBrush(Qt::red));
     superscriptFormat.setFontPointSize(14);
     superscriptFormat.setVerticalAlignment(QTextCharFormat::AlignSuperScript);

     QTextCharFormat boldFormat;
     boldFormat.setFontPointSize(14);
     boldFormat.setFontWeight(QFont::Bold);


     bool inChosenVerse = false;

     int moveToPosition = 0;
     currentTextCursor->setCharFormat(defaultFormat);
     for(int i=0; i<textInfos.size(); i++)
     {
         TextInfo textInfo = textInfos.at(i);

         if(textInfo.chapter == 1 && textInfo.verse ==1 && textInfo.numberInVerse == 1)
         {           
           currentTextCursor->insertText(textInfo.book, boldFormat);
           currentTextCursor->insertText("\n");
           currentTextCursor->setCharFormat(defaultFormat);
         }
         if(textInfo.verse == 1 && textInfo.numberInVerse == 1)
         {
             currentTextCursor->insertText("Ch "+QString().setNum(textInfo.chapter), boldFormat);
             currentTextCursor->insertText("\n");
             currentTextCursor->setCharFormat(defaultFormat);
         }
         if(textInfo.numberInVerse == 1)
         {
             currentTextCursor->insertText(QString().setNum(textInfo.verse), superscriptFormat);
             currentTextCursor->setCharFormat(defaultFormat);
             inChosenVerse = false;
         }

         int start = currentTextCursor->position();

         if(textInfo.id == id)
         {
             inChosenVerse = true;
             currentTextCursor->setCharFormat(lightGrayFormat);
         }

         currentTextCursor->insertText(textInfo.text);
         int end = currentTextCursor->position();

         if(inChosenVerse)
             moveToPosition = end;

         BaseTextUnit textUnit(start, end);
         textUnits.insert(textUnit, textInfo);

         currentTextCursor->insertText(" ");
     }

     currentTextCursor->insertText("\n");
     currentTextCursor->insertText("\n");


     return moveToPosition;
}

void BibleTextBrowser::display(VerseReference verseReference)
{

    QTextDocument* toDelete = 0;
    if(currentTextDocument != 0)
    {
        toDelete = currentTextDocument;
        delete currentTextCursor;
        textUnits.clear();
    }

    currentTextDocument = new QTextDocument();
    currentTextCursor = new QTextCursor(currentTextDocument);



     QSqlQuery query;

     QString queryString;
     QTextStream(&queryString) << "select normalised_chapter, id from bible where book_number=" << verseReference.book
             << " and chapter=" << verseReference.chapter
             << " and verse=" << verseReference.verse << " and number_in_verse=1";

     int normalisedChapter = 1;
     int id = 1;
     timer t;
     t.start();
     if(!query.exec(queryString))
     {
         qDebug() << "failed: " << query.lastError() << endl;
         exit(1);
     }


     if(query.next())
     {
         int normalisedChapter = query.value(0).toInt();
         int id = query.value(1).toInt();

         timer t;
         t.start();
         QList<TextInfo> textInfos = readInParagraph(normalisedChapter);
         cout << "read time  : " << t << endl;


         currentTextCursor->beginEditBlock();

         t.restart();
         int moveToPosition = insertParagraphIntoDocument(id, textInfos);
         cout << "insert time: " << t << endl;

         currentTextCursor->endEditBlock();


       t.restart();
       setDocument(currentTextDocument);
       cout << "set document time: " << t << endl;

       if(toDelete != 0)
           delete toDelete;


       currentTextCursor->setPosition(moveToPosition);
       setTextCursor(*currentTextCursor);

       ensureCursorVisible();

       window()->setWindowTitle("Bible Displayer - " + verseReference.stringRepresentation);
    }
    else
    {
        qDebug() << "did not find chapter bible verse";
    }

}

BaseTextUnit BibleTextBrowser::key(QPoint point)
{
    QTextCursor textCursor = cursorForPosition(point);
    return BaseTextUnit(textCursor.position(), textCursor.position());
}

void BibleTextBrowser::mouseMoveEvent ( QMouseEvent * e )
{
    BaseTextUnit keyTextUnit = key(e->pos());
    if(textUnits.contains(keyTextUnit))
    {
        emit wordHoveredOver(textUnits.value(keyTextUnit));
    }

    QTextBrowser::mouseMoveEvent(e);
}

void BibleTextBrowser::mousePressEvent(QMouseEvent* e)
{
    BaseTextUnit keyTextUnit = key(e->pos());
    if(textUnits.contains(keyTextUnit))
    {
        emit wordClicked(textUnits.value(keyTextUnit));
    }

    QTextBrowser::mousePressEvent(e);
}

