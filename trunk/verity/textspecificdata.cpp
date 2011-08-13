#include "textspecificdata.h"

#include <QString>

TextSpecificData::TextSpecificData(int bibletextId, int minChapter, int maxChapter, QHash<int, MinAndMaxIds> hash, bool rtl)
{
    this->bibletextId = bibletextId;
    this->minChapter = minChapter;
    this->maxChapter = maxChapter;
    this->hash = hash;
    this->rtl = rtl;
}
