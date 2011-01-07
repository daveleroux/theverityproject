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

    int targetStartIndex;
public:
    OffsetRule(DbUpdater* dbUpdater,
               QString sourceText,
               VerseReference sourceStartReference,
               VerseReference sourceEndReference,
               QString targetText,
               VerseReference targetStartReference);

    bool applies(VerseReference verseReference);
    int getSyncNumber(VerseReference sourceCurrentVerseReference);

};

#endif // OFFSETRULE_H
