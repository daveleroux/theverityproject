#ifndef NEWLOCATIONEVENT_H
#define NEWLOCATIONEVENT_H

#include "event.h"
#include "versereference.h"

class NewLocationEvent : public Event
{
public:
    NewLocationEvent(VerseReference verseReference);
    VerseReference verseReference;
    EventType::EventType getEventType();
};

#endif // NEWLOCATIONEVENT_H
