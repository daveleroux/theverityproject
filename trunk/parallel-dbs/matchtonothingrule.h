#ifndef MATCHTONOTHINGRULE_H
#define MATCHTONOTHINGRULE_H

#include "rule.h"

class MatchToNothingRule : public Rule
{
private:
    VerseReference verseReference;

public:
    MatchToNothingRule(DbUpdater* dbUpdater, VerseReference verseReference);
    virtual int getSyncNumber(VerseReference currentVerseReference);
    virtual bool applies(VerseReference verseReference);
};

#endif // MATCHTONOTHINGRULE_H
