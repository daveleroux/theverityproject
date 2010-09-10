#include "standardrule.h"
#include "globalvariables.h"
#include <QDebug>

StandardRule::StandardRule(DbUpdater* dbUpdater) : Rule(dbUpdater)
{
}

bool StandardRule::applies(VerseReference verseReference)
{
    return true;
}

int StandardRule::getSyncNumber(VerseReference verseReference)
{
    QMap<VerseReference, int> map = dbUpdater->syncNumberMaps.value(ESV);
    if(map.contains(verseReference))
    {
        return map.value(verseReference);
    }
    else
    {
        qDebug() << "could not find reference " << verseReference.toString() << " in standard rule";
        return Rule::getHighestUnusedSyncNumber();
    }
}
