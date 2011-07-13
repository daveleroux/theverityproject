#include "clicklistener.h"
#include <QDebug>

ClickListener::ClickListener(MainWindow* mainWindow)
{
    this->mainWindow = mainWindow;
}

QString ClickListener::wordClicked(int id)
{
    QString arb = QString().setNum(id);
//    arb = "word clicked" + id;
//    qDebug() << "word clicked" << id;
//    mainWindow->wordClicked(id);
    return arb;
}
