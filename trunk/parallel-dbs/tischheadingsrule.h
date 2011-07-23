#ifndef TISCHHEADINGSRULE_H
#define TISCHHEADINGSRULE_H

#include "rule.h"
#include <QMap>


class TischHeadingsRule : public Rule
{
private:
    QMap<int, QString> bookNameForNumber;
public:
    TischHeadingsRule(DbUpdater* dbUpdater);
    virtual int getSyncNumber(VerseReference verseReference);
    virtual bool applies(VerseReference verseReference);
};

#endif // TISCHHEADINGSRULE_H
