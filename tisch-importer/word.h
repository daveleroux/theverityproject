#ifndef WORD_H
#define WORD_H

#include <QBitArray>
#include <QString>

class Word
{
public:
    bool isEndofParagraph;

    QString text;
    QByteArray* normalisedMorphTag;
    int strongsNumber;
    QString strongsLemma;
    QString fribergLemma;

    Word(QString text, QByteArray* normalisedMorphTag, int strongsNumber, QString strongsLemma, QString fribergLemma);
};

#endif // WORD_H
