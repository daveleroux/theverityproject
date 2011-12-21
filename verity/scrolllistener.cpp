#include "scrolllistener.h"
#include <QDebug>

ScrollListener::ScrollListener()
{
}

void ScrollListener::scrolled()
{
    emit scrolledSignal();
}

void ScrollListener::moved()
{
    emit movedSignal();
}
