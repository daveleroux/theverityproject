#include "syncnumberlookup.h"

SyncNumberLookup::SyncNumberLookup()
{
    verseReferenceToIdMap = new QMap<VerseReference, int>();
    idToParallelMap = new QMap<int, int>();

    originalIdToParallelMap = new QMap<int, int>();

    idToVerseReferenceHash = new QHash<int, VerseReference>();

    bookNumberToIdMap = new QMap<int, int>();
    idToBookNumberHash = new QHash<int, int>();
}

void SyncNumberLookup::insert(VerseReference verseReference, int id)
{
    verseReferenceToIdMap->insert(verseReference, id);
    idToVerseReferenceHash->insert(id, verseReference);
}

void SyncNumberLookup::insertParallel(int id, int parallel)
{
    idToParallelMap->insert(id, parallel);
}

void SyncNumberLookup::insertOriginal(int id, int parallel)
{
    originalIdToParallelMap->insert(id, parallel);
}

QList<int> SyncNumberLookup::getAllIds()
{
    return idToParallelMap->keys();
}


bool SyncNumberLookup::hasVerseReferenceForId(int id)
{
    return idToVerseReferenceHash->contains(id);
}

VerseReference SyncNumberLookup::verseReferenceForId(int id)
{
    return idToVerseReferenceHash->value(id);
}

int SyncNumberLookup::distanceBetween(VerseReference start, VerseReference end)
{
    return verseReferenceToIdMap->keys().indexOf(end) - verseReferenceToIdMap->keys().indexOf(start);
}

VerseReference SyncNumberLookup::addOffset(int offset, VerseReference start)
{
    int startIndex = verseReferenceToIdMap->keys().indexOf(start);
    return verseReferenceToIdMap->keys().value(startIndex + offset);
}

bool SyncNumberLookup::hasVerseReference(VerseReference verseReference)
{
    return verseReferenceToIdMap->contains(verseReference);
}

int SyncNumberLookup::getParallel(VerseReference verseReference)
{
    int id = verseReferenceToIdMap->value(verseReference);
    return idToParallelMap->value(id);
}

int SyncNumberLookup::getPreviousParallel(VerseReference verseReference)
{
    int id = verseReferenceToIdMap->value(verseReference) - 1;
    return idToParallelMap->value(id);
}

void SyncNumberLookup::insertBookNumber(int bookNumber, int id)
{
   bookNumberToIdMap->insert(bookNumber, id);
   idToBookNumberHash->insert(id, bookNumber);
}

bool SyncNumberLookup::isBookHeading(int id)
{
    return idToBookNumberHash->contains(id);
}

int SyncNumberLookup::getBookNumber(int id)
{
    return idToBookNumberHash->value(id);
}

int SyncNumberLookup::getParallelForBookNumber(int bookNumber)
{
    int id = bookNumberToIdMap->value(bookNumber);
    return idToParallelMap->value(id);
}

bool SyncNumberLookup::mustUpdate(int id, int parallel)
{
    return parallel != originalIdToParallelMap->value(id);
}
