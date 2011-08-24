#ifndef PARALLELGRIDCONSTRUCTOR_H
#define PARALLELGRIDCONSTRUCTOR_H

#include "textinfo.h"
#include "versenode.h"
#include "grid.h"

class ParallelGridConstructor
{
public:
//    static VerseNode* constructChain(QList<TextInfo> textInfos);
    static Grid constructGrid(QList<VerseNode*> chainHeads);
};

#endif // PARALLELGRIDCONSTRUCTOR_H
