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
    QString _readInChapterData(int bibletextId, int normalisedChapter, int selectedId);

    ParallelDTO _readInChapterDataForParallelTexts(QList<int> bibletextIds, QMap<int, int> idsToInclude, int normalisedChapter, int selectedId);
    VerseNode* _readInChapterDataForParallelText(int bibletextId, QSet<int>& parallelIds, int idToInclude, QMap<int, int>& firstIdsMap, QMap<int, int>& lastIdsMap);
    VerseNode* _readInFromMinToMax(int bibletextId, int idFrom, int idTo, QSet<int>& parallelIds, int selectedId=0);

    QList<int> _getWordHandlerIds(int bibletextId);
    int _getStrongsNum(int bibletextId, int wordId);
    QBitArray _getNormalisedMorphTag(int bibletextId, int wordId);

    QString _constructXml(QList<int> bibletextIds, VerseNode* grid);

    QStringList _getChapterRange(int bibletextId, VerseReference verseReference);
    QStringList _getVerseRange(int bibletextId, VerseReference verseReference);

    QStringList _search(QString searchTerms);

    QString asString(QList<int> list);

//    QList<TextInfo> readInTisch(int idFrom, int idTo); //must clean up
    QString readInBible(int bibletextId, int idFrom, int idTo, bool rtl, int selectedId);
//    QList<TextInfo> readInWlc(int idFrom, int idTo); //must clean up
    QString _getNetNote(int id);

    QString _getStrongsData(int strongsNum);

public:
    static VerseLocation* getVerseLocation(int bibletextId, VerseReference verseReference);
    static TextSpecificData* getTextSpecificData(int bibletextId);
    static QString readInChapterData(int bibletextId, int normalisedChapter, int selectedId);

    static ParallelDTO readInChapterDataForParallel(QList<int> bibletextIds, QMap<int, int> idsToInclude, int normalisedChapter, int selectedId);

    static QStringList getChapterRange(int bibletextId, VerseReference verseReference);
    static QStringList getVerseRange(int bibletextId, VerseReference verseReference);

    static QStringList search(QString searchTerms);

    static QList<int> getWordHandlerIds(int bibletextId);
    static int getStrongsNum(int bibletextId, int wordId);
    static QBitArray getNormalisedMorphTag(int bibletextId, int wordId);
    static QString getNetNote(int id);

    static QString getStrongsData(int strongsNum);
};

#endif // BIBLEQUERIER_H
