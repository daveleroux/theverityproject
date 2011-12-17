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

//int Rule::getIndexOf(QString text, VerseReference verseReference)
//{
//    QMap<VerseReference, int>* map = dbUpdater->syncNumberMaps.value(text);
//    QList<VerseReference> keys = map->keys();
//    return keys.indexOf(verseReference);
//}
