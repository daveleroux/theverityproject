#include "jn1tischrule.h"
#include "globalvariables.h"


Jn1TischRule::Jn1TischRule(DbUpdater* dbUpdater) : Rule(dbUpdater)
{
    startReference = new VerseReference(JOHN, 1, 39);
    endReference = new VerseReference(JOHN, 1, 52);
}

bool Jn1TischRule::applies(VerseReference verseReference)
{
    return verseReference >= *startReference && verseReference <= *endReference;
}

int Jn1TischRule::getSyncNumber(VerseReference tischVerseReference)
{
    //maybe should rather use logic like: tischVerseReference--

    VerseReference netVerseReferenceToUse;
    QMap<VerseReference, int>* netMap = dbUpdater->syncNumberMaps.value(NET);

    if(tischVerseReference == *startReference)
    {
        return Rule::getHighestUnusedSyncNumber();
    }
    else if (tischVerseReference == *endReference)
    {
        netVerseReferenceToUse = VerseReference(JOHN, 1, 51);
    }
    else
    {
        QList<VerseReference> netKeys = netMap->keys();
        int index = netKeys.indexOf(tischVerseReference);
        netVerseReferenceToUse = netKeys.at(index - 1);
    }

    return netMap->value(netVerseReferenceToUse);
}

Jn1TischRule::~Jn1TischRule()
{
    delete startReference;
    delete endReference;
}
