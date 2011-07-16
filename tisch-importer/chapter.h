#ifndef CHAPTER_H
#define CHAPTER_H

#include <QMap>
#include <QList>
#include "verse.h"

class Chapter
{
public:
    QMap<int, QList<Verse*>* > verses;
    Chapter();
};

#endif // CHAPTER_H
