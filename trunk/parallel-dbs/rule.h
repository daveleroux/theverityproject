#ifndef RULE_H
#define RULE_H

#include "dbupdater.h"
#include "versereference.h"

class Rule
{
public:
    Rule(DbUpdater* dbUpdater);
    virtual int getSyncNumber(VerseReference verseReference) = 0;
    virtual bool applies(VerseReference verseReference) = 0;

    DbUpdater* dbUpdater;

    int getHighestUnusedSyncNumber();
private:
};

#endif // RULE_H
