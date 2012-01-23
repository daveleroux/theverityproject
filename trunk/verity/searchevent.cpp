#include "searchevent.h"

SearchEvent::SearchEvent(QString searchTerm)
{
    this->searchTerm = searchTerm;
}

EventType::EventType SearchEvent::getEventType()
{
    return EventType::SEARCH;
}
