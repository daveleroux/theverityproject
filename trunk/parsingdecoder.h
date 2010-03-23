#ifndef PARSINGDECODER_H
#define PARSINGDECODER_H

// a NON-thread-safe singleton

#include <QList>
#include <QBitArray>
#include "parseattribute.h"
#include "morphattribute.h"

class ParsingDecoder
{

private:

    MorphAttribute nextMorphAttribute(QString type, QString value);
    QList<MorphAttribute> morphAttributes;
    QBitArray zero;

    ParsingDecoder();

    static ParsingDecoder& instance();

    ParsingDecoder(const ParsingDecoder&);                 // Prevent copy-construction
    ParsingDecoder& operator=(const ParsingDecoder&);      // Prevent assignment

    QList<ParseAttribute> _parse(QBitArray bits);

public:
    static QList<ParseAttribute> parse(QBitArray bits);

};

#endif // PARSINGDECODER_H
