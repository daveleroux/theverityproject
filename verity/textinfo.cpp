#include "textinfo.h"

TextInfo::TextInfo()
{
}

TextInfo::TextInfo(int id,
                   QString book,
                   int chapter,
                   int verse,
                   int numberInVerse,
                   QString text,
                   QString morphologicalTag,
                   QBitArray normalisedMorphTag,
                   int strongsNumber,
                   QString strongsLemma,
                   QString fribergLemma)
{
    this->id = id;
    this->book = book;
    this->chapter = chapter;
    this->verse = verse;
    this->numberInVerse = numberInVerse;
    this->text = text;
    this->morphologicalTag = morphologicalTag;
    this->normalisedMorphTag = normalisedMorphTag;
    this->strongsNumber = strongsNumber;
    this->strongsLemma = strongsLemma;
    this->fribergLemma = fribergLemma;
}
