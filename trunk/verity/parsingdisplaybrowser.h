#ifndef PARSINGDISPLAYBROWSER_H
#define PARSINGDISPLAYBROWSER_H

#include <QTextBrowser>
#include "textinfo.h"

class ParsingDisplayBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    ParsingDisplayBrowser(QWidget* parent);    

public slots:
    void display(TextInfo textInfo);
};

#endif // PARSINGDISPLAYBROWSER_H
