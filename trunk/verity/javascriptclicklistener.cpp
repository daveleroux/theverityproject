#include "javascriptclicklistener.h"

#include <QDebug>
#include "wordclickedevent.h"
#include "netnoteevent.h"

JavascriptClickListener::JavascriptClickListener()
{
}

void JavascriptClickListener::wordClicked(int bibletextId, int wordId)
{
    (new WordClickedEvent(bibletextId, wordId))->fire();
}

void JavascriptClickListener::netNoteClicked(int id)
{
    (new NetNoteEvent(id))->fire();
}
