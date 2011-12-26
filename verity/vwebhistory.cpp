#include "vwebhistory.h"

VWebHistory::VWebHistory(int limit)
{
    currentIndex = -1;
    if(limit > 0)
        this->limit = limit;
}

VWebHistory::~VWebHistory()
{
    foreach(VWebHistoryItem* item, items)
    {
        delete item;
    }
}

bool VWebHistory::currentNotNull()
{
    return currentIndex >= 0;
}

VWebHistoryItem* VWebHistory::getCurrentOrNull()
{
    if(currentNotNull())
    {
        return items.at(currentIndex);
    }
    else
        return 0;
}

void VWebHistory::addItemAndDisplay(VWebHistoryItem* item)
{
    if(items.size() == limit)
    {
        VWebHistoryItem* first = items.takeFirst();
        delete first;
    }
    items.append(item);
    currentIndex = items.size() - 1;
    item->display();
}

void VWebHistory::deleteItemsAheadOfCurrent()
{
    while(items.size() > currentIndex + 1)
    {
        VWebHistoryItem* item = items.takeLast();
        delete item;
    }
}

void VWebHistory::display(VWebHistoryItem* item)
{
    if(currentNotNull())
    {
        getCurrentOrNull()->saveState();
        deleteItemsAheadOfCurrent();

    }

    addItemAndDisplay(item);
    emit forwardAvailable(false);
    emit backwardAvailable(canGoBackward());
}

bool VWebHistory::canGoForward()
{
    return currentIndex < items.size() - 1;
}

bool VWebHistory::canGoBackward()
{
    return currentIndex > 0;
}

void VWebHistory::backward()
{
    if(canGoBackward())
    {
        getCurrentOrNull()->saveState();
        currentIndex = currentIndex - 1;
        getCurrentOrNull()->restore();
        emit backwardAvailable(canGoBackward());
        emit forwardAvailable(canGoForward());
    }
}

void VWebHistory::forward()
{
    if(canGoForward())
    {
        getCurrentOrNull()->saveState();
        currentIndex = currentIndex + 1;
        getCurrentOrNull()->restore();
        emit backwardAvailable(canGoBackward());
        emit forwardAvailable(canGoForward());
    }
}

