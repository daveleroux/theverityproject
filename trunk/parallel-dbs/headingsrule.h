#ifndef HEADINGSRULE_H
#define HEADINGSRULE_H

#include "rule.h"
#include <QMap>

class HeadingsRule : public Rule
{
private:
    QMap<int, QString> bookNameForNumber;
    QString sourceText;
public:
    HeadingsRule(DbUpdater* dbUpdater, QString sourceText);
    virtual int getSyncNumber(int id);
    virtual bool applies(int id);
};

#endif // HEADINGSRULE_H
