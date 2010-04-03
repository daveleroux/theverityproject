#ifndef CHAPTERREPRESENTATION_H
#define CHAPTERREPRESENTATION_H

#include <QTextDocumentFragment>
#include <QMap>
#include "basetextunit.h"


class ChapterRepresentation
{
public:
    int normalisedChapter;
    QTextDocumentFragment textDocumentFragment;
    QMap<BaseTextUnit, TextInfo> textUnits;

    int selectionStart;
    int selectionEnd;

    ChapterRepresentation();
    ChapterRepresentation(int normalisedChapter, QTextDocumentFragment textDocumentFragment, QMap<BaseTextUnit, TextInfo> textUnits, int selectionStart, int selectionEnd);
};

#endif // CHAPTERREPRESENTATION_H
