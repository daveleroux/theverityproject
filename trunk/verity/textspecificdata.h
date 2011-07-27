#ifndef TEXTSPECIFICDATA_H
#define TEXTSPECIFICDATA_H

#include <QString>
#include "minandmaxidsforchapter.h"
#include <QHash>

class TextSpecificData
{
public:
    int bibletextId;
    int minChapter;
    int maxChapter;
    QHash<int, MinAndMaxIds> hash;
    TextSpecificData(int bibletextId, int minChapter, int maxChapter, QHash<int, MinAndMaxIds> hash);
};

#endif // TEXTSPECIFICDATA_H
