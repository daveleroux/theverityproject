#ifndef PARALLELTEXTCHAPTERDISPLAYER_H
#define PARALLELTEXTCHAPTERDISPLAYER_H

#include "chapterdisplayer.h"

class ParallelTextChapterDisplayer : public ChapterDisplayer
{
    Q_OBJECT

protected:
    ChapterRepresentation constructChapterRepresentation(int normalisedChapter, int idLocation=-1);

public:
    ParallelTextChapterDisplayer(QTextBrowser* textBrowser, int idLocation, int normalisedChapterLocation, QList<QString> texts);
};

#endif // PARALLELTEXTCHAPTERDISPLAYER_H
