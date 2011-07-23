#ifndef WLCHEADINGSRULE_H
#define WLCHEADINGSRULE_H

#include "rule.h"
#include <QMap>

class WlcHeadingsRule : public Rule
{
private:
    QMap<int, QString> bookNameForNumber;
public:
    WlcHeadingsRule(DbUpdater* dbUpdater);
    virtual int getSyncNumber(VerseReference verseReference);
    virtual bool applies(VerseReference verseReference);
};

#endif // WLCHEADINGSRULE_H
