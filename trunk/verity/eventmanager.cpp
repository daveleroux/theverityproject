#include "eventmanager.h"

EventManager::EventManager()
{
}

void EventManager::_handleEvent(Event* event)
{
    foreach(Listener* listener, listeners)
    {
        if(listener->handlesEvent(event))
            listener->handleEvent(event);
    }
    delete event;
}

EventManager& EventManager::instance()
{
    static EventManager singleton;
    return singleton;
}

void EventManager::handleEvent(Event* event)
{
    return instance()._handleEvent(event);
}

