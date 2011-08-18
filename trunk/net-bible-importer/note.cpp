#include "note.h"

Note::Note(int noteId, QString stringId)
{
    xmlDoc.appendChild(xmlDoc.createElement("note"));
    this->noteId = noteId;
    this->stringId = stringId;
}
