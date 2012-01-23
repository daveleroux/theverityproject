#ifndef PARSINGDISPLAYBROWSER_H
#define PARSINGDISPLAYBROWSER_H

#include "textinfo.h"
#include "textinfo.h"
#include "listener.h"
#include "vwebview.h"

class ParsingDisplayBrowser : public VWebView
{
    Q_OBJECT

private:
    QString frameTop;
    QString frameBottom;

public:
    ParsingDisplayBrowser(QWidget* parent);    

protected:
//    QMimeData* createMimeDataFromSelection() const;
    void handleEvent(Event* event);
    QSize sizeHint() const;

//public slots:
    void display(QBitArray);
};

#endif // PARSINGDISPLAYBROWSER_H
