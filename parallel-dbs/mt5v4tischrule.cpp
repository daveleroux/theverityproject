#include "mt5v4tischrule.h"
#include "globalvariables.h"


Mt5v4TischRule::Mt5v4TischRule(DbUpdater* dbUpdater) : Rule(dbUpdater)
{
    mt5v4Reference = new VerseReference(MATTHEW, 5, 4);
    mt5v5Reference = new VerseReference(MATTHEW, 5, 5);
}

bool Mt5v4TischRule::applies(VerseReference verseReference)
{
    return verseReference == *mt5v4Reference || verseReference == *mt5v5Reference;
}

int Mt5v4TischRule::getSyncNumber(VerseReference tischVerseReference)
{
    VerseReference netVerseReferenceToUse;
    QMap<VerseReference, int>* netMap = dbUpdater->syncNumberMaps.value(NET);

    if(tischVerseReference == *mt5v4Reference)
    {
        netVerseReferenceToUse = *mt5v5Reference;
    }
    else if (tischVerseReference == *mt5v5Reference)
    {
        netVerseReferenceToUse = *mt5v4Reference;
    }

    return netMap->value(netVerseReferenceToUse);
}

Mt5v4TischRule::~Mt5v4TischRule()
{
    delete mt5v4Reference;
    delete mt5v5Reference;
}
