#ifndef PARSINGDISPLAYBROWSER_H
#define PARSINGDISPLAYBROWSER_H

#include <QTextBrowser>
#include "textinfo.h"
#include "textandtextinfo.h"

class ParsingDisplayBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    ParsingDisplayBrowser(QWidget* parent);    

protected:
    QMimeData* createMimeDataFromSelection() const;

public slots:
    void display(TextAndTextInfo*);
};

#endif // PARSINGDISPLAYBROWSER_H
