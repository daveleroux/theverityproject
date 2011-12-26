#ifndef CHAPTERDISPLAYER_H
#define CHAPTERDISPLAYER_H

#include <QTextBrowser>
#include "chapterrepresentation.h"
#include "textinfo.h"
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QWebElement>
#include "vwebhistoryitem.h"

class ChapterDisplayer : public QObject, public VWebHistoryItem
{
    Q_OBJECT

protected:
    VWebView* webView;
    QList<int> bibletextIds;

    QString frameTop;
    QString frameBottom;

    QMap<int, ChapterRepresentation*> chapters;


    int id;
    int normalisedChapter;

    int savedScrollValue;

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
    void insertFirstChapter(int normalisedChapter, int idLocation);
    void scrollTo(int value);

    void printOutHeights(QString toPrint);


    QString transformToHtml(QString xml);

    bool ignoreScrollEvents;

    void saveState();
    void restore();


public:
    ChapterDisplayer(VWebView* webView, QList<int> bibleTextIds, int id, int normalisedChapter);
    void display();
    void checkCanScroll();

    void scrolled();
    void moved();

    ~ChapterDisplayer();


};

#endif // CHAPTERDISPLAYER_H
