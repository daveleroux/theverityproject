#ifndef VWEBVIEW_H
#define VWEBVIEW_H

#include <QtWebKit/QWebView>
#include "listener.h"

class VWebView : public QWebView, public Listener
{
    Q_OBJECT    

private:
    int thisId;
    static int staticIdentifier;
    static QString VWEBVIEW_SETTING_GROUP;

protected:
    void wheelEvent(QWheelEvent *event);
    void handleEvent(Event* event);
    virtual void zoomed();


public:
    VWebView(QWidget *parent = 0);
    ~VWebView();

signals:

public slots:

};

#endif // VWEBVIEW_H
