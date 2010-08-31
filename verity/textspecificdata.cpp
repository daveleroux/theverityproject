#include "textspecificdata.h"

#include <QString>

TextSpecificData::TextSpecificData(QString text, int minChapter, int maxChapter)
{
    this->text = text;
    this->minChapter = minChapter;
    this->maxChapter = maxChapter;
}
