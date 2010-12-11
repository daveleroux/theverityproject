#include "versereference.h"
#include <QDebug>
#include <QTextStream>

VerseReference::VerseReference()
{
    book = 0;
    chapter = 0;
    verse = 0;
}

VerseReference::VerseReference(int book, int chapter, int verse)
{
    this->book = book;
    this->chapter = chapter;
    this->verse = verse;
}

QString VerseReference::toString()
{
    QString result;
    QTextStream stream(&result);
    stream << book << "." << chapter << "." << verse;
    return result;
}

bool VerseReference::operator<(const VerseReference& rhs) const
{
    QList<int> lhsList;
    lhsList.append(book);
    lhsList.append(chapter);
    lhsList.append(verse);

    QList<int> rhsList;
    rhsList.append(rhs.book);
    rhsList.append(rhs.chapter);
    rhsList.append(rhs.verse);

    for(int i=0; i<lhsList.size(); i++)
    {
        int lhsValue = lhsList.at(i);
        int rhsValue = rhsList.at(i);

        if(lhsValue < rhsValue)
            return true;
        else if(lhsValue > rhsValue)
            return false;
    }

    return false;
}

bool VerseReference::operator<=(const VerseReference& rhs) const
{
    return (*this<rhs) || (*this==rhs);
}

bool VerseReference::operator>(const VerseReference& rhs) const
{
    return !(*this<rhs) && !(*this==rhs);
}

bool VerseReference::operator>=(const VerseReference& rhs) const
{
    return (*this>rhs) || (*this==rhs);
}

bool VerseReference::operator==(const VerseReference& rhs) const
{
    return !(*this<rhs) && !(rhs<*this);
}

bool VerseReference::operator!=(const VerseReference& rhs) const
{
    return !(*this==rhs);
}
