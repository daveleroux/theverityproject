#ifndef PARSINGWORDHANDLER_H
#define PARSINGWORDHANDLER_H
#include "wordhandler.h"

class ParsingWordHandler : public WordHandler
{
public:
    ParsingWordHandler();
    void handle(int bibletextId, int wordId);
};

#endif // PARSINGWORDHANDLER_H
