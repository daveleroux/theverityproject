#ifndef BIBLETEXTBROWSER_H
#define BIBLETEXTBROWSER_H

#include <QTextBrowser>
#include <QMouseEvent>

#include "basetextunit.h"
#include "textinfo.h"
#include "versereference.h"
#include "chapterdisplayer.h"
#include "javascriptclicklistener.h"

#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QWebElement>
#include "vwebview.h"

class BibleTextBrowser : public VWebView
{
    Q_OBJECT

private:
    ChapterDisplayer* chapterDisplayer;

public:
    BibleTextBrowser(QWidget *parent = 0);
//    void writeOutSettings();

protected:

    void display(QList<int> bibletextIds, int idLocation, int normalisedChapterLocaction);
//    QMimeData* createMimeDataFromSelection() const;

    void handleEvent(Event* event);
    void resizeEvent(QResizeEvent* event);
    void zoomed();

public slots:
    void display(QList<int> bibletextIds, VerseReference verseReference);

signals:
//    void wordClicked(TextInfo*);
//    void checkCanScrollSignal(int v);
};

#endif // BIBLETEXTBROWSER_H
