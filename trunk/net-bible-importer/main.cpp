#include <QtCore/QCoreApplication>
#include <QtSql>
#include <QDomDocument>
#include <QDomNodeList>
#include <QDomNode>
#include <QDebug>
#include <QFile>
#include "chunk.h"

Chunk* currentChunk;
QList<Chunk*> allChunks;

QDomNode* placeToAdd = 0;

int bookNumber;


QMap<QString, QString> map;



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
    query.prepare("insert into net_bible values(:id, :book_number, :normalised_chapter, :chapter, :verse, :text, :parallel)");

    query.bindValue(":id", QVariant(QVariant::Int));
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

void setNormalisedChapterField()
{
    int currentNormalisedChapter = 0;
    int currentChapter = 0;
    int currentBook = 0;

    for(int i=0; i<allChunks.size(); i++)
    {
        Chunk* chunk = allChunks.at(i);
        if(chunk->chapter > 0)
        {
            if(chunk->bookNumber != currentBook || chunk->chapter != currentChapter)
            {
                currentBook = chunk->bookNumber;
                currentChapter = chunk->chapter;
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
                assert(oldChildElement.attribute("TARGET"), "note_pane");
                assert(oldChildElement.childNodes().size(), 1);
                assert(oldChildElement.firstChildElement().tagName(), "SUP");
                assert(oldChildElement.firstChildElement().childNodes().size(), 1);
                QDomElement newChild = createElement(getDatabaseTagName("note"));
                newTreeParent.appendChild(newChild);
                QDomText newChildChild = createTextNode(oldChildElement.attribute("href"));
                newChild.appendChild(newChildChild);                
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
    for(int i=0; i<oldTreeParent.childNodes().size(); i++)
    {
        QDomNode oldChild = oldTreeParent.childNodes().at(i);

        if(oldChild.isText())
        {
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
                    assert(oldChildElement.attribute("TARGET"), "note_pane");
                    assert(oldChildElement.childNodes().size(), 1);
                    assert(oldChildElement.firstChildElement().tagName(), "SUP");
                    assert(oldChildElement.firstChildElement().childNodes().size(), 1);
                    QDomElement newChild = createElement(getDatabaseTagName("note"));
                    placeToAdd->appendChild(newChild);
                    QDomText newChildChild = createTextNode(oldChildElement.attribute("href"));                    
                    newChild.appendChild(newChildChild);
                }
            }
            else if(oldChildElement.tagName() == "span")
            {
                if(oldChildElement.attribute("class") == "versenum")
                {
                    appendCurrentChunk();
                    assert(oldChildElement.childNodes().size(), 1);
                    assert(oldChildElement.firstChild().isText());
                    QString rawVerse = oldChildElement.firstChild().toText().data();
                    QStringList verseList = rawVerse.split(":");


                    currentChunk = new Chunk(bookNumber, ((QString)verseList.at(0)).toInt(), ((QString)verseList.at(1)).toInt());
                    QDomElement newChildElement = createElement(getDatabaseTagName(paragraphClass));
                    currentChunk->xmlDoc.firstChild().appendChild(newChildElement);
                    placeToAdd = &currentChunk->xmlDoc.firstChild().lastChild();
                }
                else
                {
                    assert(oldChildElement.attribute("class") == "smallcaps");
                    QDomElement newChild = createElement(getDatabaseTagName("smallcaps"));
                    placeToAdd->appendChild(newChild);
                    QDomNode* temp = placeToAdd;
                    placeToAdd = &newChild;
                    buildVerseChunk(paragraphClass, oldChild);
                    placeToAdd = temp;
                }
            }
            else if(oldChildElement.tagName() == "b")
            {
                QDomElement newChild = createElement(getDatabaseTagName("b"));
                placeToAdd->appendChild(newChild);
                QDomNode* temp = placeToAdd;
                placeToAdd = &newChild;
                buildVerseChunk(paragraphClass, oldChild);
                placeToAdd = temp;
            }
            else if(oldChildElement.tagName() == "br")
            {
                QDomElement newChild = createElement(getDatabaseTagName("br"));
                placeToAdd->appendChild(newChild);
                QDomNode* temp = placeToAdd;
                placeToAdd = &newChild;
                buildVerseChunk(paragraphClass, oldChild);
                placeToAdd = temp;
            }
            else if (oldChildElement.tagName() == "i")
            {
                QDomElement newChild = createElement(getDatabaseTagName("i"));
                placeToAdd->appendChild(newChild);
                QDomNode* temp = placeToAdd;
                placeToAdd = &newChild;
                buildVerseChunk(paragraphClass, oldChild);
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
            placeToAdd = &currentChunk->xmlDoc.firstChild().lastChild();
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

            assert(currentChunk->xmlDoc.firstChild().lastChild().toElement().tagName(), "br");
            currentChunk->xmlDoc.firstChild().removeChild(currentChunk->xmlDoc.firstChild().lastChild());

            QDomNode newElement = createElement(getDatabaseTagName(paragraphClass));
            currentChunk->xmlDoc.firstChild().appendChild(newElement);
            buildTitleChunk(newElement, paragraphElement);

            currentChunk->xmlDoc.firstChild().appendChild(createElement(getDatabaseTagName("br")));
            currentChunk->xmlDoc.firstChild().appendChild(createElement(getDatabaseTagName("br")));
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


void doHtm(QString baseBookName, QString chapterFilename)
{
    QDomDocument doc("mydocument");
    QFile file("netbible/" + chapterFilename);
    if (!file.open(QIODevice::ReadOnly))
        exit(1);

    QByteArray byteArray = file.readAll();

    QString wholeFile = QString::fromUtf8(byteArray.data());

    if(chapterFilename == "gen50.htm")
    {
        wholeFile.replace("<i> </i>", " ");
    }


    if(chapterFilename == "psa119.htm")
    {
        wholeFile.replace("<b><font face=\"Galaxie Unicode Hebrew\">&#1465;</font>119:113</b>",
                          "<a name=\"Ps 119:113\"></a><span class=\"versenum\">119:113</span>");
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

    QString errorMsg;
    int errorLine;
    int errorColumn;
    if(!doc.setContent(wholeFile, &errorMsg , &errorLine, &errorColumn))
    {
        file.close();
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

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    currentChunk = 0;

    map.insert("paragraphtitle", "paragraphTitle");
    map.insert("section", "section");
    map.insert("psasuper", "psalmSuperscription");
    map.insert("lamhebrew", "hebrewParagraph");
    map.insert("sosspeaker", "speakerHeading");

    map.insert("note", "note");
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

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("verity.sqlite");
    if (!db.open())
    {
        qDebug() << "couldn't open db" << endl;
        return 1;
    }
    db.transaction();

    QSqlQuery query(db);

    query.exec("drop table net_bible");
    if(!query.exec("create table net_bible (id integer primary key autoincrement, "
                   "book_number int, normalised_chapter integer, chapter integer, "
                   "verse integer, text varchar(5000), parallel int)"))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

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



    for(int i=0; i<fileNames.size(); i++)
    {
        qDebug() << fileNames.at(i);

        bookNumber = i + 1;
        QFile tocFile("netbible/"+fileNames.at(i)+"_toc.htm");
        if(!tocFile.open(QIODevice::ReadOnly))
            exit(1);

        QByteArray tocByteArray = tocFile.readAll();
        QString toc = QString::fromUtf8(tocByteArray.data());

        int start = toc.indexOf("<a href=\"");
        while(start >=0)
        {
            int endQuote = toc.indexOf("\"", start+9);
            QString chapterFilename = toc.mid(start+9,endQuote-start-9);

            //            if(i+1 >= 22)
            doHtm(fileNames.at(i), chapterFilename);

            start = toc.indexOf("<a href=\"", start+1);

        }

        tocFile.close();        
    }

    setNormalisedChapterField();

    writeOutAllChunks(db, query);

    query.exec("update net_bible set parallel = id");

    query.exec("create index idx_net_bible on net_bible (book_number, normalised_chapter, chapter, verse)");
    db.commit();
    db.close();

    return 0;

}
