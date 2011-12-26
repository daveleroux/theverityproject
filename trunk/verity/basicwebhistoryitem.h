#ifndef BASICWEBHISTORYITEM_H
#define BASICWEBHISTORYITEM_H

#include "vwebhistoryitem.h"

class BasicWebHistoryItem : public VWebHistoryItem
{
protected:
    QString html;
    int scrollValue;

    void saveState();
    void restore();
    void display();

public:
    BasicWebHistoryItem(VWebView* webView, QString html);
    ~BasicWebHistoryItem();


};

#endif // BASICWEBHISTORYITEM_H
