#include "superscriptionwlcrule.h"
#include "globalvariables.h"

SuperscriptionWlcRule::SuperscriptionWlcRule(DbUpdater* dbUpdater, VerseReference verseReference) : Rule(dbUpdater)
{
    this->verseReference = verseReference;
}

bool SuperscriptionWlcRule::applies(int id)
{
    SyncNumberLookup* syncNumberLookup = dbUpdater->syncNumberMaps.value(WLC);
    if(syncNumberLookup->hasVerseReferenceForId(id))
    {
        VerseReference currentVerseReference = syncNumberLookup->verseReferenceForId(id);
        return verseReference == currentVerseReference;
    }
    return false;
}

int SuperscriptionWlcRule::getSyncNumber(int id)
{
    SyncNumberLookup* syncNumberLookup = dbUpdater->syncNumberMaps.value(NET);
    return syncNumberLookup->getPreviousParallel(verseReference);
}

