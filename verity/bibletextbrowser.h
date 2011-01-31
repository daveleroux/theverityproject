#ifndef BIBLETEXTBROWSER_H
#define BIBLETEXTBROWSER_H

#include <QTextBrowser>
#include <QMouseEvent>

#include "basetextunit.h"
#include "textinfo.h"
#include "versereference.h"
#include "documentrepresentation.h"
#include "qsnapscrollbar.h"
#include "chapterdisplayer.h"

class BibleTextBrowser : public QTextBrowser
{
    Q_OBJECT

private:
    QMap<QString, QString> fontFamilies;
    ChapterDisplayer* chapterDisplayer;
    bool sliderBeingPressed;
    bool ignoreSliderValueChanges;

public:
    BibleTextBrowser();

    void display(QList<QString> texts, VerseReference verseReference);

    void writeOutSettings();

protected:

    void display(QList<QString> texts, int idLocation, int normalisedChapterLocaction);

    QSnapScrollBar* markedScrollBar;

    void mouseMoveEvent (QMouseEvent* e);
    void mousePressEvent (QMouseEvent* e);
    void resizeEvent (QResizeEvent* event);


//    void wheelEvent ( QWheelEvent * e );

public slots:
    void tmp(QList<int>);
    void scrollbarValueChanged(int);
    void scrollbarSliderPressed();
    void scrollbarSliderReleased();

signals:
    void wordClicked(TextInfo*);
    void chapterStarts(QList<int>);
    void checkCanScrollSignal(int v);
};

#endif // BIBLETEXTBROWSER_H
