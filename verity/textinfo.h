#ifndef TEXTINFO_H
#define TEXTINFO_H

#include <QString>
#include <QBitArray>

class TextInfo
{
public:
    TextInfo();
    TextInfo(int id,
             QString book,
             int chapter,
             int verse,
             int numberInVerse,
             QString text,
             QString morphologicalTag,
             QBitArray normalisedMorphTag,
             int strongsNumber,
             QString strongsLemma,
             QString fribergLemma);

        int id;
         QString book;
         int chapter;
         int verse;
         int numberInVerse;
         QString text;
         QString morphologicalTag;
         QBitArray normalisedMorphTag;
         int strongsNumber;
         QString strongsLemma;
         QString fribergLemma;

};

#endif // TEXTINFO_H
