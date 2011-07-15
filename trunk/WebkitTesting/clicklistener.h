#ifndef CLICKLISTENER_H
#define CLICKLISTENER_H

#include <QObject>
#include "mainwindow.h"

class ClickListener : public QObject
{
    Q_OBJECT
public:    
    ClickListener();
public slots:
    void wordClicked(int id);
};

#endif // CLICKLISTENER_H
