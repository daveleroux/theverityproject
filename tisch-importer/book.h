#ifndef BOOK_H
#define BOOK_H

#include <QMap>
#include "chapter.h"

class Book
{
public:
    QString name;
    QMap<int, Chapter*> chapters;
    Book(QString name);
};

#endif // BOOK_H
