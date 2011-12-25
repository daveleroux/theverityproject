#ifndef DICTIONARYBROWSER_H
#define DICTIONARYBROWSER_H

#include <QTextBrowser>
#include "listener.h"
#include "vwebview.h"
#include "javascriptclicklistener.h"

class DictionaryBrowser : public VWebView
{
    Q_OBJECT

private:
    QString frameTop;
    QString frameBottom;

    JavascriptClickListener* javascriptClickListener;

public:    
    DictionaryBrowser(QWidget* parent=0);
    ~DictionaryBrowser();

protected:
//    QString convertDefinitionToHTML(QString stylesheet, QString input);
//    QVariant loadResource(int type, const QUrl& name);

    void setNewContent(int strongsNum);
    void handleEvent(Event* event);
    QString transformToHtml(QString xml);

public slots:
    void javaScriptWindowObjectClearedSlot();
//    void display(TextInfo*);
};

#endif // DICTIONARYBROWSER_H
