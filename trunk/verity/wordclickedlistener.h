#ifndef WORDCLICKEDLISTENER_H
#define WORDCLICKEDLISTENER_H

#include "listener.h"
#include "wordhandler.h"
#include <QList>
#include <QHash>

class WordClickedListener : public Listener
{
private:
    enum WordHandlerType
    {
        STRONGS,
        PARSING,
    };

    QHash<int, QList<WordHandler*>* > hash;

    QList<WordHandler*> getWordHandlers(int bibletextId);

public:
    WordClickedListener();
    void handleEvent(Event* event);
    ~WordClickedListener();
};

#endif // WORDCLICKEDLISTENER_H
