#ifndef TEXTINFO_H
#define TEXTINFO_H

#include <QString>
#include <QBitArray>

class TextInfo
{
public:
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

    TextInfo();
    TextInfo(int id,
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
             QString fribergLemma);


};

#endif // TEXTINFO_H
