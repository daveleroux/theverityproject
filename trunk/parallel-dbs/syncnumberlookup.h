#ifndef SYNCNUMBERLOOKUP_H
#define SYNCNUMBERLOOKUP_H

#include <QMap>
#include "versereference.h"

class SyncNumberLookup
{
private:
    QMap<VerseReference, int>* verseReferenceToIdMap;
    QMap<int, int>* idToParallelMap;

    QMap<int, int>* originalIdToParallelMap;

    QHash<int, VerseReference>* idToVerseReferenceHash;

    QMap<int, int>* bookNumberToIdMap;
    QHash<int, int>* idToBookNumberHash;

public:
    SyncNumberLookup();
    void insert(VerseReference verseReference, int id);
    void insertParallel(int id, int parallel);
    void insertOriginal(int id, int parallel);

    QList<int> getAllIds();

    bool hasVerseReferenceForId(int id);
    VerseReference verseReferenceForId(int id);

    int distanceBetween(VerseReference start, VerseReference end);
    VerseReference addOffset(int offset, VerseReference start);

    bool hasVerseReference(VerseReference verseReference);
    int getParallel(VerseReference verseReference);
    int getPreviousParallel(VerseReference verseReference);

    void insertBookNumber(int bookNumber, int id);

    bool isBookHeading(int id);

    int getBookNumber(int id);
    int getParallelForBookNumber(int bookNumber);

    bool mustUpdate(int id, int parallel);


};

#endif // SYNCNUMBERLOOKUP_H
