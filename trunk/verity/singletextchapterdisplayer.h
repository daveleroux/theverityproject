#ifndef SINGLETEXTCHAPTERDISPLAYER_H
#define SINGLETEXTCHAPTERDISPLAYER_H

#include "chapterdisplayer.h"

class SingleTextChapterDisplayer : public ChapterDisplayer
{
    Q_OBJECT

private:
    QString getText();

protected:
    ChapterRepresentation* constructChapterRepresentation(int normalisedChapter, int idLocation=-1);

public:
    SingleTextChapterDisplayer(QTextBrowser* textBrowser, QList<QString> texts, QMap<QString, QString> fontFamilies);
};

#endif // SINGLETEXTCHAPTERDISPLAYER_H
