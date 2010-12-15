#ifndef TEXTINFO_H
#define TEXTINFO_H

#include <QString>
#include <QBitArray>

class TextInfo
{
public:
    QString bibleText;
    int id;
    QString book;
    int chapter;
    int verse;
    int numberInVerse;
    QString text;
    bool paragraph;
    QString morphologicalTag;
    QBitArray normalisedMorphTag;
    int strongsNumber;
    QString strongsLemma;
    QString fribergLemma;
    int parallel;

    TextInfo();
    TextInfo(QString bibleText,
             int id,
             QString book,
             int chapter,
             int verse,
             int numberInVerse,
             bool paragraph,
             QString text,
             QString morphologicalTag,
             QBitArray normalisedMorphTag,
             int strongsNumber,
             QString strongsLemma,
             QString fribergLemma,
             int parallel);


};

#endif // TEXTINFO_H
