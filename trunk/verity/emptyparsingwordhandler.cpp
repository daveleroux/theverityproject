#include "emptyparsingwordhandler.h"

#include "parsingevent.h"

EmptyParsingWordHandler::EmptyParsingWordHandler() : WordHandler()
{
}

void EmptyParsingWordHandler::handle(int bibletextId, int wordId)
{
    (new ParsingEvent(QBitArray()))->fire();
}

