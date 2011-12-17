#include "offsetrule.h"

OffsetRule::OffsetRule(DbUpdater* dbUpdater,
                       QString sourceText,
                       VerseReference sourceStartReference,
                       VerseReference sourceEndReference,
                       QString targetText,
                       VerseReference targetStartReference) : Rule(dbUpdater)
{
    this->sourceText = sourceText;
    this->sourceStartReference = sourceStartReference;
    this->sourceEndReference = sourceEndReference;

    this->targetText = targetText;
    this->targetStartReference = targetStartReference;
}

bool OffsetRule::applies(int id)
{
    SyncNumberLookup* syncNumberLookup = dbUpdater->syncNumberMaps.value(sourceText);
    if(syncNumberLookup->hasVerseReferenceForId(id))
    {
        VerseReference verseReference = syncNumberLookup->verseReferenceForId(id);
        return verseReference >= sourceStartReference && verseReference <= sourceEndReference;
    }
    return false;
}

int OffsetRule::getSyncNumber(int id)
{
    SyncNumberLookup* sourceSyncNumberLookup = dbUpdater->syncNumberMaps.value(sourceText);

    VerseReference sourceCurrentVerseReference = sourceSyncNumberLookup->verseReferenceForId(id);

    int offset = sourceSyncNumberLookup->distanceBetween(sourceStartReference, sourceCurrentVerseReference);

    SyncNumberLookup* targetSyncNumberLookup = dbUpdater->syncNumberMaps.value(targetText);

    VerseReference targetVerseReferenceToUse = targetSyncNumberLookup->addOffset(offset, targetStartReference);

    return targetSyncNumberLookup->getParallel(targetVerseReferenceToUse);
}

