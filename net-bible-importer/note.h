#ifndef NOTE_H
#define NOTE_H

#include <QDomDocument>
#include <QDomNodeList>
#include <QDomNode>
#include <QDebug>

class Note
{
public:
    int noteId;
    QString stringId;
    QDomDocument xmlDoc;
    Note(int noteId, QString stringId);
};

#endif // NOTE_H
