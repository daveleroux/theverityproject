#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

// a NON-thread-safe singleton

#include "globalvariables.h"
#include "event.h"
#include "listener.h"
#include <QHash>
#include <typeinfo>
using namespace std;

class EventManager
{
private:

    QHash<EventType::EventType, QList<Listener*>* > hash;

    EventManager();
    ~EventManager();

    static EventManager& instance();

    EventManager(const EventManager&);                 // Prevent copy-construction
    EventManager& operator=(const EventManager&);      // Prevent assignment


    void _handleEvent(Event* event);
    void _addListener(EventType::EventType, Listener* listener);

public:
    static void handleEvent(Event* event);
    static void addListener(EventType::EventType, Listener* listener);

};

#endif // EVENTMANAGER_H
