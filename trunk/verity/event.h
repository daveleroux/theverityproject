#ifndef EVENT_H
#define EVENT_H

#include "globalvariables.h"

class Event
{
public:
    Event();
    void fire();
    virtual EventType::EventType getEventType() = 0;
};

#endif // EVENT_H
