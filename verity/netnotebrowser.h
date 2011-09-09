#ifndef NETNOTEBROWSER_H
#define NETNOTEBROWSER_H

#include <QWebView>
#include "listener.h"
#include "vwebview.h"

class NetNoteBrowser : public VWebView
{
    Q_OBJECT

public:

    NetNoteBrowser(QWidget *parent = 0);

    void handleEvent(Event* event);

    QString transformToHtml(QString xml);

    QSize sizeHint() const;

//    QSizePolicy sizePolicy();

private:
    QString frameTop;
    QString frameBottom;

signals:

public slots:

};

#endif // NETNOTEBROWSER_H
