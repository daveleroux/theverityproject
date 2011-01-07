#include "offsetrule.h"

OffsetRule::OffsetRule(DbUpdater* dbUpdater,
                       QString sourceText,
                       VerseReference sourceStartReference,
                       VerseReference sourceEndReference,
                       QString targetText,
                       VerseReference targetStartReference) : Rule(dbUpdater)
{
    this->sourceText = sourceText;
    this->sourceStartReference = sourceStartReference;
    this->sourceEndReference = sourceEndReference;

    this->targetText = targetText;
    this->targetStartReference = targetStartReference;


    targetStartIndex = getIndexOf(targetText, targetStartReference);
}

bool OffsetRule::applies(VerseReference verseReference)
{
    return verseReference >= sourceStartReference && verseReference <= sourceEndReference;
}

int OffsetRule::getSyncNumber(VerseReference sourceCurrentVerseReference)
{
    int sourceStartIndex = getIndexOf(sourceText, sourceStartReference);
    int sourceCurrentIndex = getIndexOf(sourceText, sourceCurrentVerseReference);

    int offset = sourceCurrentIndex - sourceStartIndex;

    int targetIndexToUse = targetStartIndex + offset;

    QMap<VerseReference, int>* targetMap = dbUpdater->syncNumberMaps.value(targetText);
    QList<VerseReference> targetKeys = targetMap->keys();
    VerseReference targetVerseReferenceToUse = targetKeys.at(targetIndexToUse);

    return targetMap->value(targetVerseReferenceToUse);
}

