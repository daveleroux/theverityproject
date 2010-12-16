#include "chapterdisplayer.h"
#include "biblequerier.h"
#include "textinfo.h"
#include <QScrollBar>
#include <QDebug>

ChapterDisplayer::ChapterDisplayer(QTextBrowser* textBrowser, QList<QString> texts, QMap<QString, QString> fontFamilies)
{
    this->textBrowser = textBrowser;
    this->texts = texts;
    this->fontFamilies = fontFamilies;
}

QString ChapterDisplayer::getPrimaryText()
{
    return texts.at(0);
}

QString ChapterDisplayer::getFontFamily(QString text)
{
    return fontFamilies.value(text);
}

void ChapterDisplayer::display(int id, int normalisedChapter)
{
    for(int i=0; i<chapters.values().size(); i++)
    {
        delete chapters.values().at(i);
    }
    chapters.clear();

    int fromPosLocal;
    int toPosLocal;

    ChapterRepresentation* chapterRepresentation = insertFirstChapter(normalisedChapter, id);

    fromPosLocal = chapterRepresentation->getSelectionStart();
    toPosLocal = chapterRepresentation->getSelectionEnd();

    scrollToCentre(normalisedChapter, fromPosLocal, toPosLocal);

    while(mustPrepend())
    {
        if(validChapter(getFirstNormChapter()-1))
        {
            prependChapter();
            scrollToCentre(normalisedChapter, fromPosLocal, toPosLocal);
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
            scrollToCentre(normalisedChapter, fromPosLocal, toPosLocal);
        }
        else
        {
            break;
        }
    }
}

bool ChapterDisplayer::mustAppend(int min, int max, int value, int pageStep)
{
    return value >= (max - pageStep);
}

bool ChapterDisplayer::mustAppend()
{
    QScrollBar* scrollBar = textBrowser->verticalScrollBar();
    return mustAppend(scrollBar->minimum(), scrollBar->maximum(), scrollBar->value(), scrollBar->pageStep());
}


bool ChapterDisplayer::mustPrepend(int min, int max, int value, int pageStep)
{
    return value <= (min + pageStep);
}

bool ChapterDisplayer::mustPrepend()
{
    QScrollBar* scrollBar = textBrowser->verticalScrollBar();
    return mustPrepend(scrollBar->minimum(), scrollBar->maximum(), scrollBar->value(), scrollBar->pageStep());
}

void ChapterDisplayer::addChapter(ChapterRepresentation* chapterRepresentation, bool append)
{
    chapters.insert(chapterRepresentation->getNormalisedChapter(), chapterRepresentation);
    QTextCursor textCursor(textBrowser->document());

    if(append)
        textCursor.movePosition(QTextCursor::End);
    else
        textCursor.movePosition(QTextCursor::Start);

    textCursor.insertFragment(chapterRepresentation->getTextDocumentFragment());

    calculateAndSendChapterStarts();
}

bool ChapterDisplayer::validChapter(int proposedChapter)
{
    TextSpecificData* textSpecificData = BibleQuerier::getTextSpecificData(getPrimaryText());
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


void ChapterDisplayer::scrollToCentre(int normCh, int fromPosLocal, int toPosLocal)
{
    //makes a few assumptions about the scrollbar, doesn't do things proportionately

    QScrollBar* scrollBar = textBrowser->verticalScrollBar();
    if(scrollBar->isVisible())
    {
        int fromPos = convertPosToGlobal(normCh, fromPosLocal);
        QTextCursor fromCursor(textBrowser->document());
        fromCursor.setPosition(fromPos);

        int toPos = convertPosToGlobal(normCh, toPosLocal);
        QTextCursor toCursor(textBrowser->document());
        toCursor.setPosition(toPos);

        QRect fromRect = textBrowser->cursorRect(fromCursor); //viewport co-ords
        QRect toRect = textBrowser->cursorRect(toCursor); //viewport co-ords

        int y = (fromRect.top() + toRect.bottom())/2;

        scrollBar->setValue(scrollBar->value() + y - textBrowser->viewport()->height()/2);
    }
}

int ChapterDisplayer::convertPosToGlobal(int normCh, int localPos)
{
    int total = 0;
    for(int i=0; i<chapters.keys().size(); i++)
    {
        int key = chapters.keys().at(i);
        if(key == normCh)
            return total + localPos;

        ChapterRepresentation* chRep = chapters.value(key);

        total += chRep->lastPosInFragment() + 2;
    }
    return 0; //throw exception?
}


void ChapterDisplayer::checkCanScroll()
{
    while(mustPrepend())
    {
        if(validChapter(getFirstNormChapter()-1))
        {
            int originalHeight = textBrowser->document()->size().height();
            prependChapter();
            int newHeight = textBrowser->document()->size().height();
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
            appendChapter();

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

void ChapterDisplayer::scrollDown(int pixels)
{
    QScrollBar* scrollBar = textBrowser->verticalScrollBar();
    if(scrollBar->isVisible())
    {
        scrollBar->setValue(scrollBar->value() + pixels);
    }
}

void ChapterDisplayer::scrollUp(int pixels)
{
    scrollDown(-pixels);
}

int ChapterDisplayer::getCurrentChapter()
{
    QPoint centre(textBrowser->viewport()->width()/2, textBrowser->viewport()->height()/2);
    QTextCursor centreCursor = textBrowser->cursorForPosition(centre);
    int centralPos = centreCursor.position();

    int chContainingCentralPos = 0;

    for(int i=0; i<chapters.values().size(); i++)
    {
        ChapterRepresentation* chRep = chapters.values().at(i);

        int globalPos = convertPosToGlobal(chRep->getNormalisedChapter(), 0);
        if(centralPos < globalPos)
            break;

        chContainingCentralPos = chRep->getNormalisedChapter();
    }

    return chContainingCentralPos;
}

bool ChapterDisplayer::canUnloadLastChapter()
{
    QScrollBar* scrollBar = textBrowser->verticalScrollBar();
    if(scrollBar->isVisible())
    {
        int chContainingCentralPos = getCurrentChapter();

        ChapterRepresentation* chRep = chapters.values().at(chapters.values().size()-1);


        if( chRep->getNormalisedChapter() > chContainingCentralPos + 1 )
        {
            //fake remove it and see if 'mustAppend', if so then don't remove it

            int firstGlobalPos = convertPosToGlobal(chRep->getNormalisedChapter(), chRep->firstPosInFragment());

            QTextCursor textCursor(textBrowser->document());
            textCursor.setPosition(firstGlobalPos);


            QRect rect = textBrowser->cursorRect(textCursor);//viewport co-ords

            QScrollBar* scrollBar = textBrowser->verticalScrollBar();

            return !mustAppend(scrollBar->minimum(), scrollBar->value()+ rect.top()-scrollBar->pageStep(), scrollBar->value(), scrollBar->pageStep());
        }
    }
    return false;
}


bool ChapterDisplayer::canUnloadFirstChapter()
{
    QScrollBar* scrollBar = textBrowser->verticalScrollBar();
    if(scrollBar->isVisible())
    {
        int chContainingCentralPos = getCurrentChapter();

        ChapterRepresentation* chRep = chapters.values().at(0);

        if(chRep->getNormalisedChapter() < chContainingCentralPos-1)
        {
            //fake remove it and see if 'mustPrepend', if so then don't remove it

            int endGlobalPos = convertPosToGlobal(chRep->getNormalisedChapter(), chRep->lastPosInFragment()+2);

            QTextCursor textCursor(textBrowser->document());
            textCursor.setPosition(endGlobalPos);

            QRect rect = textBrowser->cursorRect(textCursor); //viewport co-ords

            QScrollBar* scrollBar = textBrowser->verticalScrollBar();

            return !mustPrepend(scrollBar->value() + rect.bottom(), scrollBar->maximum(), scrollBar->value(), scrollBar->pageStep());
        }
    }
    return false;
}

void ChapterDisplayer::unloadLastChapter()
{    

    ChapterRepresentation* chRep = chapters.values().at(chapters.values().size()-1);

    QTextCursor textCursor(textBrowser->document());

    textCursor.beginEditBlock();

//    textCursor.setPosition(textBrowser->document()->lastBlock());

    textCursor.movePosition(QTextCursor::End);

//    int startPos = convertPosToGlobal(chRep.getNormalisedChapter(), chRep.firstPosInFragment());
    int startPos = convertPosToGlobal(chRep->getNormalisedChapter(), 2);

    while(textCursor.position() >  startPos)
    {
        textCursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::KeepAnchor);
    }


    textCursor.deletePreviousChar();

    textCursor.endEditBlock();

    chapters.remove(chRep->getNormalisedChapter());
    delete chRep;

    calculateAndSendChapterStarts();
}

void ChapterDisplayer::unloadFirstChapter()
{    
    QScrollBar* scrollBar = textBrowser->verticalScrollBar();

    int originalHeight = textBrowser->document()->size().height();
    int originalValue = scrollBar->value();

    ChapterRepresentation* chRep = chapters.values().at(0);

    int endPos = chRep->lastPosInFragment()+2;

    QTextCursor textCursor(textBrowser->document());

    textCursor.beginEditBlock();

    textCursor.setPosition(0);

    while(textCursor.position() < endPos )
    {
        textCursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
    }
    textCursor.deletePreviousChar();

    textCursor.endEditBlock();

    int newHeight = textBrowser->document()->size().height();

    textBrowser->verticalScrollBar()->setValue(originalValue - (originalHeight-newHeight));

    chapters.remove(chRep->getNormalisedChapter());
    delete chRep;

    calculateAndSendChapterStarts();
}

QList<int> ChapterDisplayer::chapterStartPositions()
{
    QList<int> result;
    for(int i=0; i<chapters.keys().size(); i++)
    {
        int key = chapters.keys().at(i);
        result.append(convertPosToGlobal(key, 0));
    }
    return result;
}

void ChapterDisplayer::mousePressed(QPoint point)
{
    if(chapters.size() > 0)
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

        ChapterRepresentation* chRep = chapters.values().at(index);

        TextInfo* textInfo = chRep->getTextInfo(localPos);
        if(textInfo != 0)
        {
            emit wordClicked(textInfo);
            delete textInfo;
        }
    }
}

QTextCharFormat ChapterDisplayer::getBoldFormat()
{
    QTextCharFormat result;
    result.setFontWeight(QFont::Bold);
    return result;
}

QTextCharFormat ChapterDisplayer::getSuperscriptFormat()
{
    QTextCharFormat result;
    result.setForeground(QBrush(Qt::red));
    result.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
    return result;
}

QTextCharFormat ChapterDisplayer::getDefaultFormat(QString text)
{
    QTextCharFormat result;
    result.setFontFamily(getFontFamily(text));
    return result;
}

void ChapterDisplayer::highlight(int startPos, int endPos)
{
    QTextCursor cursor(textBrowser->document());
    cursor.setPosition(startPos);

    while(cursor.position() <  endPos)
    {
        cursor.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor);
    }


    QTextCharFormat format = getDefaultFormat(getPrimaryText());
    format.setBackground(QBrush(Qt::lightGray));

    cursor.setCharFormat(format);
}

ChapterRepresentation* ChapterDisplayer::insertFirstChapter(int normalisedChapter, int idLocation)
{
    ChapterRepresentation* chapter =  constructChapterRepresentation(normalisedChapter, idLocation);
    addChapter(chapter, true);

    highlight(chapter->getSelectionStart(), chapter->getSelectionEnd());

    return chapter;
}

ChapterRepresentation* ChapterDisplayer::appendChapter()
{
    ChapterRepresentation* chapter =  constructChapterRepresentation(getLastNormChapter()+1);
    addChapter(chapter, true);
    return chapter;
}

ChapterRepresentation* ChapterDisplayer::prependChapter()
{
    ChapterRepresentation* chapter =  constructChapterRepresentation(getFirstNormChapter()-1);
    addChapter(chapter, false);
    return chapter;
}

void ChapterDisplayer::calculateAndSendChapterStarts()
{
    QList<int> chStartPositions = chapterStartPositions();
    QList<int> chStartPixels;
    for(int i=0; i<chStartPositions.size(); i++)
    {
        QTextCursor textCursor(textBrowser->document());
        textCursor.setPosition(chStartPositions.at(i));
        QRect rect = textBrowser->cursorRect(textCursor); //viewport co-ords

        chStartPixels.append(textBrowser->verticalScrollBar()->value()+rect.top());
    }

    emit chapterStarts(chStartPixels);
}

