#ifndef VERSEREFERENCE_H
#define VERSEREFERENCE_H

#include <QString>
#include <QList>
#include <QDebug>

struct VerseReference
{
    int book;
    int chapter;
    int verse;

    VerseReference();
    VerseReference(int book, int chapter, int verse);
    bool operator<(const VerseReference& rhs) const;    
    bool operator<=(const VerseReference& rhs) const;
    bool operator>(const VerseReference& rhs) const;
    bool operator>=(const VerseReference& rhs) const;
    bool operator==(const VerseReference& rhs) const;
    bool operator!=(const VerseReference& rhs) const;

    QString toString();
};


#endif // VERSEREFERENCE_H
