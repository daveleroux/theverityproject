#ifndef BIBLEREFERENCEEVENT_H
#define BIBLEREFERENCEEVENT_H

#include "event.h"
#include "versereference.h"

class BibleReferenceEvent : public Event
{
public:
    VerseReference verseReference;
    BibleReferenceEvent(VerseReference verseReference);
    EventType::EventType getEventType();
};

#endif // BIBLEREFERENCEEVENT_H
