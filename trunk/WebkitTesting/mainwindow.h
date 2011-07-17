#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWebKit/QWebView>
#include <QBasicTimer>
#include "chapterinfo.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QWebView* webView;
    explicit MainWindow(QWidget *parent = 0);
    void wordClicked(int id);
    ~MainWindow();
    QString getHtml(int normalisedChapter);

    QString getParallelHtml(int normalisedChapter);

    void display(int normalisedChapter);

    void append(int normalisedChapter, QString html);
    void appendChapter();

    void prepend(int normalisedChapter, QString html);
    void prependChapter();

    QList<ChapterInfo*> normalisedChapters;
    ChapterInfo* lastAddedChapterInfo;

    QBasicTimer basicTimer;
    int currentScrollMax;
    int currentHeight;

    void checkCanScroll();
    bool mustAppend();
    bool mustPrepend();

    bool validChapter(int proposedChapter);

    bool waitingUntilScrollbarMaxChanged;
    bool mustScrollDown;
    bool mustScrollToPoint;
    int pointToScrollTo;

    void timerEvent ( QTimerEvent * event );

    bool canUnloadFirstChapter();
    void unloadFirstChapter();

    bool canUnloadLastChapter();
    void unloadLastChapter();
public slots:
    void loadFinished(bool b);
    void scrolled();
};

#endif // MAINWINDOW_H
