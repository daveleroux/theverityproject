#ifndef PARSINGDISPLAYBROWSER_H
#define PARSINGDISPLAYBROWSER_H

#include <QTextBrowser>
#include "textinfo.h"
#include "textinfo.h"
#include "listener.h"

class ParsingDisplayBrowser : public QTextBrowser, public Listener
{
    Q_OBJECT

public:
    ParsingDisplayBrowser(QWidget* parent);    

protected:
    QMimeData* createMimeDataFromSelection() const;
    void handleEvent(Event* event);

public slots:
    void display(QBitArray);
};

#endif // PARSINGDISPLAYBROWSER_H
