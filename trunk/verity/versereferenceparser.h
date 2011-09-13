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

    VerseReference _parse(QString string);
    QString _calculateStringRepresentation(int book, int chapter, int verse);
    QString _booknameFromBookindex(int book);

public:
    static VerseReference parse(QString string);
    static QString calculateStringRepresentation(int book, int chapter, int verse);
    static QString booknameFromBookindex(int book);
};

#endif // VERSEREFERENCEPARSER_H
