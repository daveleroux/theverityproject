#ifndef JN1TISCHRULE_H
#define JN1TISCHRULE_H

#include "rule.h"

class Jn1TischRule : public Rule
{
private:
    VerseReference* startReference;
    VerseReference* endReference;

public:
    Jn1TischRule(DbUpdater* dbUpdater);
    virtual int getSyncNumber(VerseReference verseReference);
    virtual bool applies(VerseReference verseReference);
    ~Jn1TischRule();
};

#endif // JN1TISCHRULE_H
