#include "chapterrepresentation.h"
#include <QDebug>

ChapterRepresentation::ChapterRepresentation()
{
}

ChapterRepresentation::ChapterRepresentation(QString text,
                                             int normalisedChapter,
                                             QTextDocumentFragment textDocumentFragment,
                                             QMap<BaseTextUnit, TextInfo> textUnits,
                                             int selectionStart,
                                             int selectionEnd)
{
    this->text = text;
    this->normalisedChapter = normalisedChapter;
    this->textDocumentFragment = textDocumentFragment;
    this->textUnits = textUnits;
    this->selectionStart = selectionStart;
    this->selectionEnd = selectionEnd;
}

int ChapterRepresentation::lastPosInFragment()
{
    QMap<BaseTextUnit, TextInfo>::iterator it = textUnits.end();
    it--;
    BaseTextUnit finalTextUnit = it.key();
    return finalTextUnit.end;
}

int ChapterRepresentation::firstPosInFragment() //is this always 0?
{
    QMap<BaseTextUnit, TextInfo>::iterator it = textUnits.begin();
    BaseTextUnit firstTextUnit = it.key();
    return firstTextUnit.start;
}

int ChapterRepresentation::getNormalisedChapter()
{
    return normalisedChapter;
}

int ChapterRepresentation::getSelectionStart()
{
    return selectionStart;
}

int ChapterRepresentation::getSelectionEnd()
{
    return selectionEnd;
}

QTextDocumentFragment ChapterRepresentation::getTextDocumentFragment()
{
    return textDocumentFragment;
}

TextInfo* ChapterRepresentation::getTextInfo(int localPos)
{
    BaseTextUnit key(localPos, localPos);
    if(textUnits.contains(key))
    {
        return new TextInfo(textUnits.value(key));
    }
    return 0;
}
