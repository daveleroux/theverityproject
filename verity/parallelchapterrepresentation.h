#ifndef PARALLELCHAPTERREPRESENTATION_H
#define PARALLELCHAPTERREPRESENTATION_H

#include "chapterrepresentation.h"
#include <QMap>

class ParallelChapterRepresentation : public ChapterRepresentation
{
protected:
    QMap<int, int> firstIdsMap;
    QMap<int, int> lastIdsMap;

public:
    ParallelChapterRepresentation(int normalisedChapter,
                                  QString html,
                                  QMap<int, int> firstIdsMap,
                                  QMap<int, int> lastIdsMap);

    QMap<int, int> getIdsToIncludeIfPrepending();
    QMap<int, int> getIdsToIncludeIfAppending();
};

#endif // PARALLELCHAPTERREPRESENTATION_H
