#include <QtCore/QCoreApplication>
#include <QtSql>
#include <QDomDocument>
#include <QDomNodeList>
#include <QDomNode>
#include <QDebug>
#include <QFile>
#include <QMap>
#include "chunk.h"
#include "note.h"

int noteId;
Note* currentNote;
QMap<QString, int> noteIdMap;
//QDomNode* notePlaceToAdd = 0;
int netNoteChapterCount;

bool LIMIT_NET_NOTES = true;

Chunk* currentChunk;
QList<Chunk*> allChunks;

QDomNode* placeToAdd = 0;

int bookNumber;


QMap<QString, QString> map;

QStringList books;

bool SHOULD_NOT_START_NEW_CHUNK;

void assert(bool b)
{
    if(!b)
    {
        qDebug() << "assert failed";
        exit(1);
    }
}

void assert(int l, int r)
{
    if ( l != r)
    {
        qDebug() << "assert failed: " << l << " : " << r;
        exit(1);
    }
}

void assert(QString l, QString r)
{
    if( l != r)
    {
        qDebug() << "assert failed: " << l << " : " << r;
        exit(1);
    }
}

QString getDatabaseTagName(QString netTagName)
{
    assert(map.keys().contains(netTagName));
    return map.value(netTagName);
}

void printElement(QDomElement element)
{
    QTextStream stream;
    QString streamString;
    stream.setString(&streamString);
    stream << element;

    qDebug() << "element:";
    qDebug() << *stream.string();
}

void appendCurrentChunk()
{
    if(currentChunk != 0)
    {
        QDomElement element = currentChunk->xmlDoc.firstChildElement();
        QDomElement childElement = element.firstChildElement();
        while(!childElement.isNull())
        {
            QDomElement nextChildElement = childElement.nextSiblingElement();

            QString tagName = childElement.tagName();
            assert(map.values().contains(tagName));
            if(tagName != "br")
            {
                if(childElement.childNodes().size() == 0)
                {
                    element.removeChild(childElement);
                }
            }

            childElement = nextChildElement;
        }
        allChunks.append(currentChunk);
        currentChunk = 0;

        //        qDebug() << currentChunk->bookNumber << ", " << currentChunk->chapter << ", " << currentChunk->verse;
        //        qDebug() << currentChunk->xmlDoc.toString(-1);
        //        delete currentChunk;
        //        currentChunk = 0;
    }
}

void writeOutChunk(QSqlQuery query, Chunk* chunk)
{
    query.prepare("insert into bibles values(:id, :bibletext_id, :book_number, :normalised_chapter, :chapter, :verse, :text, :parallel)");

    query.bindValue(":id", QVariant(QVariant::Int));
    query.bindValue(":bibletext_id", 1);
    query.bindValue(":book_number", chunk->bookNumber);
    query.bindValue(":normalised_chapter", chunk->normalisedChapter);
    query.bindValue(":chapter", chunk->chapter);
    query.bindValue(":verse", chunk->verse);
    query.bindValue(":text", chunk->xmlDoc.toString(-1));
    query.bindValue(":parallel",  QVariant(QVariant::Int));


    if(!query.exec())
    {
        qDebug() << "failed: "<< query.lastError()  << endl;
        exit(1);
    }

}

void writeOutAllChunks(QSqlDatabase db, QSqlQuery query)
{
    for(int i=0; i<allChunks.size(); i++)
    {
        Chunk* chunk = allChunks.at(i);
        writeOutChunk(query, chunk);
        if(i%1000 == 0)
        {
            db.commit();
            db.transaction();
        }
    }
}

void putInChapter(Chunk* chunk)
{
    assert(chunk->xmlDoc.firstChild().firstChild().isElement());

    QList<QString> acceptableStarts;
    acceptableStarts.append(getDatabaseTagName("bodytext"));
    acceptableStarts.append(getDatabaseTagName("poetry"));
    acceptableStarts.append(getDatabaseTagName("bodyblock"));
    acceptableStarts.append(getDatabaseTagName("otpoetry"));
    acceptableStarts.append(getDatabaseTagName("BlockQuote"));

    QString tagName = chunk->xmlDoc.firstChild().firstChildElement().tagName();
    assert(acceptableStarts.contains(tagName));

    QDomElement newChild = chunk->xmlDoc.createElement(getDatabaseTagName("chapter"));
    newChild.appendChild(chunk->xmlDoc.createTextNode(QString().setNum(chunk->chapter)));

    QDomNode refChild = chunk->xmlDoc.firstChild().firstChildElement().firstChild();
    chunk->xmlDoc.firstChild().firstChildElement().insertBefore(newChild, refChild);
}

void putInVerse(Chunk* chunk)
{
    if(!chunk->xmlDoc.firstChild().firstChild().isElement())
    {
        int x = 0;
    }

    QList<QString> acceptableStarts;
    acceptableStarts.append(getDatabaseTagName("bodytext"));
    acceptableStarts.append(getDatabaseTagName("poetry"));
    acceptableStarts.append(getDatabaseTagName("bodyblock"));
    acceptableStarts.append(getDatabaseTagName("otpoetry"));
    acceptableStarts.append(getDatabaseTagName("BlockQuote"));

    QString tagName = chunk->xmlDoc.firstChild().firstChildElement().tagName();

    if(!acceptableStarts.contains(tagName))
    {
        int x = 0;
    }

    QDomElement newChild = chunk->xmlDoc.createElement(getDatabaseTagName("verse"));
    newChild.appendChild(chunk->xmlDoc.createTextNode(QString().setNum(chunk->verse)));

    QDomNode refChild = chunk->xmlDoc.firstChild().firstChildElement().firstChild();
    chunk->xmlDoc.firstChild().firstChildElement().insertBefore(newChild, refChild);

}

void setNormalisedChapterFieldAndPutInChaptersAndVerses()
{
    int currentNormalisedChapter = 0;
    int currentChapter = 0;
    int currentBook = 0;

    for(int i=0; i<allChunks.size(); i++)
    {
        Chunk* chunk = allChunks.at(i);
        if(chunk->chapter > 0)
        {
            putInVerse(chunk);

            if(chunk->bookNumber != currentBook || chunk->chapter != currentChapter)
            {
                currentBook = chunk->bookNumber;
                currentChapter = chunk->chapter;

                putInChapter(chunk);

                currentNormalisedChapter++;
            }
            chunk->normalisedChapter = currentNormalisedChapter;
        }
    }

    for(int i=0; i<allChunks.size(); i++)
    {
        Chunk* chunk = allChunks.at(i);
        if(chunk->normalisedChapter == 0)
        {
            int j = i+1;
            while(allChunks.at(j)->normalisedChapter == 0)
            {
                j++;
            }
            chunk->normalisedChapter = allChunks.at(j)->normalisedChapter;
        }
    }
}


QDomElement createNoteElement(QString name)
{
    return currentNote->xmlDoc.createElement(name);
}

QDomText createNoteTextNode(QString data)
{
    return currentNote->xmlDoc.createTextNode(data);
}

QDomElement createElement(QString name)
{
    return currentChunk->xmlDoc.createElement(name);
}

QDomText createTextNode(QString data)
{
    return currentChunk->xmlDoc.createTextNode(data);
}

void buildTitleChunk(QDomNode newTreeParent, QDomNode oldTreeParent)
{
    for(int i=0; i<oldTreeParent.childNodes().size(); i++)
    {
        QDomNode oldChild = oldTreeParent.childNodes().at(i);

        if(oldChild.isText())
        {
            QDomText newChild = createTextNode(oldChild.toText().data());
            newTreeParent.appendChild(newChild);
            assert(oldChild.childNodes().size(), 0);
        }
        else if(oldChild.isElement())
        {
            QDomElement oldChildElement = oldChild.toElement();
            if(oldChildElement.tagName() == "a")
            {
                if(!(LIMIT_NET_NOTES && currentChunk->chapter > 1))
                {
                    assert(oldChildElement.attribute("TARGET"), "note_pane");
                    assert(oldChildElement.childNodes().size(), 1);
                    assert(oldChildElement.firstChildElement().tagName(), "SUP");
                    assert(oldChildElement.firstChildElement().childNodes().size(), 1);

                    QDomElement newChild = createElement(getDatabaseTagName("netNote"));

                    newTreeParent.appendChild(newChild);

                    QString stringId = oldChildElement.attribute("href");

                    if(!stringId.contains("#note"))
                    {
                        stringId.replace("#","#note");
                    }

                    if(!noteIdMap.contains(stringId))
                        qDebug() << "Title note ID not found: " << stringId;
                    int noteId = noteIdMap.value(stringId);

                    noteIdMap.remove(stringId);

                    newChild.setAttribute("id", noteId);

                    QDomText newChildChild = createTextNode(QString().setNum(netNoteChapterCount));
                    netNoteChapterCount++;
                    newChild.appendChild(newChildChild);
                }
            }
            else if(oldChildElement.tagName() == "span")
            {
                assert(oldChildElement.attribute("class") == "smallcaps");
                QDomElement newChild = createElement(getDatabaseTagName("smallcaps"));
                newTreeParent.appendChild(newChild);
                buildTitleChunk(newChild, oldChild);
            }
            else if(oldChildElement.tagName() == "b")
            {
                QDomElement newChild = createElement(getDatabaseTagName("b"));
                newTreeParent.appendChild(newChild);
                buildTitleChunk(newChild, oldChild);
            }
            else if(oldChildElement.tagName() == "br")
            {
                QDomElement newChild = createElement(getDatabaseTagName("br"));
                newTreeParent.appendChild(newChild);
                buildTitleChunk(newChild, oldChild);
            }
            else if (oldChildElement.tagName() == "i")
            {
                QDomElement newChild = createElement(getDatabaseTagName("i"));
                newTreeParent.appendChild(newChild);
                buildTitleChunk(newChild, oldChild);
            }
            else if (oldChildElement.tagName() == "font")
            {
                assert(oldChildElement.attribute("face"), "Galaxie Unicode Hebrew");
                QDomElement newChild = createElement(getDatabaseTagName("hebrew"));
                newTreeParent.appendChild(newChild);
                buildTitleChunk(newChild, oldChild);
            }
            else
            {
                qDebug() << "unknown element tag " << oldChildElement.tagName();
                exit(1);
            }
        }
        else
        {
            qDebug() << "not text or element";
            exit(1);
        }
    }
}

void buildVerseChunk(QString paragraphClass, QDomNode oldTreeParent)
{    
//    static bool logging = false;
//    if(currentChunk != 0 && currentChunk->bookNumber == 40 && currentChunk->chapter == 2 && currentChunk->verse == 6)
//    {
//        logging = true;
//    }
//    if(logging)
//    {
//        qDebug() << oldTreeParent.childNodes().size();
//    }

    for(int i=0; i<oldTreeParent.childNodes().size(); i++)
    {
        QDomNode oldChild = oldTreeParent.childNodes().at(i);

        if(oldChild.isText())
        {

            QString text = oldChild.toText().data();
//            if(logging)
//                qDebug() << "Text: " <<  text;
            QDomText newChildText = createTextNode(oldChild.toText().data());
            placeToAdd->appendChild(newChildText);
            assert(oldChild.childNodes().size(), 0);
        }
        else if(oldChild.isElement())
        {
            QDomElement oldChildElement = oldChild.toElement();
            if(oldChildElement.tagName() == "a")
            {

                if(oldChildElement.attribute("name").length() > 0)
                {
                    assert(oldChildElement.childNodes().size(), 0);
                }
                else
                {
                    if(!(LIMIT_NET_NOTES && currentChunk->chapter > 1 && currentChunk->verse > 3))
                    {
                        assert(oldChildElement.attribute("TARGET"), "note_pane");
                        assert(oldChildElement.childNodes().size(), 1);
                        assert(oldChildElement.firstChildElement().tagName(), "SUP");
                        assert(oldChildElement.firstChildElement().childNodes().size(), 1);
                        QDomElement newChild = createElement(getDatabaseTagName("netNote"));
                        placeToAdd->appendChild(newChild);

                        QString stringId = oldChildElement.attribute("href");

                        if(!stringId.contains("#note"))
                        {
                            stringId.replace("#","#note");
                        }

                        if(!noteIdMap.contains(stringId))
                            qDebug() << "Verse note ID not found: " << stringId;

                        int noteId = noteIdMap.value(stringId);
                        noteIdMap.remove(stringId);

                        newChild.setAttribute("id", noteId);
                        QDomText newChildChild = createTextNode(QString().setNum(netNoteChapterCount));
                        netNoteChapterCount++;
                        newChild.appendChild(newChildChild);
                    }
                }
            }
            else if(oldChildElement.tagName() == "span")
            {
                if(oldChildElement.attribute("class") == "versenum")
                {
                    if(SHOULD_NOT_START_NEW_CHUNK)
                    {
                        qDebug() << "violation of new chunk boundary";
                        qDebug() << currentChunk->bookNumber << " " <<currentChunk->chapter << ":" << currentChunk->verse;
                    }
                    appendCurrentChunk();
                    assert(oldChildElement.childNodes().size(), 1);
                    assert(oldChildElement.firstChild().isText());
                    QString rawVerse = oldChildElement.firstChild().toText().data();
                    QStringList verseList = rawVerse.split(":");


                    currentChunk = new Chunk(bookNumber, ((QString)verseList.at(0)).toInt(), ((QString)verseList.at(1)).toInt());
                    QDomElement newChildElement = createElement(getDatabaseTagName(paragraphClass));
                    currentChunk->xmlDoc.firstChild().appendChild(newChildElement);
//                    QDomNode tempPlaceToAdd = currentChunk->xmlDoc.firstChild().lastChild();
//                    placeToAdd = &tempPlaceToAdd;
                    placeToAdd = &(currentChunk->xmlDoc.firstChild().lastChild());
//                    placeToAdd = &newChildElement;
//                    const QDomNode& tempRef = currentChunk->xmlDoc.firstChild().lastChild();

                }
                else
                {
                    assert(oldChildElement.attribute("class") == "smallcaps");
                    QDomElement newChild = createElement(getDatabaseTagName("smallcaps"));
                    placeToAdd->appendChild(newChild);
                    QDomNode* temp = placeToAdd;
                    placeToAdd = &newChild;
                    SHOULD_NOT_START_NEW_CHUNK = true;
                    buildVerseChunk(paragraphClass, oldChild);
                    SHOULD_NOT_START_NEW_CHUNK = false;
                    placeToAdd = temp;
                }
            }
            else if(oldChildElement.tagName() == "b")
            {
                QDomElement newChild = createElement(getDatabaseTagName("b"));
                placeToAdd->appendChild(newChild);
                QDomNode* temp = placeToAdd;
                placeToAdd = &newChild;
                SHOULD_NOT_START_NEW_CHUNK = true;
                buildVerseChunk(paragraphClass, oldChild);
                SHOULD_NOT_START_NEW_CHUNK = false;
                placeToAdd = temp;
            }
            else if(oldChildElement.tagName() == "br")
            {
                QDomElement newChild = createElement(getDatabaseTagName("br"));
                placeToAdd->appendChild(newChild);
                QDomNode* temp = placeToAdd;
                placeToAdd = &newChild;
                SHOULD_NOT_START_NEW_CHUNK = true;
                buildVerseChunk(paragraphClass, oldChild);
                SHOULD_NOT_START_NEW_CHUNK = false;
                placeToAdd = temp;
            }
            else if (oldChildElement.tagName() == "i")
            {
                QDomElement newChild = createElement(getDatabaseTagName("i"));
                placeToAdd->appendChild(newChild);
                QDomNode* temp = placeToAdd;
                placeToAdd = &newChild;
                SHOULD_NOT_START_NEW_CHUNK = true;
                buildVerseChunk(paragraphClass, oldChild);
                SHOULD_NOT_START_NEW_CHUNK = false;
                placeToAdd = temp;
            }
            else
            {
                qDebug() << "unknown element tag " << oldChildElement.tagName();
                exit(1);
            }
        }
        else
        {
            qDebug() << "not text or element";
            exit(1);
        }
    }
}

bool containsVersenum(QDomElement element)
{
    if(element.tagName() == "span" && element.attribute("class") == "versenum")
        return true;
    else
    {
        for(int i=0; i<element.childNodes().size(); i++)
        {
            QDomNode childNode = element.childNodes().at(i);
            if(childNode.isElement() && containsVersenum(childNode.toElement()))
                return true;
        }
    }
    return false;
}

void doParagraph(QDomElement paragraphElement)
{
    bool specialCase = (bookNumber == 5 && currentChunk!=0 && currentChunk->chapter == 33 && currentChunk->verse == 2)
                       || (bookNumber == 8 && currentChunk!=0 && currentChunk->chapter == 1 && currentChunk->verse == 19)
                       || (bookNumber == 8 && currentChunk!=0 && currentChunk->chapter == 3 && currentChunk->verse == 16)
                       || (bookNumber == 9 && currentChunk!=0 && currentChunk->chapter == 4 && currentChunk->verse == 1)
                       || (bookNumber == 9 && currentChunk!=0 && currentChunk->chapter == 20 && currentChunk->verse == 42)
                       || (bookNumber == 10 && currentChunk!=0 && currentChunk->chapter == 19 && currentChunk->verse == 8)
                       || (bookNumber == 12 && currentChunk!=0 && currentChunk->chapter == 9 && currentChunk->verse == 14)
                       || (bookNumber == 13 && currentChunk!=0 && currentChunk->chapter == 1 && currentChunk->verse == 29)
                       || (bookNumber == 13 && currentChunk!=0 && currentChunk->chapter == 1 && currentChunk->verse == 51)
                       || (bookNumber == 13 && currentChunk!=0 && currentChunk->chapter == 9 && currentChunk->verse == 1)
                       || (bookNumber == 16 && currentChunk!=0 && currentChunk->chapter == 7 && currentChunk->verse == 73)
                       || (bookNumber == 24 && currentChunk!=0 && currentChunk->chapter == 38 && currentChunk->verse == 28)
                       || (bookNumber == 25 && currentChunk!=0 && currentChunk->chapter == 1 && currentChunk->verse == 11)
                       || (bookNumber == 27 && currentChunk!=0 && currentChunk->chapter == 11 && currentChunk->verse == 2)
                       || (bookNumber == 28 && currentChunk!=0 && currentChunk->chapter == 6 && currentChunk->verse == 11)
                       || (bookNumber == 28 && currentChunk!=0 && currentChunk->chapter == 9 && currentChunk->verse == 7)
                       || (bookNumber == 31 && currentChunk!=0 && currentChunk->chapter == 1 && currentChunk->verse == 1)
                       || (bookNumber == 34 && currentChunk!=0 && currentChunk->chapter == 1 && currentChunk->verse == 3)
                       || (bookNumber == 34 && currentChunk!=0 && currentChunk->chapter == 3 && currentChunk->verse == 15)
                       || (bookNumber == 42 && currentChunk!=0 && currentChunk->chapter == 9 && currentChunk->verse == 43)
                       || (bookNumber == 43 && currentChunk!=0 && currentChunk->chapter == 19 && currentChunk->verse == 16)
                       || (bookNumber == 44 && currentChunk!=0 && currentChunk->chapter == 8 && currentChunk->verse == 1)
                       || (bookNumber == 54 && currentChunk!=0 && currentChunk->chapter == 6 && currentChunk->verse == 2)
                       || (bookNumber == 62 && currentChunk!=0 && currentChunk->chapter == 5 && currentChunk->verse == 4);

    QString paragraphClass = paragraphElement.attribute("class");
    if( (paragraphClass == "paragraphtitle" && !(specialCase))
        || paragraphClass == "section"
                || paragraphClass == "psasuper"
                || paragraphClass == "lamhebrew" ) //e.g. see Ps 119
        {

        appendCurrentChunk();

        currentChunk = new Chunk(bookNumber);
        QDomNode newElement = createElement(getDatabaseTagName(paragraphClass));
        currentChunk->xmlDoc.firstChild().appendChild(newElement);
        buildTitleChunk(newElement, paragraphElement);

        appendCurrentChunk();
    }
    else if(paragraphClass == "bodytext"
            || paragraphClass == "poetry"
            || paragraphClass == "bodyblock"
            || paragraphClass == "otpoetry"
            || paragraphClass == "BlockQuote")
    {
        if(paragraphElement.firstChild().isText())
        {
            assert(currentChunk->xmlDoc.firstChild().lastChild().toElement().tagName(), "br");
            currentChunk->xmlDoc.firstChild().removeChild(currentChunk->xmlDoc.firstChild().lastChild());
        }

        if(currentChunk != 0 )
        {
            QDomElement newElement = createElement(getDatabaseTagName(paragraphClass));
            currentChunk->xmlDoc.firstChild().appendChild(newElement);
//            QDomNode tempPlaceToAdd = currentChunk->xmlDoc.firstChild().lastChild();
//            placeToAdd = &tempPlaceToAdd;
            placeToAdd = &currentChunk->xmlDoc.firstChild().lastChild();
//            placeToAdd = &newElement;
        }
        else
            placeToAdd = 0;

        buildVerseChunk(paragraphClass, paragraphElement);

        currentChunk->xmlDoc.firstChild().appendChild(createElement(getDatabaseTagName("br")));
        currentChunk->xmlDoc.firstChild().appendChild(createElement(getDatabaseTagName("br")));
    }
    else if(paragraphClass == "paragraphtitle") //this is for the special cases
    {
        assert(currentChunk->xmlDoc.firstChild().lastChild().toElement().tagName(), "br");
        currentChunk->xmlDoc.firstChild().removeChild(currentChunk->xmlDoc.firstChild().lastChild());

        QDomNode newElement = createElement(getDatabaseTagName(paragraphClass));
        currentChunk->xmlDoc.firstChild().appendChild(newElement);
        QDomText text = createTextNode(paragraphElement.firstChild().toText().data());
        newElement.appendChild(text);
        currentChunk->xmlDoc.firstChild().appendChild(createElement(getDatabaseTagName("br")));

    }
    else if(paragraphClass == "sosspeaker")
    {
        QDomElement sibling = paragraphElement.nextSiblingElement();
        assert(sibling.tagName(), "p");
        if(containsVersenum(sibling)) //it can now get its own chunk
        {
            appendCurrentChunk();
            currentChunk = new Chunk(bookNumber);


            QDomElement newElement = createElement(getDatabaseTagName(paragraphClass));
            currentChunk->xmlDoc.firstChild().appendChild(newElement);
            buildTitleChunk(newElement, paragraphElement);

            appendCurrentChunk();

        }
        else //it's in the middle of a verse so must be part of its chunk
        {

            //            assert(currentChunk->xmlDoc.firstChild().lastChild().toElement().tagName(), "br");
            //            currentChunk->xmlDoc.firstChild().removeChild(currentChunk->xmlDoc.firstChild().lastChild());

            QDomNode newElement = createElement(getDatabaseTagName(paragraphClass));
            currentChunk->xmlDoc.firstChild().appendChild(newElement);
            buildTitleChunk(newElement, paragraphElement);

            currentChunk->xmlDoc.firstChild().appendChild(createElement(getDatabaseTagName("br")));
            //            currentChunk->xmlDoc.firstChild().appendChild(createElement(getDatabaseTagName("br")));
        }
    }
    else if(paragraphClass == "")
    {
        assert(paragraphElement.childNodes().size(), 1);

        QDomText nextChapterNode = paragraphElement.firstChild().firstChild().toText();
        assert(!nextChapterNode.isNull());

        QString nextChapter = nextChapterNode.data();

        assert(nextChapter, "Next Chapter");                
    }
    else
    {
        qDebug() << "unknown paragraph type: " << paragraphElement.attribute("class");
        exit(1);
    }
}

void putNoteInDB(Note* note)
{
    QSqlQuery query;
    query.prepare("insert into net_notes values(:id, :note)");


    assert(note->xmlDoc.firstChild().nodeName(), "note");
    QDomNodeList childNodes = note->xmlDoc.firstChild().childNodes();
    for(int i=0; i<childNodes.size(); i++)
    {
        QDomNode node = childNodes.at(i);

        if(!(node.nodeName() == "translatorsNote" ||
             node.nodeName() == "studyNote" ||
             node.nodeName() == "textCriticalNote" ||
             node.nodeName() == "mapNote"))
        {
            qDebug() << note->noteId << ": not a valid note: " << node.nodeName();
        }
    }

    query.bindValue(":id", note->noteId);
    query.bindValue(":text", note->xmlDoc.toString(-1));


    if(!query.exec())
    {
        qDebug() << "failed: "<< query.lastError()  << endl;
        exit(1);
    }

}

void writeOutCurrentNote()
{
    if(currentNote != 0)
    {
        noteIdMap.insert(currentNote->stringId, currentNote->noteId);

        putNoteInDB(currentNote);

        currentNote = 0;
    }
}

//QDomElement* getNotePlaceToAdd()
//{
//    qDebug() << "in getNotePlaceToAdd:";
//    //    qDebug() << currentNote->xmlDoc.toString();

//    QDomElement* result = &currentNote->xmlDoc.firstChildElement();
//    if(result->hasChildNodes())
//        result = &result->lastChildElement();

//    if(result->isNull())
//    {
//        int x = 10;
//    }
//    return result;
//}

void buildNote(QDomNode newTreeParent, QDomNode oldTreeParent)
{
    for(int i=0; i<oldTreeParent.childNodes().size(); i++)
    {
        QDomNode oldChild = oldTreeParent.childNodes().at(i);

        if(oldChild.isText())
        {
            QDomText newChildText = createNoteTextNode(oldChild.toText().data());
            newTreeParent.appendChild(newChildText);
            assert(oldChild.childNodes().size(), 0);
        }
        else if(oldChild.isElement())
        {
            QDomElement oldChildElement = oldChild.toElement();

            if(oldChildElement.tagName() == "span")
            {
                assert(oldChildElement.childNodes().size(), 1);
                assert(oldChildElement.firstChild().isText());

                if(oldChildElement.attribute("class") == "smallcaps")
                {
                    QDomElement newChild = createNoteElement(getDatabaseTagName("smallcaps"));
                    newTreeParent.appendChild(newChild);
                    newChild.appendChild(createNoteTextNode(oldChildElement.firstChild().toText().data()));

                }
                else if (oldChildElement.attribute("class") == "fnreference")
                {

                }
                else
                {
                    qDebug() << "unknown span: " << oldChildElement.attribute("class");
                    exit(1);
                }

                //                    QDomElement newChild = createElement(getDatabaseTagName("smallcaps"));
                //                    placeToAdd->appendChild(newChild);
                //                    QDomNode* temp = placeToAdd;
                //                    placeToAdd = &newChild;
                //                    buildVerseChunk(paragraphClass, oldChild);
                //                    placeToAdd = temp;

            }
            else if(oldChildElement.tagName() == "b")
            {
                assert(oldChildElement.childNodes().size(), 1);
                assert(oldChildElement.firstChild().isText());

                QString test = oldChildElement.firstChild().toText().data();
                test = test.trimmed();
                if(test == "tn")
                {
                    QDomElement newChild = createNoteElement(getDatabaseTagName("translatorsNote"));
                    currentNote->xmlDoc.firstChildElement().appendChild(newChild);
                    //                    notePlaceToAdd = &newChild;

                    newTreeParent = newChild;

                    QDomElement boldChild = createNoteElement(getDatabaseTagName("heading"));
                    newChild.appendChild(boldChild);

                    boldChild.appendChild(createNoteTextNode("Translator's Note"));

                }
                else if(test == "sn")
                {
                    QDomElement newChild = createNoteElement(getDatabaseTagName("studyNote"));
                    currentNote->xmlDoc.firstChildElement().appendChild(newChild);
                    //                    notePlaceToAdd = &newChild;

                    newTreeParent = newChild;

                    QDomElement boldChild = createNoteElement(getDatabaseTagName("heading"));
                    newChild.appendChild(boldChild);

                    boldChild.appendChild(createNoteTextNode("Study Note"));

                }
                else if(test == "tc")
                {
                    QDomElement newChild = createNoteElement(getDatabaseTagName("textCriticalNote"));
                    currentNote->xmlDoc.firstChildElement().appendChild(newChild);
                    //                    notePlaceToAdd = &newChild;

                    newTreeParent = newChild;

                    QDomElement boldChild = createNoteElement(getDatabaseTagName("heading"));
                    newChild.appendChild(boldChild);

                    boldChild.appendChild(createNoteTextNode("Text-Critical Note"));

                }
                else if(test == "map")
                {
                    QDomElement newChild = createNoteElement(getDatabaseTagName("mapNote"));
                    currentNote->xmlDoc.firstChildElement().appendChild(newChild);
                    //                    notePlaceToAdd = &newChild;

                    newTreeParent = newChild;

                    QDomElement boldChild = createNoteElement(getDatabaseTagName("heading"));
                    newChild.appendChild(boldChild);

                    boldChild.appendChild(createNoteTextNode("Map Note"));

                }
                else
                {
                    QDomElement newChild = createNoteElement(getDatabaseTagName("b"));
                    newTreeParent.appendChild(newChild);
                    newChild.appendChild(createNoteTextNode(oldChildElement.firstChild().toText().data()));
                }
            }
            else if(oldChildElement.tagName() == "br")
            {
                assert(oldChildElement.childNodes().size(), 0);
                QDomElement newChild = createNoteElement(getDatabaseTagName("br"));
                newTreeParent.appendChild(newChild);
            }
            else if (oldChildElement.tagName() == "i")
            {
                //                assert(oldChildElement.childNodes().size(), 1);
                //                assert(oldChildElement.firstChild().isText());

                QDomElement newChild = createNoteElement(getDatabaseTagName("i"));
                newTreeParent.appendChild(newChild);
                buildNote(newChild, oldChildElement);
            }
            else if (oldChildElement.tagName() == "sup")
            {
                //                assert(oldChildElement.childNodes().size(), 1);
                //                assert(oldChildElement.firstChild().isText());

                QDomElement newChild = createNoteElement(getDatabaseTagName("sup"));
                newTreeParent.appendChild(newChild);
                buildNote(newChild, oldChildElement);
            }
            else if (oldChildElement.tagName() == "font")
            {
                if(oldChildElement.attribute("face") == "Greek Uncials")
                {
                    buildNote(newTreeParent, oldChildElement);
                }
                else
                {
                    QDomElement newChild;
                    if(oldChildElement.attribute("face") =="Galaxie Unicode Hebrew")
                    {
                        newChild = createNoteElement(getDatabaseTagName("hebrew"));
                    }
                    else if(oldChildElement.attribute("face") == "Galaxie Unicode Greek")
                    {
                        newChild = createNoteElement(getDatabaseTagName("greek"));
                    }
                    else if(oldChildElement.attribute("face") =="Scholar" || oldChildElement.attribute("face") == "Greek" || oldChildElement.attribute("face") == "Greektl")
                    {
                        newChild = createNoteElement(getDatabaseTagName("i"));

                    }
                    else
                    {
                        qDebug() << "unknown font face "<< oldChildElement.attribute("face");
                        exit(1);
                    }
                    newTreeParent.appendChild(newChild);
                    buildNote(newChild, oldChildElement);
                }
            }
            else
            {
                qDebug() << "unknown element tag " << oldChildElement.tagName();
                exit(1);
            }
        }
        else
        {
            qDebug() << "not text or element";
            exit(1);
        }
    }
}

void doNoteParagraph(QString noteChapterFilename, QDomElement paragraphElement)
{
    assert(paragraphElement.attribute("class"), "footnote");

    QDomElement firstChild = paragraphElement.firstChildElement();
    if(firstChild.nodeName() == "span")
    {
        assert(firstChild.attribute("class"), "fnreference");

        writeOutCurrentNote();

        noteId++;
        QString localNumber = firstChild.firstChild().toText().data();
        currentNote = new Note(noteId, noteChapterFilename + "#" + localNumber);

    }
    //    notePlaceToAdd = &(currentNote->xmlDoc.firstChild());

    buildNote(currentNote->xmlDoc.firstChild(), paragraphElement);
}

void importNoteChapter(QString chapterFilenameWithoutFiletype)
{
    QString noteChapterFilename =  chapterFilenameWithoutFiletype + "_notes.htm";

    QDomDocument doc("mydocument");
    QFile file("netbible/" + noteChapterFilename);
    if (!file.open(QIODevice::ReadOnly))
        exit(1);

    QByteArray byteArray = file.readAll();

    QString wholeFile = QString::fromLatin1(byteArray.data());

    if(noteChapterFilename == "gal6_notes.htm")
    {
        wholeFile.replace("<note=301>tn","<note=301><b>tn</b>");
    }

    wholeFile.replace("<LINK REL=StyleSheet HREF=\"style.css\" TYPE=\"text/css\" MEDIA=screen>", "");

    wholeFile.replace(QRegExp("<note=[0-9]*>"), "");

    wholeFile.replace(QRegExp("<A NAME=\"([0-9]*)\">"), "<a name=\"note\\1\"></a>");

    int index = 0;
    while((index = wholeFile.indexOf(QRegExp("<a name=\"note[0-9]*\"></a>"), index)) > 0)
    {
        int closeQuoteIndex = wholeFile.indexOf("\"", index + 9);
        QString noteName = wholeFile.mid(index + 9, closeQuoteIndex - index - 9);

        int spanIndex = wholeFile.indexOf("<span class=\"fnreference\">", index);
        int spanCloseIndex = wholeFile.indexOf("<", spanIndex+1);
        int lengthToReplace = spanCloseIndex - spanIndex - 26;
        wholeFile.replace(spanIndex+26, lengthToReplace, noteName);

        index++;
    }

    wholeFile.replace("<BODY BGCOLOR=\"#ddddc5\"><p>", "<body>");
    wholeFile.replace("<HTML>", "<html>");

    if(wholeFile.indexOf("</body>") == -1)
    {
        wholeFile.append("</body></html>");
    }

    if(noteChapterFilename == "gen3_notes.htm")
    {
        wholeFile.replace("</p><p class=\"footnote\">leadership", "leadership");
    }

    if(noteChapterFilename == "num23_notes.htm")
    {
        wholeFile.replace("tn tn","tn");
    }

    if(noteChapterFilename == "rut4_notes.htm")
    {
        wholeFile.replace("<sup>","");
        wholeFile.replace("</sup>","");
    }

    if(noteChapterFilename == "1sa21_notes.htm")
    {
        wholeFile.replace("sn","<b>sn</b>");
    }

    if(noteChapterFilename == "1sa23_notes.htm")
    {
        wholeFile.replace("sn ","<b>sn</b> ");
    }

    if(noteChapterFilename == "job36_notes.htm")
    {
        wholeFile.replace("<i><b>tn</b> Heb </i>","<b>tn</b> <i>Heb</i> ");
    }

    if(noteChapterFilename == "pro3_notes.htm")
    {
        wholeFile.replace("</p><p class=\"footnote\">NIV"," NIV");
    }

    if(noteChapterFilename == "sos5_notes.htm")
    {
        wholeFile.replace("construction. </p><p class=\"footnote\">","construction. ");
    }

    if(noteChapterFilename == "jer37_notes.htm")
    {
        wholeFile.replace("style.</p><p class=\"footnote\">","style. ");
    }

    if(noteChapterFilename == "jer46_notes.htm")
    {
        wholeFile.replace("</p><p class=\"footnote\">The"," The");
    }

    if(noteChapterFilename == "amo1_notes.htm")
    {
        wholeFile.replace("Ammonites, even because of four.&#8221;</p><p class=\"footnote\">",
                          "Ammonites, even because of four.&#8221;</p><p class=\"footnote\"><b>sn</b>");
    }

    if(noteChapterFilename == "luk8_notes.htm")
    {
        wholeFile.replace("</p><p class=\"footnote\">are","are");
    }

    if(noteChapterFilename == "rev12_notes.htm")
    {
        wholeFile.replace("<b>sn A</b>","<b>sn</b> A");
    }

    if(noteChapterFilename == "pro1_notes.htm"
       || noteChapterFilename == "sos4_notes.htm"
       || noteChapterFilename == "sos7_notes.htm"
       || noteChapterFilename == "jon2_notes.htm"
       || noteChapterFilename == "1ti5_notes.htm")
    {
        wholeFile.replace("<br>", "");
    }

    if(noteChapterFilename == "nah1_notes.htm")
    {
        wholeFile.replace("<br>", "<br/>");
    }

    if(noteChapterFilename == "deu9_notes.htm")
    {
        wholeFile.replace("<font face=\"Scholar\">a</font><font face=\"Hebrewtl\">r</font>", "<font face=\"Scholar\">ar</font>");
    }

    if(noteChapterFilename == "rut1_notes.htm")
    {
        wholeFile.replace("<b><font face=\"Galaxie Unicode Hebrew\">&#1499;&#1468;&#1460;&#1497;</font></b>", "<font face=\"Galaxie Unicode Hebrew\">&#1499;&#1468;&#1460;&#1497;</font>");
    }

    if(noteChapterFilename == "neh8_notes.htm")
    {
        wholeFile.replace("</font><font face=\"Hebrewtl\">y</font>", "y</font>");
    }

    if(noteChapterFilename == "job21_notes.htm")
    {
        wholeFile.replace("<b><font face=\"Galaxie Unicode Hebrew\">&#1495;&#1458;&#1489;&#1464;&#1500;&#1460;&#1497;&#1501;</font></b>", "<font face=\"Galaxie Unicode Hebrew\">&#1495;&#1458;&#1489;&#1464;&#1500;&#1460;&#1497;&#1501;</font>");
    }

    if(noteChapterFilename == "psa9_notes.htm")
    {
        wholeFile.replace("<font face=\"Hebrewtl\">/</font>", "");
    }

    if(noteChapterFilename == "pro16_notes.htm")
    {
        wholeFile.replace("(<font face=\"Hebrewtl\">z~E</font>) ", "");
    }
    //    if(noteChapterFilename == "pro4_notes.htm")
    //    {
    wholeFile.replace("<b><font", "<font");
    wholeFile.replace("</font></b>", "</font>");
    //    }

    wholeFile.replace("<i>NBD<sup>3</sup></i>", "<i>NBD</i><sup>3</sup>");//yes, I'm hacking
    wholeFile.replace("<b><i>and</i></b>", "<b>and</b>");

    wholeFile.replace("<b><i> </i></b>", " ");

    wholeFile.replace("<i> </i>", " ");

    wholeFile.replace("<b><i>tn</i></b>", "<b>tn</b>");
    wholeFile.replace("<b><i>sn</i></b>", "<b>sn</b>");



    if(noteChapterFilename == "lev16_notes.htm")
    {
        wholeFile.replace("<i><font face=\"Galaxie Unicode Hebrew\">&#1490;&#1468;&#1464;&#1494;&#1468;&#1461;&#1512;</font> </i>", "<font face=\"Galaxie Unicode Hebrew\">&#1490;&#1468;&#1464;&#1494;&#1468;&#1461;&#1512;</font> ");
    }

    //    if(noteChapterFilename == "eze5_notes.htm")
    //    {
    wholeFile.replace("&#8216;<font face=\"Hebrewtl\">h!nn#n' ?l?K&gt;</font>,&#8217;", "");
    //    }

    if(noteChapterFilename == "eze13_notes.htm" || noteChapterFilename == "eze30_notes.htm")
    {
        wholeFile.replace("Hebrewtl", "Scholar");
    }

    wholeFile.replace("<font face=\"Scholar\"> </font>", " ");
    wholeFile.replace("<font face=\"Hebrewtl\"> </font>", " ");
    wholeFile.replace("<font face=\"Galaxie Unicode Hebrew\"> </font>", " ");
    wholeFile.replace("<font face=\"Galaxie Unicode Greek\"> </font>", " ");

    wholeFile.replace("<span class=\"smallcaps\"> </span>", " ");
    wholeFile.replace("<sup> </sup>", " ");

    wholeFile.replace("<b>,</b>", ",");


    QString errorMsg;
    int errorLine;
    int errorColumn;
    if(!doc.setContent(wholeFile, &errorMsg , &errorLine, &errorColumn))
    {
        file.close();

        qDebug() << "xml not well formed:";
        qDebug() << noteChapterFilename;
        qDebug() << errorMsg;
        qDebug() << errorLine;
        qDebug() << errorColumn;
        exit(1);
    }

    QDomNodeList paragraphs = doc.elementsByTagName("p");

    for(int i=0; i<paragraphs.size(); i++)
    {
        //        qDebug() << "para: " << i+1;
        QDomElement paragraph = paragraphs.at(i).toElement();
        doNoteParagraph(noteChapterFilename, paragraph);
    }

    writeOutCurrentNote();
}

void importBibleChapter(QString baseBookName, QString chapterFilenameWithoutFiletype)
{
    netNoteChapterCount = 1;

    QString chapterFilename = chapterFilenameWithoutFiletype + ".htm";

    QDomDocument doc("mydocument");
    QFile file("netbible/" + chapterFilename);
    if (!file.open(QIODevice::ReadOnly))
        exit(1);

    QByteArray byteArray = file.readAll();

    QString wholeFile = QString::fromLatin1(byteArray.data());

    wholeFile.replace("<b><i> </i></b>", " ");

    if(chapterFilename == "gen50.htm")
    {
        wholeFile.replace("<i> </i>", " ");
    }

    if(chapterFilename == "exo20.htm")
    {
        wholeFile.replace("<p class=\"bodytext\"><b>","<p class=\"bodytext\">");
        wholeFile.replace("20:8</span> &#8220;</b>","20:8</span> &#8220;");
    }

    if(chapterFilename == "exo21.htm")
    {
        wholeFile.replace("<p class=\"bodytext\"><b>","<p class=\"bodytext\">");
        wholeFile.replace("21:22</span> &#8220;</b>","21:22</span> &#8220;");
    }

    if(chapterFilename == "exo22.htm")
    {
        wholeFile.replace("<p class=\"bodytext\"><b>","<p class=\"bodytext\">");
        wholeFile.replace("22:5</span> &#8220;</b>","22:5</span> &#8220;");
        wholeFile.replace("22:7</span> &#8220;</b>","22:7</span> &#8220;");
        wholeFile.replace("22:18</span> &#8220;</b>","22:18</span> &#8220;");
        wholeFile.replace("22:21</span> &#8220;Y</b>","22:21</span> &#8220;Y");
    }

    if(chapterFilename == "exo25.htm")
    {
        wholeFile.replace("<p class=\"bodytext\"><b>","<p class=\"bodytext\">");
        wholeFile.replace("25:17</span> &#8220;</b>","25:17</span> &#8220;");
    }

    if(chapterFilename == "exo26.htm")
    {
        wholeFile.replace("<p class=\"bodytext\"><b>","<p class=\"bodytext\">");
        wholeFile.replace("26:36</span> &#8220;</b>","26:36</span> &#8220;");
    }

    if(chapterFilename == "exo29.htm")
    {
        wholeFile.replace("<p class=\"bodytext\"><b>","<p class=\"bodytext\">");
        wholeFile.replace("29:15</span> &#8220;</b>","29:15</span> &#8220;");
    }

    if(chapterFilename == "num7.htm")
    {
        wholeFile.replace("<b>;",";");
        wholeFile.replace("7:65</span></b>","7:65</span>");
        wholeFile.replace("7:71</span></b>","7:71</span>");
    }

    if(chapterFilename == "psa119.htm")
    {
        wholeFile.replace("<b><font face=\"Galaxie Unicode Hebrew\">&#1465;</font>119:113</b>",
                          "<a name=\"Ps 119:113\"></a><span class=\"versenum\">119:113</span>");
    }

    if(chapterFilename == "sos1.htm")
    {
        wholeFile.replace("bring",
                          " bring");
    }

    if(chapterFilename == "1sa20.htm")
    {
        wholeFile.replace("&#8217;&#8221;</p>",
                          "&#8217;&#8221;</p><p class=\"paragraphtitle\">David Goes to Nob</p><p class=\"bodytext\"><b>(21:1)</b><a href=\"1sa21_notes.htm#note1\" TARGET=\"note_pane\"><SUP>1</SUP></A> Then David<a href=\"1sa21_notes.htm#note2\" TARGET=\"note_pane\"><SUP>2</SUP></A> got up and left, while Jonathan went back to the city. </p>");
    }

    if(chapterFilename == "1sa21.htm")
    {
        wholeFile.replace("<p class=\"paragraphtitle\">David Goes to Nob</p><p class=\"bodytext\"><b>(21:1)</b><a href=\"1sa21_notes.htm#note1\" TARGET=\"note_pane\"><SUP>1</SUP></A> Then David<a href=\"1sa21_notes.htm#note2\" TARGET=\"note_pane\"><SUP>2</SUP></A> got up and left, while Jonathan went back to the city. ",
                          "<p class=\"bodytext\">");

        wholeFile.replace("<b>(21:2)</b> ",
                          "");
    }

    if(chapterFilename == "neh7.htm")
    {
        wholeFile.replace("cities.</p>",
                          "cities.</p><p class=\"paragraphtitle\">The People Respond to the Reading of the Law</p><p class=\"bodytext\">When the seventh month arrived and the Israelites<a href=\"neh8_notes.htm#note1\" TARGET=\"note_pane\"><SUP>1</SUP></A> were settled in their cities,<a href=\"neh8_notes.htm#note2\" TARGET=\"note_pane\"><SUP>2</SUP></A> </p>");
    }

    if(chapterFilename == "neh8.htm")
    {
        wholeFile.replace("<p class=\"paragraphtitle\">The People Respond to the Reading of the Law</p><p class=\"bodytext\">When the seventh month arrived and the Israelites<a href=\"neh8_notes.htm#note1\" TARGET=\"note_pane\"><SUP>1</SUP></A> were settled in their cities,<a href=\"neh8_notes.htm#note2\" TARGET=\"note_pane\"><SUP>2</SUP></A> ",
                          "<p class=\"bodytext\">");
    }

    if(chapterFilename == "sos1.htm")
    {
        wholeFile.replace("<p class=\"poetry\"><b><i>The Beloved to Her Lover:</i></b></p>",
                          "<p class=\"sosspeaker\"><b><i>The Beloved to Her Lover:</i></b></p>");
    }

    if(chapterFilename == "isa64.htm")
    {
        wholeFile.replace("64:1</span> (",
                          "64:1</span>");

        wholeFile.replace("<a name=\"Is 63:19\"></a><span class=\"versenum\">63:19</span>b)",
                          "<b>(63:19b)</b>");
    }

    if(chapterFilename == "ecc2.htm"
       || chapterFilename == "2sa16.htm"
       || chapterFilename == "mat2.htm"
       || chapterFilename == "mat3.htm"
       || chapterFilename == "mat11.htm"
       || chapterFilename == "mat21.htm"
       || chapterFilename == "luk4.htm"
       || chapterFilename == "luk20.htm"
       || chapterFilename == "joh12.htm"
       || chapterFilename == "act2.htm"
       || chapterFilename == "rom3.htm"
       || chapterFilename == "rom11.htm"
       || chapterFilename == "heb1.htm"
       || chapterFilename == "rev18.htm")
    {
        wholeFile.replace("<i> </i>", "");
    }

    if(chapterFilename == "act7.htm")
    {
        wholeFile.replace("</b> <b>","</b> ");
        wholeFile.replace("7:43</span> ","7:43</span> <b>");
    }

    if(chapterFilename == "2sa3.htm"
       || chapterFilename == "psa7.htm"
       || chapterFilename == "mar14.htm"
       || chapterFilename == "luk22.htm"
       || chapterFilename == "luk23.htm"
       || chapterFilename == "rom9.htm"
       || chapterFilename == "rom10.htm"
       || chapterFilename == "col1.htm"
       || chapterFilename == "1jo2.htm")
    {
        wholeFile.replace("<i> </i>", " ");
    }

    if(chapterFilename == "luk22.htm")
    {
        wholeFile.replace("<b>[","");
        wholeFile.replace("22:43</span></b>","22:43</span>[");
    }

    if(chapterFilename == "jer32.htm")
    {
        wholeFile.replace("<b>32:2 </b>",
                          "<a name=\"Je 32:2\"></a><span class=\"versenum\">32:2</span> ");
    }
    if(chapterFilename == "jon2.htm")
    {
        wholeFile.replace("<b>1:17 ",
                          "<a name=\"Jon 1:17\"></a><span class=\"versenum\">1:17</span><b>");
    }

    if(chapterFilename == "mar16.htm") //this should probably be better done, this is not a fault of the NET text
    {
        wholeFile.replace("<a href=\"mar16_notes.htm#note9\" TARGET=\"note_pane\"><SUP>9</SUP></A>[[",
                          "");

        wholeFile.replace("16:9</span>",
                          "16:9</span><a href=\"mar16_notes.htm#note9\" TARGET=\"note_pane\"><SUP>9</SUP></A>[[");
    }

    if(chapterFilename == "joh8.htm") //this should probably be better done, this is not a fault of the NET text
    {
        wholeFile.replace("[[",
                          "");

        wholeFile.replace(" And each",
                          "[[ And each");
    }

    if(chapterFilename == "joh9.htm") //this should probably be better done, this is not a fault of the NET text
    {
        wholeFile.replace("[",
                          "");

        wholeFile.replace("He said, &#8220;",
                          "[ He said, &#8220;");
    }

    wholeFile.replace("<HTML>", "<html>");

    wholeFile.replace("<LINK REL=StyleSheet HREF=\"style.css\" TYPE=\"text/css\" MEDIA=screen>",
                      "<link REL=\"StyleSheet\" HREF=\"style.css\" TYPE=\"text/css\" MEDIA=\"screen\"/>");

    wholeFile.replace("<p class=title", "<p class=\"title\"");

    wholeFile.replace("<p class=bodytext>", "<p class=\"bodytext\">");

    wholeFile.replace("</A>", "</a>");

    wholeFile.replace("<br>", "<br/>");

    wholeFile.replace("</body>", "</p></p></body>");

    if(wholeFile.indexOf("</body>") == -1)
    {
        wholeFile.append("</p></body></html>");
    }

    wholeFile.replace("</b> <b>", " ");

    QString errorMsg;
    int errorLine;
    int errorColumn;
    if(!doc.setContent(wholeFile, &errorMsg , &errorLine, &errorColumn))
    {
        file.close();
        qDebug() << chapterFilename;
        qDebug() << "xml not well formed";
        qDebug() << errorMsg;
        qDebug() << errorLine;
        qDebug() << errorColumn;
        exit(1);
    }

    QDomElement htmlElement = doc.firstChildElement();
    assert(htmlElement.tagName(), "html");

    QDomElement headElement = htmlElement.firstChildElement();
    assert(headElement.tagName(), "HEAD");

    QDomElement bodyElement = headElement.nextSiblingElement();
    assert(bodyElement.tagName(), "body");

    QDomElement titleElement = bodyElement.firstChildElement();
    assert(titleElement.tagName(), "p");
    assert(titleElement.attribute("class"), "title");

    QDomElement bodyTextElement = titleElement.nextSiblingElement();
    assert(bodyTextElement.tagName(), "p");
    assert(bodyTextElement.attribute("class"), "bodytext");

    QDomElement paragraphElement = bodyTextElement.firstChildElement();

    do
    {
        assert(paragraphElement.tagName(), "p");

        doParagraph(paragraphElement);

        paragraphElement = paragraphElement.nextSiblingElement();
    }
    while (!paragraphElement.isNull());

    appendCurrentChunk();

    file.close();
}

QString getBookName(int bookNumber)
{
    return books.at(bookNumber-1);
}

QList<QString> getChapterFilenames(QString fileName)
{
    QList<QString> result;

    QFile tocFile("netbible/"+fileName +"_toc.htm");
    if(!tocFile.open(QIODevice::ReadOnly))
        exit(1);

    QByteArray tocByteArray = tocFile.readAll();
    QString toc = QString::fromLatin1(tocByteArray.data());

    int start = toc.indexOf("<a href=\"");
    while(start >=0)
    {
        int endQuote = toc.indexOf("\"", start+9);
        QString chapterFilename = toc.mid(start+9,endQuote-start-9-4);

        result.append(chapterFilename);

        start = toc.indexOf("<a href=\"", start+1);

    }

    tocFile.close();

    return result;
}
void removeUnusedNotes()
{
    qDebug() << "removing unused notes";
    QSqlQuery query;
    query.prepare("delete from net_notes where id = :id");

    foreach(int noteId, noteIdMap.values())
    {
        query.bindValue(":id", noteId);

        if(!query.exec())
        {
            qDebug() << "failed: "<< query.lastError()  << endl;
            exit(1);
        }
    }
    qDebug() << "finished removing unused notes";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SHOULD_NOT_START_NEW_CHUNK = false;

    currentChunk = 0;    
    currentNote = 0;
    noteId  = 0;

    map.insert("paragraphtitle", "paragraphTitle");
    map.insert("section", "section");
    map.insert("psasuper", "psalmSuperscription");
    map.insert("lamhebrew", "hebrewParagraph");
    map.insert("sosspeaker", "speakerHeading");

    map.insert("netNote", "netNote");
    map.insert("smallcaps", "smallCaps");
    map.insert("b", "b");
    map.insert("i", "i");
    map.insert("hebrew", "hebrew");

    map.insert("bodytext", "bodyText");
    map.insert("poetry", "poetry");
    map.insert("bodyblock", "bodyBlock");
    map.insert("otpoetry", "poetry");
    map.insert("BlockQuote", "blockQuote");

    map.insert("br", "br");

    map.insert("chapter", "chapter");
    map.insert("bookName", "bookName");
    map.insert("verse", "verse");


    map.insert("translatorsNote", "translatorsNote");
    map.insert("studyNote", "studyNote");
    map.insert("textCriticalNote", "textCriticalNote");
    map.insert("mapNote", "mapNote");
    map.insert("sup", "sup");
    map.insert("greek", "greek");
    map.insert("heading", "heading");

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("verity.sqlite");
    if (!db.open())
    {
        qDebug() << "couldn't open db" << endl;
        return 1;
    }
    db.transaction();

    QSqlQuery query(db);



    books << "Genesis" << "Exodus" << "Leviticus" << "Numbers" << "Deuteronomy" << "Joshua" << "Judges" << "Ruth" << "1 Samuel" << "2 Samuel" << "1 Kings" << "2 Kings" << "1 Chronicles" << "2 Chronicles" << "Ezra" << "Nehemiah" << "Esther" << "Job" << "Psalms" << "Proverbs" << "Ecclesiastes" << "Song of Songs" << "Isaiah" << "Jeremiah" << "Lamentations" << "Ezekiel" << "Daniel" << "Hosea" << "Joel" << "Amos" << "Obadiah" << "Jonah" << "Micah" << "Nahum" << "Habakkuk" << "Zephaniah" << "Haggai" << "Zechariah" << "Malachi";
    books << "Matthew" << "Mark" << "Luke" << "John" << "Acts" << "Romans" << "1 Corinthians" << "2 Corinthians" << "Galatians" << "Ephesians" << "Philippians" << "Colossians" << "1 Thessalonians" << "2 Thessalonians" << "1 Timothy" << "2 Timothy" << "Titus" << "Philemon" << "Hebrews" << "James" << "1 Peter" << "2 Peter" << "1 John" << "2 John" << "3 John" << "Jude" << "Revelation";

    QStringList fileNames;
    //    fileNames << "gen";

    fileNames << "gen"
            << "exo"
            << "lev"
            << "num"
            << "deu"
            << "jos"
            << "jdg"
            << "rut"
            << "1sa"
            << "2sa"
            << "1ki"
            << "2ki"
            << "1ch"
            << "2ch"
            << "ezr"
            << "neh"
            << "est"
            << "job"
            << "psa"
            << "pro"
            << "ecc"
            << "sos"
            << "isa"
            << "jer"
            << "lam"
            << "eze"
            << "dan"
            << "hos"
            << "joe"
            << "amo"
            << "oba"
            << "jon"
            << "mic"
            << "nah"
            << "hab"
            << "zep"
            << "hag"
            << "zec"
            << "mal"
            << "mat"
            << "mar"
            << "luk"
            << "joh"
            << "act"
            << "rom"
            << "1co"
            << "2co"
            << "gal"
            << "eph"
            << "phi"
            << "col"
            << "1th"
            << "2th"
            << "1ti"
            << "2ti"
            << "tit"
            << "phm"
            << "heb"
            << "jam"
            << "1pe"
            << "2pe"
            << "1jo"
            << "2jo"
            << "3jo"
            << "jud"
            << "rev";



    query.exec("drop table net_notes");

    if(!query.exec("create table net_notes ( "
                   "id int, "
                   "note varchar(5000))"))
    {
        qDebug() << "failed: "<< query.lastError()  << endl;
        exit(1);
    }

    for(int i=0; i<fileNames.size(); i++)
    {
        qDebug() << "notes for: " << fileNames.at(i);
        QList<QString> chapterFilenames = getChapterFilenames(fileNames.at(i));

        foreach(QString chapterFilename, chapterFilenames)
        {
            importNoteChapter(chapterFilename);
        }
    }

    query.exec("create index idx_net_notes on net_notes (id)");
    db.commit();
    db.transaction();

    //    db.close();
    //    return 0;

    query.exec("drop table bibles");

    if(!query.exec("create table bibles (id integer primary key autoincrement, "
                   "bibletext_id int, "
                   "book_number int, normalised_chapter integer, chapter integer, "
                   "verse integer, text varchar(5000), parallel int)"))
    {
        qDebug() << "failed: "<< query.lastError()  << endl;
        exit(1);
    }

    qDebug() << "books: " << fileNames.size();
    for(int i=0; i<fileNames.size(); i++)
    {
        qDebug() << fileNames.at(i);

        bookNumber = i + 1;

        currentChunk = new Chunk(bookNumber);
        QDomNode newElement = createElement(getDatabaseTagName("bookName"));
        newElement.appendChild(createTextNode(getBookName(bookNumber)));
        currentChunk->xmlDoc.firstChild().appendChild(newElement);

        appendCurrentChunk();


        QList<QString> chapterFilenames = getChapterFilenames(fileNames.at(i));
        foreach(QString chapterFilename, chapterFilenames)
        {
            importBibleChapter(fileNames.at(i), chapterFilename);
        }
    }

    setNormalisedChapterFieldAndPutInChaptersAndVerses();

    writeOutAllChunks(db, query);

    removeUnusedNotes();

    query.exec("update bibles set parallel = id");

    query.exec("create index idx_bibles on bibles (id, bibletext_id, book_number, normalised_chapter, chapter, verse, parallel)");
    db.commit();
    db.close();

    return 0;

}
