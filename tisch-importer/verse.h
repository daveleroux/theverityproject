#ifndef VERSE_H
#define VERSE_H

#include <QList>
#include "word.h"

class Verse
{
public:
    QList<Word*> words;
    Verse();
};

#endif // VERSE_H
