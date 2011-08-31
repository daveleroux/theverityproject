#include "basicevent.h"

BasicEvent::BasicEvent(EventType::EventType eventType) : Event()
{
    this->eventType = eventType;
}

EventType::EventType BasicEvent::getEventType()
{
    return eventType;
}
