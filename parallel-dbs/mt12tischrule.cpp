#include "mt12tischrule.h"
#include "globalvariables.h"


Mt12TischRule::Mt12TischRule(DbUpdater* dbUpdater) : Rule(dbUpdater)
{
}

bool Mt12TischRule::applies(VerseReference verseReference)
{
    return verseReference == VerseReference(MATTHEW, 12, 47);
}

int Mt12TischRule::getSyncNumber(VerseReference tischVerseReference)
{
    QMap<VerseReference, int>* kjvMap = dbUpdater->syncNumberMaps.value(KJV);
    return kjvMap->value(tischVerseReference);
}


