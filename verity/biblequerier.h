#ifndef BIBLEQUERIER_H
#define BIBLEQUERIER_H

// a NON-thread-safe singleton

#include <QMap>
#include "versereference.h"
#include "verselocation.h"
#include "textspecificdata.h"
#include "textinfo.h"
#include "paralleldto.h"
#include "versenode.h"

class BibleQuerier
{
private:

    BibleQuerier();
    QMap<int, TextSpecificData*> textSpecificDataMap;

    static BibleQuerier& instance();

    BibleQuerier(const BibleQuerier&);                 // Prevent copy-construction
    BibleQuerier& operator=(const BibleQuerier&);      // Prevent assignment


    TextSpecificData* __getTextSpecificData(int bibletextId);

    VerseLocation* _getVerseLocation(int bibletextId, VerseReference verseReference);
    TextSpecificData* _getTextSpecificData(int bibletextId);
    QString _readInChapterData(int bibletextId, int normalisedChapter);

    ParallelDTO _readInChapterDataForParallelTexts(QList<int> bibletextIds, QMap<int, int> idsToInclude, int normalisedChapter);
    VerseNode* _readInChapterDataForParallelText(int bibletextId, QSet<int>& parallelIds, int idToInclude, QMap<int, int>& firstIdsMap, QMap<int, int>& lastIdsMap);
    VerseNode* _readInFromMinToMax(int bibletextId, int idFrom, int idTo, QSet<int>& parallelIds);

    QString _constructXml(VerseNode* grid);

    QStringList _search(QString searchTerms);

    QString asString(QList<int> list);

//    QList<TextInfo> readInTisch(int idFrom, int idTo); //must clean up
    QString readInBible(int bibletextId, int idFrom, int idTo);
//    QList<TextInfo> readInWlc(int idFrom, int idTo); //must clean up


public:
    static VerseLocation* getVerseLocation(int bibletextId, VerseReference verseReference);
    static TextSpecificData* getTextSpecificData(int bibletextId);
    static QString readInChapterData(int bibletextId, int normalisedChapter);

    static ParallelDTO readInChapterDataForParallel(QList<int> bibletextIds, QMap<int, int> idsToInclude, int normalisedChapter);

    static QStringList search(QString searchTerms);
};

#endif // BIBLEQUERIER_H
