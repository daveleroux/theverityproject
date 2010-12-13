#ifndef SINGLETEXTCHAPTERDISPLAYER_H
#define SINGLETEXTCHAPTERDISPLAYER_H

#include "chapterdisplayer.h"

class SingleTextChapterDisplayer : public ChapterDisplayer
{
    Q_OBJECT

private:
    QString getText();
protected:
    ChapterRepresentation insertFirstChapter(int normalisedChapter, int idLocation);
    ChapterRepresentation appendChapter();
    ChapterRepresentation prependChapter();

    ChapterRepresentation constructChapterRepresentation(int normalisedChapter, int idLocation=-1);

public:
    SingleTextChapterDisplayer(QTextBrowser* textBrowser, int idLocation, int normalisedChapterLocation, QList<QString> texts);
};

#endif // SINGLETEXTCHAPTERDISPLAYER_H
