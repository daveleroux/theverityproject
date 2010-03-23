#ifndef PARSINGDISPLAYBROWSER_H
#define PARSINGDISPLAYBROWSER_H

#include <QTextBrowser>
#include "textinfo.h"

class ParsingDisplayBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    ParsingDisplayBrowser();
    ParsingDisplayBrowser(QWidget* parent);
    void display(TextInfo textInfo, bool showDefinition=false);
};

#endif // PARSINGDISPLAYBROWSER_H
