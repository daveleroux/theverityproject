#include "javascriptclicklistener.h"

#include <QDebug>
#include "wordclickedevent.h"

JavascriptClickListener::JavascriptClickListener()
{
}

void JavascriptClickListener::wordClicked(int bibletextId, int wordId)
{
    (new WordClickedEvent(bibletextId, wordId))->fire();
}
