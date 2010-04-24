#ifndef DICTIONARYBROWSER_H
#define DICTIONARYBROWSER_H

#include <QTextBrowser>
#include "textinfo.h"

class DictionaryBrowser : public QTextBrowser
{
    Q_OBJECT

public:    
    DictionaryBrowser(QWidget* parent);

protected:
    QString convertDefinitionToHTML(QString input);

public slots:
    void display(QUrl);
    void display(TextInfo);

};

#endif // DICTIONARYBROWSER_H
