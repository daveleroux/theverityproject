#include "netnoteevent.h"
#include "globalvariables.h"

NetNoteEvent::NetNoteEvent(int id)
{
    this->id = id;
}

EventType::EventType NetNoteEvent::getEventType()
{
    return EventType::NET_NOTE;
}
