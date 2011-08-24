#ifndef PARALLELCHAPTERREPRESENTATION_H
#define PARALLELCHAPTERREPRESENTATION_H

#include "chapterrepresentation.h"
#include <QMap>

class ParallelChapterRepresentation : public ChapterRepresentation
{
protected:
    QMap<int, int> firstIdsMap;
    QMap<int, int> lastIdsMap;
    bool evenStart;
    bool evenNumberOfRows;

public:
    ParallelChapterRepresentation(int normalisedChapter,
                                  QString html,
                                  QMap<int, int> firstIdsMap,
                                  QMap<int, int> lastIdsMap,
                                  bool evenNumberOfRows);

    QMap<int, int> getIdsToIncludeIfPrepending();
    QMap<int, int> getIdsToIncludeIfAppending();

    bool isEvenStart();
    bool isEvenNumberOfRows();

    void setEvenStart(bool evenStart);
};

#endif // PARALLELCHAPTERREPRESENTATION_H
