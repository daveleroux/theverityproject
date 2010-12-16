#ifndef PARALLELTEXTCHAPTERDISPLAYER_H
#define PARALLELTEXTCHAPTERDISPLAYER_H

#include "chapterdisplayer.h"

class ParallelTextChapterDisplayer : public ChapterDisplayer
{
    Q_OBJECT

protected:
    ChapterRepresentation* constructChapterRepresentation(int normalisedChapter, int idLocation=-1);

    QSet<int> extractParallelIds(QList<TextInfo> textInfos);

public:
    ParallelTextChapterDisplayer(QTextBrowser* textBrowser, QList<QString> texts, QMap<QString, QString> fontFamilies);
};

#endif // PARALLELTEXTCHAPTERDISPLAYER_H
