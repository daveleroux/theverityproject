#ifndef DOCUMENTREPRESENTATION_H
#define DOCUMENTREPRESENTATION_H

#include <QObject>
#include <QTextBrowser>
#include <QTextDocument>
#include "chapterrepresentation.h"
#include "textinfo.h"
#include "versereference.h"

struct VerseLocation
{
    int id;
    int normalisedChapter;
    VerseLocation(int id, int normalisedChapter)
    {
        this->id = id;
        this->normalisedChapter = normalisedChapter;
    }
};

class DocumentRepresentation : public QObject
{
    Q_OBJECT

private:
    QTextDocument* textDocument;
    QTextBrowser* textBrowser;

    QMap<int, ChapterRepresentation> chapters;
    int minChapter;
    int maxChapter;
    VerseLocation* verseLocation;

    void openDatabase();

    VerseLocation* getVerseLocation(VerseReference verseReference);


    void initialiseMinAndMaxChapters();

    ChapterRepresentation constructChapterRepresentation(int normalisedChapter);
    void addChapter(ChapterRepresentation chRep, bool append);
    void appendChapter(ChapterRepresentation chapterRepresentation);
    void prependChapter(ChapterRepresentation chapterRepresentation);

    QList<TextInfo> readInChapterData(int normalisedChapter);


    int convertPosToGlobal(int normCh, int pos);

    int getFirstNormChapter();
    int getLastNormChapter();

    bool validChapter(int proposedChapter);

    bool mustPrepend(int min, int max, int value, int pageStep);
    bool mustPrepend();

    bool mustAppend(int min, int max, int value, int pageStep);
    bool mustAppend();

    void scrollToCentre(int normCh, int fromPos, int toPos);
    void scrollDown(int pixels);
    void scrollUp(int pixels);
//    void unloadDistantChapters();
    bool canUnloadFirstChapter();
    bool canUnloadLastChapter();
    void unloadFirstChapter();
    void unloadLastChapter();
    int getCurrentChapter();

    QList<int> chapterStartPositions();
    void calculateAndSendChapterStarts();

public:
    DocumentRepresentation(QTextDocument* textDocument, QTextBrowser* textBrowser);
    void display(VerseReference verseReference);
    void checkCanScroll();
    void mousePressed(QPoint point);

signals:
     void selectionRequest(int startPos, int endPos);
     void wordClicked(TextInfo textInfo);
     void chapterStarts(QList<int>);


};

#endif // DOCUMENTREPRESENTATION_H
