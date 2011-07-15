#include "clicklistener.h"
#include <QDebug>

ClickListener::ClickListener()
{
}

void ClickListener::wordClicked(int id)
{
    qDebug() << "dom modified " << id;
}
