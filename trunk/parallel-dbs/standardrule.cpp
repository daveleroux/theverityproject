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
    QMap<VerseReference, int>* map = dbUpdater->syncNumberMaps.value(NET);

//    if(verseReference.book == 40 && verseReference.chapter == 1 && verseReference.verse == 1)
//    {
//        qDebug() << "ok, in standard rule looking for mt 1:1";
//        for(int i=0; i<map->keys().size(); i++)
//        {
//            VerseReference key = map->keys().at(i);
//            if(key.book == 40 && key.chapter == 1 && key.verse == 1)
//            {
//                bool b = (verseReference == key);
//                qDebug() << "found same thing, are equal: " << b;
//            }
//        }
//    }
    if(map->contains(verseReference))
    {
        return map->value(verseReference);
    }
    else
    {
        qDebug() << "could not find reference " << verseReference.toString() << " in standard rule";
        return Rule::getHighestUnusedSyncNumber();
    }
}
