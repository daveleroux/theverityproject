#ifndef NETNOTEEVENT_H
#define NETNOTEEVENT_H

#include "event.h"

class NetNoteEvent : public Event
{
public:
    int id;
    NetNoteEvent(int id);
    EventType::EventType getEventType();
};

#endif // NETNOTEEVENT_H
