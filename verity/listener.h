#ifndef LISTENER_H
#define LISTENER_H

#include "event.h"

class Listener
{
public:
    Listener();
    virtual void handleEvent(Event* event) = 0;
};

#endif // LISTENER_H
