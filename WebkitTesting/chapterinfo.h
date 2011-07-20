#ifndef CHAPTERINFO_H
#define CHAPTERINFO_H

#include <QString>

struct ChapterInfo
{
    int normalisedChapter;
    int height;
    QString html;
    ChapterInfo(int normalisedChapter, int height, QString html);
};

#endif // CHAPTERINFO_H
