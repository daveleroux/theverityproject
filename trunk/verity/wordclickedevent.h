#ifndef WORDCLICKEDEVENT_H
#define WORDCLICKEDEVENT_H

#include "event.h"

class WordClickedEvent : public Event
{
public:
    int bibletextId;
    int wordId;
    WordClickedEvent(int bibletextId, int wordId);
};

#endif // WORDCLICKEDEVENT_H
