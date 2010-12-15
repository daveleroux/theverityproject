#ifndef PARALLELCHAPTERREPRESENTATION_H
#define PARALLELCHAPTERREPRESENTATION_H

#include "chapterrepresentation.h"

class ParallelChapterRepresentation : public ChapterRepresentation
{
protected:
    QMap<QString, int> firstIdsMap;
    QMap<QString, int> lastIdsMap;

public:
    ParallelChapterRepresentation(QString text,
                                  int normalisedChapter,
                                  QTextDocumentFragment textDocumentFragment,
                                  QMap<BaseTextUnit, TextInfo> textUnits,
                                  int selectionStart,
                                  int selectionEnd,
                                  QMap<QString, int> firstIdsMap,
                                  QMap<QString, int> lastIdsMap);

    int getFirstIdForText(QString text);
    int getLastIdForText(QString text);
};

#endif // PARALLELCHAPTERREPRESENTATION_H
