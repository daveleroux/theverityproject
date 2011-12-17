#include "headingsrule.h"
#include "globalvariables.h"

HeadingsRule::HeadingsRule(DbUpdater* dbUpdater, QString sourceText) : Rule(dbUpdater)
{
    this->sourceText = sourceText;
}

bool HeadingsRule::applies(int id)
{
    SyncNumberLookup* syncNumberLookup = dbUpdater->syncNumberMaps.value(sourceText);
    return syncNumberLookup->isBookHeading(id);
}

int HeadingsRule::getSyncNumber(int id)
{
    SyncNumberLookup* syncNumberLookup = dbUpdater->syncNumberMaps.value(sourceText);
    int bookNumber = syncNumberLookup->getBookNumber(id);

    SyncNumberLookup* netSyncNumberLookup = dbUpdater->syncNumberMaps.value(NET);
    return netSyncNumberLookup->getParallelForBookNumber(bookNumber);
}

