#ifndef MATCHTONOTHINGRULE_H
#define MATCHTONOTHINGRULE_H

#include "rule.h"

class MatchToNothingRule : public Rule
{
private:
    QString sourceText;
    VerseReference verseReference;

public:
    MatchToNothingRule(DbUpdater* dbUpdater, QString sourceText, VerseReference verseReference);
    virtual int getSyncNumber(int id);
    virtual bool applies(int id);
};

#endif // MATCHTONOTHINGRULE_H
