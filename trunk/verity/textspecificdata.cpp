#include "textspecificdata.h"

#include <QString>

TextSpecificData::TextSpecificData(QString text, int minChapter, int maxChapter, QHash<int, MinAndMaxIds> hash)
{
    this->text = text;
    this->minChapter = minChapter;
    this->maxChapter = maxChapter;
    this->hash = hash;
}
