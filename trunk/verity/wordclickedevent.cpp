#include "wordclickedevent.h"
#include "globalvariables.h"

WordClickedEvent::WordClickedEvent(int bibletextId, int wordId) : Event()
{
    this->bibletextId = bibletextId;
    this->wordId = wordId;
}

EventType::EventType WordClickedEvent::getEventType()
{
    return EventType::WORD_CLICKED;
}
