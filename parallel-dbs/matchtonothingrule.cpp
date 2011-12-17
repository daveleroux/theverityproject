#include "matchtonothingrule.h"


MatchToNothingRule::MatchToNothingRule(DbUpdater* dbUpdater, QString sourceText, VerseReference verseReference) : Rule(dbUpdater)
{
    this->sourceText = sourceText;
    this->verseReference = verseReference;
}

bool MatchToNothingRule::applies(int id)
{
    SyncNumberLookup* syncNumberLookup = dbUpdater->syncNumberMaps.value(sourceText);
    if(syncNumberLookup->hasVerseReferenceForId(id))
    {
        VerseReference currentVerseReference = syncNumberLookup->verseReferenceForId(id);
        return verseReference == currentVerseReference;
    }
    return false;
}

int MatchToNothingRule::getSyncNumber(int id)
{
    return Rule::getHighestUnusedSyncNumber();
}

