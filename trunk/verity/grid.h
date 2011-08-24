#ifndef GRID_H
#define GRID_H

#include "versenode.h"

struct Grid
{
    VerseNode* verseNode;
    bool evenNumberOfRows;
    Grid(VerseNode* verseNode, bool evenNumberOfRows);
};

#endif // GRID_H
