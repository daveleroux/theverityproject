#ifndef DICTIONARYBROWSER_H
#define DICTIONARYBROWSER_H

#include <QTextBrowser>
#include "textinfo.h"
#include "textinfo.h"
#include "listener.h"

class DictionaryBrowser : public QTextBrowser, public Listener
{
    Q_OBJECT

public:    
    DictionaryBrowser(QWidget* parent=0);

protected:
    QString convertDefinitionToHTML(QString stylesheet, QString input);
    QVariant loadResource(int type, const QUrl& name);

    void handleEvent(Event* event);

public slots:
    void display(TextInfo*);
};

#endif // DICTIONARYBROWSER_H
