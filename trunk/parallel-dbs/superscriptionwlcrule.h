#ifndef SUPERSCRIPTIONWLCRULE_H
#define SUPERSCRIPTIONWLCRULE_H

#include "rule.h"

class SuperscriptionWlcRule : public Rule
{
private:
    VerseReference verseReference;

public:
    SuperscriptionWlcRule(DbUpdater* dbUpdater, VerseReference verseReference);
    virtual int getSyncNumber(int id);
    virtual bool applies(int id);
};

#endif // SUPERSCRIPTIONWLCRULE_H
