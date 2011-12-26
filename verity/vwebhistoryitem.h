#ifndef VWEBHISTORYITEM_H
#define VWEBHISTORYITEM_H

#include "vwebview.h"

class VWebHistoryItem
{
protected:
    VWebView* webView;

public:
    VWebHistoryItem(VWebView* webView);
    virtual ~VWebHistoryItem();

    virtual void saveState() = 0;
    virtual void restore() = 0;
    virtual void display() = 0;

};

#endif // VWEBHISTORYITEM_H
