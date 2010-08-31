#ifndef TEXTSPECIFICDATA_H
#define TEXTSPECIFICDATA_H

#include <QString>

class TextSpecificData
{
public:
    QString text;
    int minChapter;
    int maxChapter;
    TextSpecificData(QString text, int minChapter, int maxChapter);
};

#endif // TEXTSPECIFICDATA_H
