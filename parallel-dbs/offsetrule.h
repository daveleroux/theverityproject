#ifndef OFFSETRULE_H
#define OFFSETRULE_H

#include "rule.h"

class OffsetRule : public Rule
{
private:
    QString sourceText;
    VerseReference sourceStartReference;
    VerseReference sourceEndReference;
    QString targetText;
    VerseReference targetStartReference;

public:
    OffsetRule(DbUpdater* dbUpdater,
               QString sourceText,
               VerseReference sourceStartReference,
               VerseReference sourceEndReference,
               QString targetText,
               VerseReference targetStartReference);

    bool applies(int id);
    int getSyncNumber(int id);

};

#endif // OFFSETRULE_H
