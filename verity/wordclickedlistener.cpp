#include "wordclickedlistener.h"

#include "wordclickedevent.h"
#include <QDebug>
#include "biblequerier.h"
#include "strongswordhandler.h"
#include "parsingwordhandler.h"
#include "emptyparsingwordhandler.h"

WordClickedListener::WordClickedListener() : Listener()
{
}

QList<WordHandler*> WordClickedListener::getWordHandlers(int bibletextId)
{
    if(hash.contains(bibletextId))
    {
        return *hash.value(bibletextId);
    }
    else
    {
        QList<int> wordHandlerIds = BibleQuerier::getWordHandlerIds(bibletextId);
        QList<WordHandler*>* list = new QList<WordHandler*>();

        foreach(int wordHandlerId, wordHandlerIds)
        {
            switch(wordHandlerId)
            {
            case STRONGS:
                list->append(new StrongsWordHandler());
                break;
            case PARSING:
                list->append(new ParsingWordHandler());
                break;
            case EMPTY_PARSING:
                list->append(new EmptyParsingWordHandler());
                break;
            }
        }

        hash.insert(bibletextId, list);
        return *hash.value(bibletextId);
    }
}

void WordClickedListener::handleEvent(Event* event)
{
    WordClickedEvent* wordClickedEvent = static_cast<WordClickedEvent*>(event);
    QList<WordHandler*> list = getWordHandlers(wordClickedEvent->bibletextId);

    foreach(WordHandler* wordHandler, list)
    {
        wordHandler->handle(wordClickedEvent->bibletextId, wordClickedEvent->wordId);
    }
}

WordClickedListener::~WordClickedListener()
{
    for(int i=0; i<hash.keys().size(); i++)
    {
        int key = hash.keys().at(i);
        QList<WordHandler*>* list = hash.value(key);
        for(int j=0; j<list->size(); j++)
        {
            delete list->at(j);
        }
        delete list;
    }
}
