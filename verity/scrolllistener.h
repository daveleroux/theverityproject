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
    void movedSignal();
public slots:
    void scrolled();
    void moved();

};

#endif // SCROLLLISTENER_H
