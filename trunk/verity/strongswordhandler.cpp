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
    (new StrongsEvent(bibletextId, strongsNum))->fire(); //@todo remove the bibletextid hack here, should just send a strongsnum
}
