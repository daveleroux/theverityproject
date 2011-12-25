#include "javascriptclicklistener.h"

#include <QDebug>
#include "wordclickedevent.h"
#include "netnoteevent.h"
#include "strongsevent.h"

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

void JavascriptClickListener::strongsClicked(int id)
{
    (new StrongsEvent(id))->fire();
}
