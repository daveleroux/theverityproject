#include "chapterrepresentation.h"
#include <QDebug>

ChapterRepresentation::ChapterRepresentation()
{
}

ChapterRepresentation::ChapterRepresentation(int normalisedChapter,
                                             QTextDocumentFragment textDocumentFragment,
                                             QMap<BaseTextUnit, TextInfo> textUnits,
                                             int selectionStart,
                                             int selectionEnd)
{
    this->normalisedChapter = normalisedChapter;
    this->textDocumentFragment = textDocumentFragment;
    this->textUnits = textUnits;
    this->selectionStart = selectionStart;
    this->selectionEnd = selectionEnd;
}

