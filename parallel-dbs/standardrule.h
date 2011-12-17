#ifndef STANDARDRULE_H
#define STANDARDRULE_H

#include "rule.h"

class StandardRule : public Rule
{
private:
    QString sourceText;

public:
    StandardRule(DbUpdater* dbUpdater, QString sourceText);
    virtual int getSyncNumber(int id);
    virtual bool applies(int id);

};

#endif // STANDARDRULE_H
