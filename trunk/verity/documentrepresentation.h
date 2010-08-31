#ifndef DOCUMENTREPRESENTATION_H
#define DOCUMENTREPRESENTATION_H

#include <QObject>
#include <QTextBrowser>
#include <QTextDocument>
#include "chapterrepresentation.h"
#include "textinfo.h"
#include "versereference.h"
#include "textspecificdata.h"

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
    QMap<QString, TextSpecificData*> textSpecificDataMap;
    TextSpecificData* currentTextSpecificData;

    void setCurrentTextSpecificData(QString text);

    VerseLocation* verseLocation;

    void openDatabase();

    VerseLocation* getVerseLocation(VerseReference verseReference);


    TextSpecificData* calculateMinAndMaxChapters(QString text);

    ChapterRepresentation constructChapterRepresentation(QString text, int normalisedChapter);
    void addChapter(ChapterRepresentation chRep, bool append);
    void appendChapter(ChapterRepresentation chapterRepresentation);
    void prependChapter(ChapterRepresentation chapterRepresentation);

    QList<TextInfo> readInChapterData(QString text, int normalisedChapter);


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

    bool canUnloadFirstChapter();
    bool canUnloadLastChapter();
    void unloadFirstChapter();
    void unloadLastChapter();
    int getCurrentChapter();

    QList<int> chapterStartPositions();
    void calculateAndSendChapterStarts();

public:
    DocumentRepresentation(QTextDocument* textDocument, QTextBrowser* textBrowser, QString fontFamily);
    void display(VerseReference verseReference);
    void checkCanScroll();
    void mousePressed(QPoint point);
    QTextCharFormat defaultFormat;


signals:
    void selectionRequest(int startPos, int endPos);
    void wordClicked(QString text, TextInfo textInfo);
    void chapterStarts(QList<int>);
};

#endif // DOCUMENTREPRESENTATION_H
