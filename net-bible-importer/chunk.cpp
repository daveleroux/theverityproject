#include "chunk.h"

Chunk::Chunk(int bookNumber, int chapter, int verse)
{    
    xmlDoc.appendChild(xmlDoc.createElement("chunk"));

    this->bookNumber = bookNumber;
    this->chapter = chapter;
    this->verse = verse;

    normalisedChapter = 0;
}
