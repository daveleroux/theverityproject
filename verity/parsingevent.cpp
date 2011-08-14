#include "parsingevent.h"
#include "globalvariables.h"

ParsingEvent::ParsingEvent(QBitArray normalisedMorphTag) : Event()
{
    this->normalisedMorphTag = normalisedMorphTag;
}

EventType::EventType ParsingEvent::getEventType()
{
    return EventType::PARSING;
}
