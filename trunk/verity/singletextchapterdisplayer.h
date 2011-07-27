#ifndef SINGLETEXTCHAPTERDISPLAYER_H
#define SINGLETEXTCHAPTERDISPLAYER_H

#include "chapterdisplayer.h"

class SingleTextChapterDisplayer : public ChapterDisplayer
{
    Q_OBJECT

protected:
    ChapterRepresentation* constructChapterRepresentation(int normalisedChapter, int idLocation=-1);

public:
    SingleTextChapterDisplayer(QWebView* webView, QList<int> bibletextIds);
};

#endif // SINGLETEXTCHAPTERDISPLAYER_H
