#ifndef BIBLETEXTBROWSER_H
#define BIBLETEXTBROWSER_H

#include <QTextBrowser>
#include <QMouseEvent>

#include "basetextunit.h"
#include "textinfo.h"
#include "versereference.h"

class BibleTextBrowser : public QTextBrowser
{
     Q_OBJECT

private:
    BaseTextUnit key(QPoint point);

public:
    BibleTextBrowser();
    void display(VerseReference verseReference);

protected:
     QTextDocument* currentTextDocument;
     QTextCursor* currentTextCursor;

    void mouseMoveEvent (QMouseEvent* e);
    void mousePressEvent ( QMouseEvent * e );

    QMap<BaseTextUnit, TextInfo> textUnits;

    QList<TextInfo> readInParagraph(int paragraph);
    int insertParagraphIntoDocument(int id, QList<TextInfo> textInfos);

signals:
     void wordHoveredOver(TextInfo textInfo);
     void wordClicked(TextInfo textInfo);
};

#endif // BIBLETEXTBROWSER_H
