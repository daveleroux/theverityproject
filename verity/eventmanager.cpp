#include "eventmanager.h"

EventManager::EventManager()
{
}

EventManager::~EventManager()
{
    foreach(QList<Listener*>* list, hash.values())
    {
        delete list;
    }
}

void EventManager::_handleEvent(Event* eventToHandle)
{
    QList<Listener*>* list = hash.value(eventToHandle->getEventType());
    if(list != 0)
    {
        for(int i=0; i<list->size(); i++)
        {
           list->at(i)->handleEvent(eventToHandle);
        }
    }

    delete eventToHandle;
}

void EventManager::_addListener(EventType::EventType eventType, Listener* listener)
{
    QList<Listener*>* list = hash.value(eventType);
    if(list == 0)
    {
        list = new QList<Listener*>();
        list->append(listener);
        hash.insert(eventType, list);
    }
    else
    {
        list->append(listener);
    }
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

void EventManager::addListener(EventType::EventType eventType, Listener* listener)
{
    instance()._addListener(eventType, listener);
}
