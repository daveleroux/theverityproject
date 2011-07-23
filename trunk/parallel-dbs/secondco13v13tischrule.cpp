#include "secondco13v13tischrule.h"
#include "globalvariables.h"


SecondCo13v13TischRule::SecondCo13v13TischRule(DbUpdater* dbUpdater) : Rule(dbUpdater)
{
    secondCo13v13Reference = new VerseReference(TWO_COR, 13, 13);
    secondCo13v14Reference = new VerseReference(TWO_COR, 13, 14);
}

bool SecondCo13v13TischRule::applies(VerseReference verseReference)
{
    return verseReference == *secondCo13v13Reference;
}

int SecondCo13v13TischRule::getSyncNumber(VerseReference tischVerseReference)
{
    QMap<VerseReference, int>* netMap = dbUpdater->syncNumberMaps.value(NET);
    return netMap->value(*secondCo13v14Reference);
}

SecondCo13v13TischRule::~SecondCo13v13TischRule()
{
    delete secondCo13v13Reference;
    delete secondCo13v14Reference;
}

