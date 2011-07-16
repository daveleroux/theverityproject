#include "word.h"

Word::Word(QString text, QBitArray normalisedMorphTag, int strongsNumber, QString strongsLemma, QString fribergLemma)
{
    isEndofParagraph = false;

    this->text = text;
    this->normalisedMorphTag = normalisedMorphTag;
    this->strongsNumber = strongsNumber;
    this->strongsLemma = strongsLemma;
    this->fribergLemma = fribergLemma;
}
