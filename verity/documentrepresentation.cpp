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

DocumentRepresentation::DocumentRepresentation(QTextDocument* textDocument, QTextBrowser* textBrowser) : QObject()
{
    this->textDocument = textDocument;
    this->textBrowser = textBrowser;
    verseLocation = 0;
    openDatabase(); //need to put these data retrieval details behind an interface
    initialiseMinAndMaxChapters();
}

void DocumentRepresentation::openDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(DATA_PATH + "/bibles.sqlite");
    if (!db.open())
    {
        qDebug() << "couldn't open db" << endl;
        exit(1);
    }
}

void DocumentRepresentation::initialiseMinAndMaxChapters()
{
    QSqlQuery query;
    if(!query.exec("select min(normalised_chapter), max(normalised_chapter) from bible"))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

    if(query.next())
    {
        minChapter = query.value(0).toInt();
        maxChapter = query.value(1).toInt();
    }
    else
    {
        qDebug() << "could not find min and max chapters" << endl;
        exit(1);
    }

}

QList<TextInfo> DocumentRepresentation::readInChapterData(int normalisedChapter)
{
    QList<TextInfo> textInfos;

    QSqlQuery query;
    query.setForwardOnly(true);

    if(!query.exec("select id, book, chapter, verse, number_in_verse, paragraph, text "
                   ", morphological_tag, normalised_morph_tag, strongs_number, strongs_lemma, friberg_lemma "
                   "from bible where normalised_chapter = "
                   + QString().setNum(normalisedChapter) +
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
        bool paragraph = query.value(5).toBool();
        QString text = query.value(6).toString();
        QString morphologicalTag = query.value(7).toString();


        QByteArray normalisedMorphTagBytes = QByteArray::fromBase64(query.value(8).toByteArray());
        QDataStream stream(normalisedMorphTagBytes);
        QBitArray normalisedMorphTag(81);
        stream >> normalisedMorphTag;

        int strongsNumber = query.value(9).toInt();
        QString strongsLemma = query.value(10).toString();
        QString fribergLemma = query.value(11).toString();

        TextInfo textInfo(id, book, chapter, verse, numberInVerse, paragraph, text, morphologicalTag, normalisedMorphTag, strongsNumber, strongsLemma, fribergLemma);
        textInfos.append(textInfo);
    }

    return textInfos;
}

ChapterRepresentation DocumentRepresentation::constructChapterRepresentation(int normalisedChapter)
{
    QList<TextInfo> textInfos = readInChapterData(normalisedChapter);
    QMap<BaseTextUnit, TextInfo> textUnits;

    QTextDocument document;
    QTextCursor textCursor(&document);
    textCursor.beginEditBlock();

    QTextCharFormat defaultFormat;


    QTextCharFormat superscriptFormat;
    superscriptFormat.setForeground(QBrush(Qt::red));
    superscriptFormat.setVerticalAlignment(QTextCharFormat::AlignSuperScript);

    QTextCharFormat boldFormat;
    boldFormat.setFontWeight(QFont::Bold);

    textCursor.insertBlock();

    int selectionStart = -1;
    int selectionEnd = -1;

    textCursor.setCharFormat(defaultFormat);
    for(int i=0; i<textInfos.size(); i++)
    {
        TextInfo textInfo = textInfos.at(i);

        if(textInfo.paragraph)
        {
            textCursor.insertText(QString(QChar::ParagraphSeparator));
            textCursor.insertText(QString(QChar::ParagraphSeparator));
        }
        if(textInfo.chapter == 1 && textInfo.verse ==1 && textInfo.numberInVerse == 1)
        {
            textCursor.insertText(textInfo.book, boldFormat);
            textCursor.insertText(QString(QChar::ParagraphSeparator));
            textCursor.setCharFormat(defaultFormat);
        }
        if(textInfo.verse == 1 && textInfo.numberInVerse == 1)
        {
            textCursor.insertText("Chapter "+QString().setNum(textInfo.chapter), boldFormat);
            textCursor.insertText(QString(QChar::ParagraphSeparator));
            textCursor.setCharFormat(defaultFormat);
        }
        if(textInfo.numberInVerse == 1)
        {
            if(selectionStart != -1 && selectionEnd == -1)
                selectionEnd = textCursor.position();
            textCursor.insertText(QString().setNum(textInfo.verse), superscriptFormat);
            textCursor.setCharFormat(defaultFormat);
        }

        int start = textCursor.position();

        if(verseLocation != 0 && textInfo.id == verseLocation->id)
        {
            selectionStart = start;
        }

        textCursor.insertText(textInfo.text);
        int end = textCursor.position();


        BaseTextUnit textUnit(start, end);
        textUnits.insert(textUnit, textInfo);

        textCursor.insertText(" ");
    }

    textCursor.insertText(QString(QChar::ParagraphSeparator));

    textCursor.endEditBlock();

    if(selectionStart != -1 && selectionEnd == -1)
        selectionEnd = textCursor.position()-2;

    return ChapterRepresentation(normalisedChapter,QTextDocumentFragment(&document), textUnits, selectionStart, selectionEnd);
}


int DocumentRepresentation::convertPosToGlobal(int normCh, int localPos)
{
    int total = 0;
    for(int i=0; i<chapters.keys().size(); i++)
    {
        int key = chapters.keys().at(i);
        if(key == normCh)
            return total + localPos;

        ChapterRepresentation chRep = chapters.value(key);
        QMap<BaseTextUnit, TextInfo>::iterator it = chRep.textUnits.end();
        it--;
        BaseTextUnit finalTextUnit = it.key();

        total += finalTextUnit.end + 2;
    }
    return 0; //throw exception?
}

void DocumentRepresentation::calculateAndSendChapterStarts()
{
    QList<int> chStartPositions = chapterStartPositions();
    QList<int> chStartPixels;
    for(int i=0; i<chStartPositions.size(); i++)
    {
        QTextCursor textCursor(textDocument);
        textCursor.setPosition(chStartPositions.at(i));
        QRect rect = textBrowser->cursorRect(textCursor); //viewport co-ords

        chStartPixels.append(textBrowser->verticalScrollBar()->value()+rect.top());
    }

    emit chapterStarts(chStartPixels);
}

void DocumentRepresentation::addChapter(ChapterRepresentation chapterRepresentation, bool append)
{
    chapters.insert(chapterRepresentation.normalisedChapter, chapterRepresentation);
    QTextCursor textCursor(textDocument);

    if(append)
        textCursor.movePosition(QTextCursor::End);
    else
        textCursor.movePosition(QTextCursor::Start);

    textCursor.insertFragment(chapterRepresentation.textDocumentFragment);

    calculateAndSendChapterStarts();
}

void DocumentRepresentation::appendChapter(ChapterRepresentation chapterRepresentation)
{
    qDebug() << "appending " << chapterRepresentation.normalisedChapter;
    addChapter(chapterRepresentation, true);
}

void DocumentRepresentation::prependChapter(ChapterRepresentation chapterRepresentation)
{
    qDebug() << "prepending" << chapterRepresentation.normalisedChapter;
    addChapter(chapterRepresentation, false);
}

void DocumentRepresentation::scrollToCentre(int normCh, int fromPosLocal, int toPosLocal)
{
    //makes a few assumptions about the scrollbar, doesn't do things proportionately

    QScrollBar* scrollBar = textBrowser->verticalScrollBar();
    if(scrollBar->isVisible())
    {
        int fromPos = convertPosToGlobal(normCh, fromPosLocal);
        QTextCursor fromCursor(textDocument);
        fromCursor.setPosition(fromPos);

        int toPos = convertPosToGlobal(normCh, toPosLocal);
        QTextCursor toCursor(textDocument);
        toCursor.setPosition(toPos);

        QRect fromRect = textBrowser->cursorRect(fromCursor); //viewport co-ords
        QRect toRect = textBrowser->cursorRect(toCursor); //viewport co-ords

        int y = (fromRect.top() + toRect.bottom())/2;

        scrollBar->setValue(scrollBar->value() + y - textBrowser->viewport()->height()/2);
    }
}

void DocumentRepresentation::scrollDown(int pixels)
{
    QScrollBar* scrollBar = textBrowser->verticalScrollBar();
    if(scrollBar->isVisible())
    {
        scrollBar->setValue(scrollBar->value() + pixels);
    }
}

void DocumentRepresentation::scrollUp(int pixels)
{
    scrollDown(-pixels);
}

int DocumentRepresentation::getFirstNormChapter()
{
    QMap<int, ChapterRepresentation>::iterator it = chapters.begin();
    return it.key();
}

int DocumentRepresentation::getLastNormChapter()
{
    QMap<int, ChapterRepresentation>::iterator it = chapters.end();
    it--;
    return it.key();
}

bool DocumentRepresentation::validChapter(int proposedChapter)
{
    return proposedChapter <= maxChapter && proposedChapter >= minChapter;
}

bool DocumentRepresentation::mustAppend(int min, int max, int value, int pageStep)
{
    return value >= (max - pageStep);
}

bool DocumentRepresentation::mustAppend()
{
    QScrollBar* scrollBar = textBrowser->verticalScrollBar();
    return mustAppend(scrollBar->minimum(), scrollBar->maximum(), scrollBar->value(), scrollBar->pageStep());
}


bool DocumentRepresentation::mustPrepend(int min, int max, int value, int pageStep)
{
    return value <= (min + pageStep);
}

bool DocumentRepresentation::mustPrepend()
{
    QScrollBar* scrollBar = textBrowser->verticalScrollBar();
    return mustPrepend(scrollBar->minimum(), scrollBar->maximum(), scrollBar->value(), scrollBar->pageStep());
}

void DocumentRepresentation::display(VerseReference verseReference)
{    
    VerseLocation* newVerseLocation = getVerseLocation(verseReference);
    if(newVerseLocation != 0)
    {
        textBrowser->window()->setWindowTitle(PROGRAM_NAME + " - " + verseReference.stringRepresentation);

        if(verseLocation != 0)
            delete verseLocation;

        verseLocation = newVerseLocation;

        textDocument->clear();
        chapters.clear();

        int fromPosLocal;
        int toPosLocal;

        for(int i= verseLocation->normalisedChapter-1; i<=verseLocation->normalisedChapter+1; i++)
        {
            if(validChapter(i))
            {
                ChapterRepresentation chapterRepresentation = constructChapterRepresentation(i);
                appendChapter(chapterRepresentation);
                if(i == verseLocation->normalisedChapter)
                {
                    fromPosLocal = chapterRepresentation.selectionStart;
                    toPosLocal = chapterRepresentation.selectionEnd;
                }
            }
        }
        scrollToCentre(verseLocation->normalisedChapter, fromPosLocal, toPosLocal);

        QTextCursor cursor(textDocument);
        cursor.setPosition(convertPosToGlobal(verseLocation->normalisedChapter,fromPosLocal));
        QTextCharFormat format;// = cursor.charFormat();

        while(cursor.position() < convertPosToGlobal(verseLocation->normalisedChapter, toPosLocal))
            cursor.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor);


        format.setBackground(QBrush(Qt::lightGray));

        cursor.setCharFormat(format);
//        emit selectionRequest(convertPosToGlobal(verseLocation->normalisedChapter,fromPosLocal),
//                              convertPosToGlobal(verseLocation->normalisedChapter, toPosLocal));


        while(mustPrepend())
        {
            if(validChapter(getFirstNormChapter()-1))
            {
                ChapterRepresentation chRep = constructChapterRepresentation(getFirstNormChapter()-1);
                prependChapter(chRep);
                scrollToCentre(verseLocation->normalisedChapter, fromPosLocal, toPosLocal);
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
                ChapterRepresentation chRep = constructChapterRepresentation(getLastNormChapter()+1);
                appendChapter(chRep);
                scrollToCentre(verseLocation->normalisedChapter, fromPosLocal, toPosLocal);
            }
            else
            {
                break;
            }
        }
    }
}


int DocumentRepresentation::getCurrentChapter()
{
    QPoint centre(textBrowser->viewport()->width()/2, textBrowser->viewport()->height()/2);
    QTextCursor centreCursor = textBrowser->cursorForPosition(centre);
    int centralPos = centreCursor.position();

    int chContainingCentralPos = 0;

    for(int i=0; i<chapters.values().size(); i++)
    {
        ChapterRepresentation chRep = chapters.values().at(i);

        int globalPos = convertPosToGlobal(chRep.normalisedChapter, 0);
        if(centralPos < globalPos)
            break;

        chContainingCentralPos = chRep.normalisedChapter;
    }

    return chContainingCentralPos;
}

void DocumentRepresentation::unloadLastChapter()
{
    QScrollBar* scrollBar = textBrowser->verticalScrollBar();

    ChapterRepresentation chRep = chapters.values().at(chapters.values().size()-1);


    qDebug() << "unloading last" << chRep.normalisedChapter;


    QTextCursor textCursor(textDocument);

    textCursor.beginEditBlock();

    textCursor.setPosition(convertPosToGlobal(chRep.normalisedChapter,0));

    while(textCursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor))
    {
    }

    textCursor.deletePreviousChar();

    textCursor.endEditBlock();

    chapters.remove(chRep.normalisedChapter);

    calculateAndSendChapterStarts();
}

void DocumentRepresentation::unloadFirstChapter()
{
    QScrollBar* scrollBar = textBrowser->verticalScrollBar();

    int originalHeight = textDocument->size().height();
    int originalValue = scrollBar->value();

    ChapterRepresentation chRep = chapters.values().at(0);
    QMap<BaseTextUnit, TextInfo>::iterator it = chRep.textUnits.end();
    it--;
    BaseTextUnit finalTextUnit = it.key();

    qDebug() << "unloading first" << chRep.normalisedChapter;

    int endPos = finalTextUnit.end+2;

    QTextCursor textCursor(textDocument);

    textCursor.beginEditBlock();

    textCursor.setPosition(0);

    while(textCursor.position() < endPos )
    {
        textCursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
    }
    textCursor.deletePreviousChar();

    textCursor.endEditBlock();

    int newHeight = textDocument->size().height();

    textBrowser->verticalScrollBar()->setValue(originalValue - (originalHeight-newHeight));

    chapters.remove(chRep.normalisedChapter);

    calculateAndSendChapterStarts();
}

bool DocumentRepresentation::canUnloadLastChapter()
{
    QScrollBar* scrollBar = textBrowser->verticalScrollBar();
    if(scrollBar->isVisible())
    {
        int chContainingCentralPos = getCurrentChapter();

        ChapterRepresentation chRep = chapters.values().at(chapters.values().size()-1);

        if( chRep.normalisedChapter > chContainingCentralPos + 1 )
        {
            //fake remove it and see if 'mustAppend', if so then don't remove it

            QMap<BaseTextUnit, TextInfo>::iterator it = chRep.textUnits.begin();
            BaseTextUnit firstTextUnit = it.key();

            int firstGlobalPos = convertPosToGlobal(chRep.normalisedChapter, firstTextUnit.start);

            QTextCursor textCursor(textDocument);
            textCursor.setPosition(firstGlobalPos);


            QRect rect = textBrowser->cursorRect(textCursor);//viewport co-ords

            QScrollBar* scrollBar = textBrowser->verticalScrollBar();

            return !mustAppend(scrollBar->minimum(), scrollBar->value()+ rect.top()-scrollBar->pageStep(), scrollBar->value(), scrollBar->pageStep());
        }
    }
    return false;
}

bool DocumentRepresentation::canUnloadFirstChapter()
{
    QScrollBar* scrollBar = textBrowser->verticalScrollBar();
    if(scrollBar->isVisible())
    {
        int chContainingCentralPos = getCurrentChapter();

        ChapterRepresentation chRep = chapters.values().at(0);

        if(chRep.normalisedChapter < chContainingCentralPos-1)
        {
            //fake remove it and see if 'mustPrepend', if so then don't remove it

            QMap<BaseTextUnit, TextInfo>::iterator it = chRep.textUnits.end();
            it--;
            BaseTextUnit finalTextUnit = it.key();

            int endGlobalPos = convertPosToGlobal(chRep.normalisedChapter, finalTextUnit.end+2);

            QTextCursor textCursor(textDocument);
            textCursor.setPosition(endGlobalPos);

            QRect rect = textBrowser->cursorRect(textCursor); //viewport co-ords

            QScrollBar* scrollBar = textBrowser->verticalScrollBar();

            return !mustPrepend(scrollBar->value() + rect.bottom(), scrollBar->maximum(), scrollBar->value(), scrollBar->pageStep());
        }
    }
    return false;
}

void DocumentRepresentation::checkCanScroll()
{
    while(mustPrepend())
    {
        if(validChapter(getFirstNormChapter()-1))
        {
            int originalHeight = textDocument->size().height();
            ChapterRepresentation chRep = constructChapterRepresentation(getFirstNormChapter()-1);
            prependChapter(chRep);
            int newHeight = textDocument->size().height();
            scrollDown(newHeight-originalHeight);
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
            ChapterRepresentation chRep = constructChapterRepresentation(getLastNormChapter()+1);
            appendChapter(chRep);
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
}

VerseLocation* DocumentRepresentation::getVerseLocation(VerseReference verseReference)
{
    QSqlQuery query;

    QString queryString;
    QTextStream(&queryString) << "select normalised_chapter, id from bible where book_number=" << verseReference.book
            << " and chapter=" << verseReference.chapter
            << " and verse=" << verseReference.verse << " and number_in_verse=1";

    if(!query.exec(queryString))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }
    if(query.next())
    {
        int normalisedChapter = query.value(0).toInt();
        int id = query.value(1).toInt();

        return new VerseLocation(id, normalisedChapter);
    }
    else
    {
        qDebug() << "did not find chapter bible verse";
        return 0;
    }
}

QList<int> DocumentRepresentation::chapterStartPositions()
{
    QList<int> result;
    for(int i=0; i<chapters.keys().size(); i++)
    {
        int key = chapters.keys().at(i);
        result.append(convertPosToGlobal(key, 0));
    }
    return result;
}

void DocumentRepresentation::mousePressed(QPoint point)
{
    if(verseLocation != 0)
    {
        QTextCursor textCursor = textBrowser->cursorForPosition(point);

        int globalPos = textCursor.position();
        QList<int> startPositions = chapterStartPositions();
        int index = 0;
        for(int i=1; i<startPositions.size(); i++)
        {
            if(globalPos < startPositions.at(i))
                break;
            index = i;
        }

        int localPos = globalPos - startPositions.at(index);

        ChapterRepresentation chRep = chapters.values().at(index);

        BaseTextUnit key(localPos, localPos);
        if(chRep.textUnits.contains(key))
            emit wordClicked(chRep.textUnits.value(key));
    }
}
