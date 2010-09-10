#include "rule.h"

Rule::Rule(DbUpdater* dbUpdater)
{
    this->dbUpdater = dbUpdater;
}

int Rule::getHighestUnusedSyncNumber()
{
    int result = dbUpdater->highestUnusedSyncNumber;
    dbUpdater->highestUnusedSyncNumber++;
    return result;
}
