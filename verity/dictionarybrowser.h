#ifndef DICTIONARYBROWSER_H
#define DICTIONARYBROWSER_H

#include <QTextBrowser>
#include "listener.h"
#include "vwebview.h"
#include "javascriptclicklistener.h"
#include "vwebhistory.h"

class DictionaryBrowser : public VWebView
{
    Q_OBJECT

private:
    QString frameTop;
    QString frameBottom;

    JavascriptClickListener* javascriptClickListener;
    VWebHistory* vWebHistory;

public:    
    DictionaryBrowser(QWidget* parent=0);
    ~DictionaryBrowser();

protected:
    void setNewContent(int strongsNum);
    void handleEvent(Event* event);
    QString transformToHtml(QString xml);

signals:
    void backwardAvailable(bool);
    void forwardAvailable(bool);

public slots:
    void javaScriptWindowObjectClearedSlot();
    void backward();
    void forward();
};

#endif // DICTIONARYBROWSER_H
