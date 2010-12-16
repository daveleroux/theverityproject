#ifndef CHAPTERDISPLAYER_H
#define CHAPTERDISPLAYER_H

#include <QTextBrowser>
#include "chapterrepresentation.h"
#include "textinfo.h"

class ChapterDisplayer : public QObject
{
    Q_OBJECT

protected:
    QTextBrowser* textBrowser;
    QList<QString> texts;

    QMap<int, ChapterRepresentation*> chapters;
    QMap<QString, QString> fontFamilies;

    void scrollToCentre(int normCh, int fromPosLocal, int toPosLocal);
    int convertPosToGlobal(int normCh, int localPos);

    ChapterRepresentation* insertFirstChapter(int normalisedChapter, int idLocation);
    ChapterRepresentation* appendChapter();
    ChapterRepresentation* prependChapter();

    virtual ChapterRepresentation* constructChapterRepresentation(int normalisedChapter, int idLocation=-1) = 0;

    void addChapter(ChapterRepresentation* chapterRepresentation, bool append);

    bool mustAppend(int min, int max, int value, int pageStep);
    bool mustAppend();
    bool mustPrepend(int min, int max, int value, int pageStep);
    bool mustPrepend();

    int getFirstNormChapter();
    int getLastNormChapter();

    QString getPrimaryText();


    void scrollDown(int pixels);
    void scrollUp(int pixels);

    bool canUnloadLastChapter();
    bool canUnloadFirstChapter();

    int getCurrentChapter();

    void unloadLastChapter();
    void unloadFirstChapter();

    void calculateAndSendChapterStarts();


    QList<int> chapterStartPositions();

    bool validChapter(int proposedChapter);

    QTextCharFormat getSuperscriptFormat();
    QTextCharFormat getDefaultFormat(QString text);
    QTextCharFormat getBoldFormat();

    void highlight(int startPos, int endPos);

    QString getFontFamily(QString text);

public:
    ChapterDisplayer(QTextBrowser* textBrowser, QList<QString> texts, QMap<QString, QString> fontFamilies);
    void display(int id, int normalisedChapter);
    void mousePressed(QPoint point);
    void checkCanScroll();

signals:
    void wordClicked(TextInfo*);
    void chapterStarts(QList<int>);
};

#endif // CHAPTERDISPLAYER_H
