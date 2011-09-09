#include "versereference.h"

VerseReference::VerseReference()
{

}

VerseReference::VerseReference(int book, int chapter, int verse, QString stringRepresentation)
{
    this->book = book;
    this->chapter = chapter;
    this->verse = verse;
    this->stringRepresentation = stringRepresentation;
}
