#ifndef PARALLELGRIDCONSTRUCTOR_H
#define PARALLELGRIDCONSTRUCTOR_H

#include "textinfo.h"
#include "versenode.h"

class ParallelGridConstructor
{
public:
//    static VerseNode* constructChain(QList<TextInfo> textInfos);
    static VerseNode* constructGrid(QList<VerseNode*> chainHeads);
};

#endif // PARALLELGRIDCONSTRUCTOR_H
