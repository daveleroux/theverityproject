#ifndef SEARCHEVENT_H
#define SEARCHEVENT_H

#include "event.h"

class SearchEvent : public Event
{
public:
    QString searchTerm;
    SearchEvent(QString searchterm);
    EventType::EventType getEventType();
};

#endif // SEARCHEVENT_H
