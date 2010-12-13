#ifndef BIBLEQUERIER_H
#define BIBLEQUERIER_H

// a NON-thread-safe singleton

#include <QMap>
#include "versereference.h"
#include "verselocation.h"
#include "textspecificdata.h"
#include "textinfo.h"

class BibleQuerier
{
private:

    BibleQuerier();
    QMap<QString, TextSpecificData*> textSpecificDataMap;

    static BibleQuerier& instance();

    BibleQuerier(const BibleQuerier&);                 // Prevent copy-construction
    BibleQuerier& operator=(const BibleQuerier&);      // Prevent assignment


    TextSpecificData* calculateMinAndMaxChapters(QString text);

    VerseLocation* _getVerseLocation(QString text, VerseReference verseReference);
    TextSpecificData* _getTextSpecificData(QString text);
    QList<TextInfo> _readInChapterData(QString text, int normalisedChapter);

public:
    static VerseLocation* getVerseLocation(QString text, VerseReference verseReference);
    static TextSpecificData* getTextSpecificData(QString text);
    static QList<TextInfo> readInChapterData(QString text, int normalisedChapter);
};

#endif // BIBLEQUERIER_H
