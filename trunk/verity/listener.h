#ifndef LISTENER_H
#define LISTENER_H

#include "event.h"

class Listener
{
public:
    Listener();
    bool handlesEvent(Event* event);
    void handleEvent(Event* event);
};

#endif // LISTENER_H
