#include "javascriptclicklistener.h"

#include <QDebug>

JavascriptClickListener::JavascriptClickListener()
{
}

void JavascriptClickListener::wordClicked(int bibletextId, int wordId)
{
    qDebug() << "got a wordclicked " << bibletextId << " " << wordId;
}
