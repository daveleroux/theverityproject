#include "parallelchapterrepresentation.h"

ParallelChapterRepresentation::ParallelChapterRepresentation(QString text,
                                                             int normalisedChapter,
                                                             QTextDocumentFragment textDocumentFragment,
                                                             QMap<BaseTextUnit, TextInfo> textUnits,
                                                             int selectionStart,
                                                             int selectionEnd,
                                                             QMap<QString, int> firstIdsMap,
                                                             QMap<QString, int> lastIdsMap) : ChapterRepresentation(text,
                                                                                                       normalisedChapter,
                                                                                                       textDocumentFragment,
                                                                                                       textUnits,
                                                                                                       selectionStart,
                                                                                                       selectionEnd)
{
    this->firstIdsMap = firstIdsMap;
    this->lastIdsMap = lastIdsMap;
}

int ParallelChapterRepresentation::getFirstIdForText(QString text)
{
    return firstIdsMap.value(text);
}

int ParallelChapterRepresentation::getLastIdForText(QString text)
{
    return lastIdsMap.value(text);
}

