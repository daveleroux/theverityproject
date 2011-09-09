#include "biblereferenceevent.h"
#include "globalvariables.h"

BibleReferenceEvent::BibleReferenceEvent(VerseReference verseReference)
{
    this->verseReference = verseReference;
}

EventType::EventType BibleReferenceEvent::getEventType()
{
    return EventType::BIBLE_REFERENCE;
}

