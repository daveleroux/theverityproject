#ifndef GEN32WLCRULE_H
#define GEN32WLCRULE_H

#include "rule.h"

class Gen32WlcRule : public Rule
{
private:
    VerseReference* startReference;
    VerseReference* endReference;

public:
    Gen32WlcRule(DbUpdater* dbUpdater);
    virtual int getSyncNumber(VerseReference verseReference);
    virtual bool applies(VerseReference verseReference);
    ~Gen32WlcRule();
};

#endif // GEN32WLCRULE_H
