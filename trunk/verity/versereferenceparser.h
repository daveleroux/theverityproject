#ifndef VERSEREFERENCEPARSER_H
#define VERSEREFERENCEPARSER_H

// a NON-thread-safe singleton


#include <QHash>
#include "versereference.h"

class VerseReferenceParser
{
private:
    QHash<QString, int> hash;
    QHash<int, QString> normalisedBookNames;

    VerseReferenceParser();

    static VerseReferenceParser& instance();

    VerseReferenceParser(const VerseReferenceParser&);                 // Prevent copy-construction
    VerseReferenceParser& operator=(const VerseReferenceParser&);      // Prevent assignment

    QString calculateStringRepresentation(int book, int chapter, int verse);

    VerseReference _parse(QString string);

public:
    static VerseReference parse(QString string);
};

#endif // VERSEREFERENCEPARSER_H
