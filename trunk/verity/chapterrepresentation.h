#ifndef CHAPTERREPRESENTATION_H
#define CHAPTERREPRESENTATION_H

#include <QTextDocumentFragment>
#include <QMap>
#include "basetextunit.h"
#include "textinfo.h"

class ChapterRepresentation
{
protected:
    int normalisedChapter;
    QTextDocumentFragment textDocumentFragment;
    QMap<BaseTextUnit, TextInfo> textUnits;

    int selectionStart;
    int selectionEnd;

    QString text;

public:
    ChapterRepresentation();
    ChapterRepresentation(QString text,
                          int normalisedChapter,
                          QTextDocumentFragment textDocumentFragment,
                          QMap<BaseTextUnit, TextInfo> textUnits,
                          int selectionStart,
                          int selectionEnd);
    int lastPosInFragment();
    int firstPosInFragment();

    int getNormalisedChapter();
    int getSelectionStart();
    int getSelectionEnd();
    QTextDocumentFragment getTextDocumentFragment();

    TextInfo* getTextInfo(int localPos);
};

#endif // CHAPTERREPRESENTATION_H
