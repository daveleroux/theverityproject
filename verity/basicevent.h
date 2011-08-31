#ifndef BASICEVENT_H
#define BASICEVENT_H

#include "event.h"

class BasicEvent : public Event
{
public:
    EventType::EventType eventType;

    BasicEvent(EventType::EventType eventType);
    EventType::EventType getEventType();
};

#endif // BASICEVENT_H
