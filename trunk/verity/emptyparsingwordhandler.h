#ifndef EMPTYPARSINGWORDHANDLER_H
#define EMPTYPARSINGWORDHANDLER_H

#include "wordhandler.h"

class EmptyParsingWordHandler : public WordHandler
{
public:
    EmptyParsingWordHandler();
    void handle(int bibletextId, int wordId);
};

#endif // EMPTYPARSINGWORDHANDLER_H
