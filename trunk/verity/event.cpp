#include "event.h"
#include "eventmanager.h"

Event::Event()
{
}

void Event::fire()
{
    EventManager::handleEvent(this);
}
