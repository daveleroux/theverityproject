#ifndef SCROLLLISTENER_H
#define SCROLLLISTENER_H

#include <QObject>

class ScrollListener : public QObject
{
    Q_OBJECT

public:
    ScrollListener();
signals:
    void scrolledSignal();
public slots:
    void scrolled();

};

#endif // SCROLLLISTENER_H
