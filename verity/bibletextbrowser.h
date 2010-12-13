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
//    DocumentRepresentation* documentRepresentation;
    QString fontFamily;
    ChapterDisplayer* chapterDisplayer;

public:
    BibleTextBrowser();

    void display(QList<QString> texts, VerseReference verseReference);

    void writeOutSettings();

protected:

    void display(QList<QString> texts, int idLocation, int normalisedChapterLocaction);

    QSnapScrollBar* markedScrollBar;

    void mouseMoveEvent (QMouseEvent* e);
    void mousePressEvent ( QMouseEvent * e );


    void wheelEvent ( QWheelEvent * e );

public slots:
//    void select(int, int);
//    void tmp(QList<int>);

signals:
    void wordClicked(TextAndTextInfo*);
    void chapterStarts(QList<int>);
};

#endif // BIBLETEXTBROWSER_H
