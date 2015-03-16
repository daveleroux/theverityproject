#ifndef SEARCHDISPLAYBROWSER_H
#define SEARCHDISPLAYBROWSER_H

#include "listener.h"
#include "vwebview.h"
#include "versereference.h"
#include "javascriptclicklistener.h"



class SearchDisplayBrowser : public VWebView
{
    Q_OBJECT
private:
    QString frameTop;
    QString frameBottom;
    JavascriptClickListener* javascriptClickListener;

public:
    SearchDisplayBrowser(QWidget* parent);
    ~SearchDisplayBrowser();

protected:
    void handleEvent(Event* event);
    QSize sizeHint() const;
    void display(QString searchTerm);
    QList<SearchResult> doActualSearch(QString searchTerm);

public slots:
    void javaScriptWindowObjectClearedSlot();
};

#endif // SEARCHDISPLAYBROWSER_H
