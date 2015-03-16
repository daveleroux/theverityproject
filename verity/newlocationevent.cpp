#include "newlocationevent.h"
#include "globalvariables.h"

NewLocationEvent::NewLocationEvent(VerseReference verseReference)
{
    this->verseReference = verseReference;
}

EventType::EventType NewLocationEvent::getEventType()
{
    return EventType::NEW_LOCATION;
}
