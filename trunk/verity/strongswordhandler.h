#ifndef STRONGSWORDHANDLER_H
#define STRONGSWORDHANDLER_H

#include "wordhandler.h"

class StrongsWordHandler : public WordHandler
{
public:
    StrongsWordHandler();
    void handle(int bibletextId, int wordId);
};

#endif // STRONGSWORDHANDLER_H
