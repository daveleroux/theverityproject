#include "singletextchapterdisplayer.h"
#include "biblequerier.h"

SingleTextChapterDisplayer::SingleTextChapterDisplayer(VWebView* webView, QList<int> bibletextIds, int id, int normalisedChapter) :
        ChapterDisplayer(webView, bibletextIds, id, normalisedChapter)
{
}


ChapterRepresentation* SingleTextChapterDisplayer::constructChapterRepresentation(int normalisedChapter, int idLocation)
{
    QString xml = BibleQuerier::readInChapterData(bibletextIds.at(0), normalisedChapter, idLocation);

    return new ChapterRepresentation(normalisedChapter, transformToHtml(xml));
}
