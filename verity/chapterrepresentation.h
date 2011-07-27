#ifndef CHAPTERREPRESENTATION_H
#define CHAPTERREPRESENTATION_H

#include <QString>

class ChapterRepresentation
{
protected:
    int normalisedChapter;
    QString html;
    int height;

public:
    ChapterRepresentation();
    ChapterRepresentation(int normalisedChapter,
                          QString html);

    int getNormalisedChapter();
    void setHeight(int height);
    QString getHtml();
    int getHeight();
};

#endif // CHAPTERREPRESENTATION_H
