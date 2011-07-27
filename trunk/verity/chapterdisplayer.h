#ifndef CHAPTERDISPLAYER_H
#define CHAPTERDISPLAYER_H

#include <QTextBrowser>
#include "chapterrepresentation.h"
#include "textinfo.h"
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QWebElement>
#include "scrolllistener.h"

class ChapterDisplayer : public QObject
{
    Q_OBJECT

protected:
    QWebView* webView;
    QList<int> bibletextIds;

    QString frameTop;
    QString frameBottom;

    QMap<int, ChapterRepresentation*> chapters;

    ScrollListener* scrollListener;

    virtual ChapterRepresentation* constructChapterRepresentation(int normalisedChapter, int idLocation=-1) = 0;


    bool validChapter(int proposedChapter);
    int getFirstNormChapter();
    int getLastNormChapter();
    void scrollToCentre();
    QString getHtmlFromChapterRepresentations();
    void add(ChapterRepresentation* chapterRepresentation);
    void prependChapter();
    void appendChapter();
    bool mustPrepend();
    bool mustAppend();
    int getDocumentHeight();
    void unloadFirstChapter();
    void unloadLastChapter();
    bool canUnloadFirstChapter();
    bool canUnloadLastChapter();
    void highlight();
    void insertFirstChapter(int normalisedChapter, int idLocation);
    void scrollTo(int value);

    void printOutHeights(QString toPrint);


    QString transformToHtml(QString xml);

    bool ignoreScrollEvents;


public:
    ChapterDisplayer(QWebView* webView, QList<int> bibleTextIds);
    void display(int id, int normalisedChapter);
    void checkCanScroll();

public slots:
    void scrolled();
    void loadFinished(bool b);
    void javaScriptWindowObjectClearedSlot();

signals:
            //    void wordClicked(TextInfo*);
        };

#endif // CHAPTERDISPLAYER_H
