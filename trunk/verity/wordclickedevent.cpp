#include "wordclickedevent.h"

WordClickedEvent::WordClickedEvent(int bibletextId, int wordId) : Event()
{
    this->bibletextId = bibletextId;
    this->wordId = wordId;
}
