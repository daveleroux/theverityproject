#include "gen32wlcrule.h"
#include "globalvariables.h"

Gen32WlcRule::Gen32WlcRule(DbUpdater* dbUpdater) : Rule(dbUpdater)
{
    startReference = new VerseReference(GENESIS, 32, 1);
    endReference = new VerseReference(GENESIS, 32, 33);
}

bool Gen32WlcRule::applies(VerseReference verseReference)
{
    return verseReference >= *startReference && verseReference <= *endReference;
}

int Gen32WlcRule::getSyncNumber(VerseReference wlcVerseReference)
{
    VerseReference esvVerseReferenceToUse;
    QMap<VerseReference, int> esvMap = dbUpdater->syncNumberMaps.value(ESV);

    if(wlcVerseReference == *endReference)
    {
        esvVerseReferenceToUse = VerseReference(GENESIS, 32, 32);
    }
    else
    {
        QList<VerseReference> esvKeys = esvMap.keys();
        int index = esvKeys.indexOf(wlcVerseReference);
        esvVerseReferenceToUse = esvKeys.at(index-1);
    }

    return esvMap.value(esvVerseReferenceToUse);
}

Gen32WlcRule::~Gen32WlcRule()
{
    delete startReference;
    delete endReference;
}
