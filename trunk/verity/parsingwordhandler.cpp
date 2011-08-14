#include "parsingwordhandler.h"
#include "parsingevent.h"
#include "biblequerier.h"

ParsingWordHandler::ParsingWordHandler() : WordHandler()
{
}

void ParsingWordHandler::handle(int bibletextId, int wordId)
{
    QBitArray normalisedMorphTag = BibleQuerier::getNormalisedMorphTag(bibletextId, wordId);
    (new ParsingEvent(normalisedMorphTag))->fire();
}
