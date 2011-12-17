#include "standardrule.h"
#include "globalvariables.h"
#include <QDebug>

StandardRule::StandardRule(DbUpdater* dbUpdater, QString sourceText) : Rule(dbUpdater)
{
    this->sourceText = sourceText;
}

bool StandardRule::applies(int id)
{
    return true;
}

int StandardRule::getSyncNumber(int id)
{
    SyncNumberLookup* sourceSyncNumberLookup = dbUpdater->syncNumberMaps.value(sourceText);

    if(sourceSyncNumberLookup->hasVerseReferenceForId(id))
    {
        VerseReference verseReference = sourceSyncNumberLookup->verseReferenceForId(id);

        SyncNumberLookup* netSyncNumberLookup = dbUpdater->syncNumberMaps.value(NET);
        if(netSyncNumberLookup->hasVerseReference(verseReference))
        {
            return netSyncNumberLookup->getParallel(verseReference);
        }

    }
    return Rule::getHighestUnusedSyncNumber();
}
