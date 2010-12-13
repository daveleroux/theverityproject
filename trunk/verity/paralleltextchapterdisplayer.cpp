#include "paralleltextchapterdisplayer.h"

ParallelTextChapterDisplayer::ParallelTextChapterDisplayer(QTextBrowser* textBrowser, int idLocation, int normalisedChapterLocation, QList<QString> texts) :
        ChapterDisplayer(textBrowser, idLocation, normalisedChapterLocation, texts)
{
}

ChapterRepresentation ParallelTextChapterDisplayer::insertFirstChapter(int normalisedChapter, int idLocation)
{
    return ChapterRepresentation();
}

ChapterRepresentation ParallelTextChapterDisplayer::appendChapter()
{
    return ChapterRepresentation();
}

ChapterRepresentation ParallelTextChapterDisplayer::prependChapter()
{
    return ChapterRepresentation();
}
