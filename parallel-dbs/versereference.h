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

    QString textIfLucky;

    VerseReference();

    VerseReference(int book, int chapter, int verse);

    VerseReference(int book, int chapter, int verse, QString textIfLucky);

    bool operator<(const VerseReference& rhs) const;    
    bool operator<=(const VerseReference& rhs) const;
    bool operator>(const VerseReference& rhs) const;
    bool operator>=(const VerseReference& rhs) const;
    bool operator==(const VerseReference& rhs) const;
    bool operator!=(const VerseReference& rhs) const;

    QString toString();

};

inline uint qHash(const VerseReference &key)
{
    return key.book * 10000 + key.chapter * 100 + key.verse;
}


#endif // VERSEREFERENCE_H
