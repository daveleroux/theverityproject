#ifndef VERSEREFERENCE_H
#define VERSEREFERENCE_H

#include <QString>

struct VerseReference
{
    QString text;
    int book;
    int chapter;
    int verse;
    QString stringRepresentation;

    VerseReference(QString text, int book, int chapter, int verse, QString stringRepresentation);
};

#endif // VERSEREFERENCE_H
