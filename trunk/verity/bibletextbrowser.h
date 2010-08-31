#ifndef BIBLETEXTBROWSER_H
#define BIBLETEXTBROWSER_H

#include <QTextBrowser>
#include <QMouseEvent>

#include "basetextunit.h"
#include "textinfo.h"
#include "versereference.h"
#include "documentrepresentation.h"
#include "qsnapscrollbar.h"

class BibleTextBrowser : public QTextBrowser
{
    Q_OBJECT

private:
    DocumentRepresentation* documentRepresentation;

public:
    BibleTextBrowser();
    void display(VerseReference verseReference);

    void writeOutSettings();

protected:

    QSnapScrollBar* markedScrollBar;

    void mouseMoveEvent (QMouseEvent* e);
    void mousePressEvent ( QMouseEvent * e );


    void wheelEvent ( QWheelEvent * e );

public slots:
    void select(int, int);
    void tmp(QList<int>);

signals:
    //     void wordHoveredOver(TextInfo textInfo);
    void wordClicked(QString, TextInfo textInfo);
    void chapterStarts(QList<int>);
};

#endif // BIBLETEXTBROWSER_H
