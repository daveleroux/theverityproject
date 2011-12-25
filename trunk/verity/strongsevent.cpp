#include "strongsevent.h"
#include "globalvariables.h"

StrongsEvent::StrongsEvent(int strongsNum) : Event()
{    
    this->strongsNum = strongsNum;
}

EventType::EventType StrongsEvent::getEventType()
{
    return EventType::STRONGS;
}
