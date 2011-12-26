#ifndef BIBLETEXTBROWSER_H
#define BIBLETEXTBROWSER_H

#include <QTextBrowser>
#include <QMouseEvent>

#include "basetextunit.h"
#include "textinfo.h"
#include "versereference.h"
#include "chapterdisplayer.h"
#include "javascriptclicklistener.h"
#include "vwebhistory.h"

#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QWebElement>
#include "vwebview.h"
#include "scrolllistener.h"
#include "javascriptclicklistener.h"

class BibleTextBrowser : public VWebView
{
    Q_OBJECT

private:
    VWebHistory* webHistory;

    ScrollListener* scrollListener;
    JavascriptClickListener* javascriptClickListener;


public:
    BibleTextBrowser(QWidget *parent = 0);
    ~BibleTextBrowser();
//    void writeOutSettings();

protected:

    void display(QList<int> bibletextIds, int idLocation, int normalisedChapterLocaction);
//    QMimeData* createMimeDataFromSelection() const;

    void handleEvent(Event* event);
    void resizeEvent(QResizeEvent* event);
    void zoomed();

public slots:
    void display(QList<int> bibletextIds, VerseReference verseReference);
    void backward();
    void forward();

    void scrolled();
    void moved();
    void javaScriptWindowObjectClearedSlot();

signals:
    void backwardAvailable(bool);
    void forwardAvailable(bool);
};

#endif // BIBLETEXTBROWSER_H
