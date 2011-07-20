#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWebKit/QWebView>
#include <QBasicTimer>
#include "chapterinfo.h"
#include "clicklistener.h"
#include "scrolllistener.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QWebView* webView;
    explicit MainWindow(QWidget *parent = 0);
    void wordClicked(int id);
    QString getHtml(int normalisedChapter);

    QString getParallelHtml(int normalisedChapter);


    void append(int normalisedChapter, QString html);
    void appendChapter();

    void prepend(int normalisedChapter, QString html);
    void prependChapter();

    QList<ChapterInfo*> normalisedChapters;
    ChapterInfo* lastAddedChapterInfo;


    int currentScrollMax;
    int currentHeight;

    void checkCanScroll();
    bool mustAppend();
    bool mustPrepend();

    bool validChapter(int proposedChapter);


    bool canUnloadFirstChapter();
    void unloadFirstChapter();

    bool canUnloadLastChapter();
    void unloadLastChapter();

    QString replaceAll(QString wholeChapter, QMap<QString, QString> replaceMap);

    QString frameTop;
    QString frameBottom;

    QString getHtmlFromChapterInfos();

    ClickListener* clickListener;
    ScrollListener* scrollListener;

    int getDocumentHeight();

public slots:
    void loadFinished(bool b);
    void scrolled();
    void javaScriptWindowObjectCleared();
};

#endif // MAINWINDOW_H
