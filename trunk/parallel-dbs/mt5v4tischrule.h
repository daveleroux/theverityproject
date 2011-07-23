#ifndef MT5V4TISCHRULE_H
#define MT5V4TISCHRULE_H

#include "rule.h"

class Mt5v4TischRule : public Rule
{
private:
    VerseReference* mt5v4Reference;
    VerseReference* mt5v5Reference;

public:
    Mt5v4TischRule(DbUpdater* dbUpdater);
    virtual int getSyncNumber(VerseReference verseReference);
    virtual bool applies(VerseReference verseReference);
    ~Mt5v4TischRule();
};

#endif // MT5V4TISCHRULE_H
