#ifndef STANDARDRULE_H
#define STANDARDRULE_H

#include "dbupdater.h"
#include "rule.h"

class StandardRule : public Rule
{
public:
    StandardRule(DbUpdater* dbUpdater);
    virtual int getSyncNumber(VerseReference verseReference);
    virtual bool applies(VerseReference verseReference);

};

#endif // STANDARDRULE_H
