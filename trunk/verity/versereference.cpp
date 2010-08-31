#include "versereference.h"

VerseReference::VerseReference(QString text, int book, int chapter, int verse, QString stringRepresentation)
{
    this->text = text;
    this->book = book;
    this->chapter = chapter;
    this->verse = verse;
    this->stringRepresentation = stringRepresentation;
}
