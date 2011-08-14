#ifndef PARSINGEVENT_H
#define PARSINGEVENT_H

#include "event.h"
#include <QBitArray>

class ParsingEvent : public Event
{
public:
    QBitArray normalisedMorphTag;
    ParsingEvent(QBitArray normalisedMorphTag);
    EventType::EventType getEventType();
};

#endif // PARSINGEVENT_H
