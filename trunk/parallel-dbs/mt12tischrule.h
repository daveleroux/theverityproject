#ifndef MT12TISCHRULE_H
#define MT12TISCHRULE_H

#include "rule.h"

//THIS IS NOW UNUSED BECAUSE THE NET HAS A MT 12:47

class Mt12TischRule : public Rule
{
public:
    Mt12TischRule(DbUpdater* dbUpdater);
    virtual int getSyncNumber(VerseReference verseReference);
    virtual bool applies(VerseReference verseReference);
};

#endif // MT12TISCHRULE_H
