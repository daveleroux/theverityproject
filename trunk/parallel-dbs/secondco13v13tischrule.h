#ifndef SECONDCO13V13TISCHRULE_H
#define SECONDCO13V13TISCHRULE_H

#include "rule.h"

class SecondCo13v13TischRule : public Rule
{
private:
    VerseReference* secondCo13v13Reference;
    VerseReference* secondCo13v14Reference;

public:
    SecondCo13v13TischRule(DbUpdater* dbUpdater);
    virtual int getSyncNumber(VerseReference verseReference);
    virtual bool applies(VerseReference verseReference);
    ~SecondCo13v13TischRule();
};

#endif // SECONDCO13V13TISCHRULE_H
