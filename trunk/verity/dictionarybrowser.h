#ifndef DICTIONARYBROWSER_H
#define DICTIONARYBROWSER_H

#include <QTextBrowser>
#include "textinfo.h"

class DictionaryBrowser : public QTextBrowser
{
    Q_OBJECT

public:    
    DictionaryBrowser(QWidget* parent=0);

protected:
    QString convertDefinitionToHTML(QString stylesheet, QString input);
    QVariant loadResource(int type, const QUrl& name);

public slots:
    void display(QString, TextInfo);
};

#endif // DICTIONARYBROWSER_H
