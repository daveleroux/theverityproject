#ifndef STRONGSEVENT_H
#define STRONGSEVENT_H

#include "event.h"

class StrongsEvent : public Event
{
public:
    int strongsNum;
    StrongsEvent(int strongsNum);
    EventType::EventType getEventType();
};

#endif // STRONGSEVENT_H
