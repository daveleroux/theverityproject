#include "chapterrepresentation.h"
#include <QDebug>

ChapterRepresentation::ChapterRepresentation()
{
}

ChapterRepresentation::ChapterRepresentation(int normalisedChapter,
                                             QString html)
{
    this->normalisedChapter = normalisedChapter;
    this->html = html;
    this->height = 0;
}


int ChapterRepresentation::getNormalisedChapter()
{
    return normalisedChapter;
}

void ChapterRepresentation::setHeight(int height)
{
    this->height = height;
}

int ChapterRepresentation::getHeight()
{
    return height;
}

QString ChapterRepresentation::getHtml()
{
    return html;
}
