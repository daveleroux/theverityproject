#ifndef CHUNK_H
#define CHUNK_H

#include <QDomDocument>
#include <QDomNodeList>
#include <QDomNode>
#include <QDebug>

class Chunk
{
public:    
    int chapter;
    int verse;

    int bookNumber;
    int normalisedChapter;

    QDomDocument xmlDoc;
    Chunk(int bookNumber, int chapter = 0, int verse = 0);
};

#endif // CHUNK_H
