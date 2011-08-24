#include "parallelchapterrepresentation.h"

ParallelChapterRepresentation::ParallelChapterRepresentation(int normalisedChapter,
                                                             QString html,
                                                             QMap<int, int> firstIdsMap,
                                                             QMap<int, int> lastIdsMap,
                                                             bool evenNumberOfRows) : ChapterRepresentation(normalisedChapter,
                                                                                                                html)
{
    evenStart = false;
    this->firstIdsMap = firstIdsMap;
    this->lastIdsMap = lastIdsMap;
    this->evenNumberOfRows = evenNumberOfRows;
}

QMap<int, int> ParallelChapterRepresentation::getIdsToIncludeIfPrepending()
{
    QMap<int, int> result = firstIdsMap;
    foreach (int bibletextId, result.keys())
    {
        result.insert(bibletextId, result.value(bibletextId)-1);
    }
    return result;
}

QMap<int, int> ParallelChapterRepresentation::getIdsToIncludeIfAppending()
{
    QMap<int, int> result = lastIdsMap;
    foreach (int bibletextId, result.keys())
    {
        result.insert(bibletextId, result.value(bibletextId)+1);
    }
    return result;
}

bool ParallelChapterRepresentation::isEvenStart()
{
    return evenStart;
}

bool ParallelChapterRepresentation::isEvenNumberOfRows()
{
    return evenNumberOfRows;
}

void ParallelChapterRepresentation::setEvenStart(bool evenStart)
{
    this->evenStart = evenStart;
}
