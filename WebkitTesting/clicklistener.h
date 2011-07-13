#ifndef CLICKLISTENER_H
#define CLICKLISTENER_H

#include <QObject>
#include "mainwindow.h"

class ClickListener : public QObject
{
    Q_OBJECT
public:
    MainWindow* mainWindow;
    ClickListener(MainWindow* mainWindow);
public slots:
    QString wordClicked(int id);
};

#endif // CLICKLISTENER_H
