#ifndef CHAPTERDISPLAYER_H
#define CHAPTERDISPLAYER_H

#include <QTextBrowser>
#include "chapterrepresentation.h"
#include "textandtextinfo.h"

class ChapterDisplayer : public QObject
{
        Q_OBJECT

protected:
    QTextBrowser* textBrowser;
    int idLocation;
    int normalisedChapterLocation;
    QList<QString> texts;

    QMap<int, ChapterRepresentation> chapters;

    void scrollToCentre(int normCh, int fromPosLocal, int toPosLocal);
    int convertPosToGlobal(int normCh, int localPos);

    ChapterRepresentation insertFirstChapter(int normalisedChapter, int idLocation);
    ChapterRepresentation appendChapter();
    ChapterRepresentation prependChapter();

    virtual ChapterRepresentation constructChapterRepresentation(int normalisedChapter, int idLocation=-1) = 0;

    void addChapter(ChapterRepresentation chapterRepresentation, bool append);

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

    QList<int> chapterStartPositions();

    bool validChapter(int proposedChapter);

    void highlight(int startPos, int endPos);

public:
    ChapterDisplayer(QTextBrowser* textBrowser, int idLocation, int normalisedChapterLocation, QList<QString> texts);
    void display();
    void mousePressed(QPoint point);
    void checkCanScroll();

signals:
    void wordClicked(TextAndTextInfo*);
};

#endif // CHAPTERDISPLAYER_H
