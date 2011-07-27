#include "singletextchapterdisplayer.h"
#include "biblequerier.h"

SingleTextChapterDisplayer::SingleTextChapterDisplayer(QWebView* webView, QList<int> bibletextIds) :
        ChapterDisplayer(webView, bibletextIds)
{
}


ChapterRepresentation* SingleTextChapterDisplayer::constructChapterRepresentation(int normalisedChapter, int idLocation)
{
    QString xml = BibleQuerier::readInChapterData(bibletextIds.at(0), normalisedChapter);

    return new ChapterRepresentation(normalisedChapter, transformToHtml(xml));
}
