#ifndef STRONGSEVENT_H
#define STRONGSEVENT_H

#include "event.h"

class StrongsEvent : public Event
{
public:
    int bibletextId; //todo remove bibletextId
    int strongsNum;
    StrongsEvent(int bibletextId, int strongsNum);
    EventType::EventType getEventType();
};

#endif // STRONGSEVENT_H
