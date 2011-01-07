#include "matchtonothingrule.h"


MatchToNothingRule::MatchToNothingRule(DbUpdater* dbUpdater, VerseReference verseReference) : Rule(dbUpdater)
{
    this->verseReference = verseReference;
}

bool MatchToNothingRule::applies(VerseReference currentVerseReference)
{
    return verseReference == currentVerseReference;
}

int MatchToNothingRule::getSyncNumber(VerseReference verseReference)
{
    return Rule::getHighestUnusedSyncNumber();
}

