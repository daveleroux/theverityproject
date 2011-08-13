#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

// a NON-thread-safe singleton

#include "event.h"
#include "listener.h"
#include <QList>

class EventManager
{
private:

    QList<Listener*> listeners;

    EventManager();

    static EventManager& instance();

    EventManager(const EventManager&);                 // Prevent copy-construction
    EventManager& operator=(const EventManager&);      // Prevent assignment


    void _handleEvent(Event* event);

public:
    static void handleEvent(Event* event);

};

#endif // EVENTMANAGER_H
