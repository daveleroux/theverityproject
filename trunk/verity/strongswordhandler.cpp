#include "strongswordhandler.h"
#include "biblequerier.h"
#include "strongsevent.h"
#include <QDebug>

StrongsWordHandler::StrongsWordHandler() : WordHandler()
{
}

void StrongsWordHandler::handle(int bibletextId, int wordId)
{
    int strongsNum = BibleQuerier::getStrongsNum(bibletextId, wordId);
    (new StrongsEvent(strongsNum))->fire();
}
