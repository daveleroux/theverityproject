#include "strongsevent.h"
#include "globalvariables.h"

StrongsEvent::StrongsEvent(int bibletextId, int strongsNum) : Event()
{
    this->bibletextId = bibletextId;
    this->strongsNum = strongsNum;
}

EventType::EventType StrongsEvent::getEventType()
{
    return EventType::STRONGS;
}
