#ifndef PARALLELTEXTCHAPTERDISPLAYER_H
#define PARALLELTEXTCHAPTERDISPLAYER_H

#include "chapterdisplayer.h"

class ParallelTextChapterDisplayer : public ChapterDisplayer
{
    Q_OBJECT

protected:
    ChapterRepresentation* constructChapterRepresentation(int normalisedChapter, int idLocation=-1);

    QSet<int> extractParallelIds(QList<TextInfo> textInfos);

    QString transformToHtml(QString xml, bool evenStart);

public:
    ParallelTextChapterDisplayer(VWebView* webView, QList<int> bibletextIds, int id, int normalisedChapter);
};

#endif // PARALLELTEXTCHAPTERDISPLAYER_H
