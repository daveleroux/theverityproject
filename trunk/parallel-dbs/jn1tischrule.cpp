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

    VerseReference esvVerseReferenceToUse;
    QMap<VerseReference, int> esvMap = dbUpdater->syncNumberMaps.value(ESV);

    if(tischVerseReference == *startReference)
    {
        return Rule::getHighestUnusedSyncNumber();
    }
    else if (tischVerseReference == *endReference)
    {
        esvVerseReferenceToUse = VerseReference(JOHN, 1, 51);
    }
    else
    {
        QList<VerseReference> esvKeys = esvMap.keys();
        int index = esvKeys.indexOf(tischVerseReference);
        esvVerseReferenceToUse = esvKeys.at(index - 1);
    }

    return esvMap.value(esvVerseReferenceToUse);
}

Jn1TischRule::~Jn1TischRule()
{
    delete startReference;
    delete endReference;
}
