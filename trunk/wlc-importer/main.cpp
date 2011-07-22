#include <QtCore/QCoreApplication>
#include <QtSql>
#include <QDomDocument>
#include <QDomNodeList>
#include <QDomNode>
#include <QDebug>
#include <QFile>
#include <QList>

int wordIdCount;

void writeOutChunk(QSqlQuery query, int bookNumber, int normalisedChapter,int chapter, int verse, QString text)
{
    query.prepare("insert into bibles values(:id, :bibletext_id, :book_number, :normalised_chapter, :chapter, :verse, :text, :parallel)");

    query.bindValue(":id", QVariant(QVariant::Int));
    query.bindValue(":bibletext_id", 3);
    query.bindValue(":book_number", bookNumber);
    query.bindValue(":normalised_chapter", normalisedChapter);
    query.bindValue(":chapter", chapter);
    query.bindValue(":verse", verse);
    query.bindValue(":text", text);
    query.bindValue(":parallel",  QVariant(QVariant::Int));

    if(!query.exec())
    {
        qDebug() << "failed: "<< query.lastError()  << endl;
        exit(1);
    }
}

void writeOutStrongsWord(QSqlQuery query, int wordId, int strongsNumber, QString strongsLemma, QString fribergLemma)
{
    query.prepare("insert into strongs_word values(:bibletext_id, :word_id, :strongsNumber, :strongsLemma, :fribergLemma)");

    query.bindValue(":bibletext_id", 3);
    query.bindValue(":word_id", wordId);
    query.bindValue(":strongsNumber", strongsNumber);
    query.bindValue(":strongsLemma", strongsLemma);
    query.bindValue(":fribergLemma", fribergLemma);

    if(!query.exec())
    {
        qDebug() << "failed: "<< query.lastError()  << endl;
        exit(1);
    }
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    wordIdCount = 1;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("verity.sqlite");
    if (!db.open())
    {
        qDebug() << "couldn't open db" << endl;
        return 1;
    }

    db.transaction();

    QSqlQuery query;
    if(!query.exec("delete from bibles where bibletext_id=3"))
    {
        qDebug() << "failed: " << query.lastError();
        exit(1);
    }

    if(!query.exec("delete from strongs_word where bibletext_id=3"))
    {
        qDebug() << "failed: " << query.lastError();
        exit(1);
    }

    QStringList standardBookNames;
    standardBookNames
            << QString::fromUtf8("בראשית")         //Genesis
            << QString::fromUtf8("שמות")           //Exodus
            << QString::fromUtf8("ויקרא" )         //Lev
            << QString::fromUtf8("במדבר" )         //Num
            << QString::fromUtf8("דברים" )         //Deut
            << QString::fromUtf8("יהושע" )         //Josh
            << QString::fromUtf8("שופטים")         //Judges
            << QString::fromUtf8("רות"   )         //Ruth
            << QString::fromUtf8("שמואל" )         //Samuel
            << QString::fromUtf8("שמואל" )         //Samuel
            << QString::fromUtf8("מלכים" )       //kings
            << QString::fromUtf8("מלכים" )       //kings
            << QString::fromUtf8("דברי הימים")     //chronicles
            << QString::fromUtf8("דברי הימים")     //chronicles
            << QString::fromUtf8("עזרא ונחמיה")    //ezra nehemiah
            << QString::fromUtf8("עזרא ונחמיה")    //ezra nehemiah
            << QString::fromUtf8("אסתר"       )    //esther
            << QString::fromUtf8("איוב"       )    //job
            << QString::fromUtf8("תהלים"      )    //psalms
            << QString::fromUtf8("משלי"       )    //proverbs
            << QString::fromUtf8("קהלת"       )    //Ecclesiastes
            << QString::fromUtf8("שיר השירים" )    //song of songs
            << QString::fromUtf8("ישעיה"      )    //isaiah
            << QString::fromUtf8("ירמיה"      )    //jeremiah
            << QString::fromUtf8("איכה"       )    //lamentations
            << QString::fromUtf8("יחזקאל"     )    //ezekiel
            << QString::fromUtf8("דניאל"      )    //daniel
            << QString::fromUtf8("הושע"       )    //hosea
            << QString::fromUtf8("יואל"       )    //joel
            << QString::fromUtf8("עמוס"       )    //amos
            << QString::fromUtf8("עובדיה"     )    //obadiah
            << QString::fromUtf8("יונה"       )    //jonah
            << QString::fromUtf8("מיכה"       )    //micah
            << QString::fromUtf8("נחום"       )    //nahum
            << QString::fromUtf8("חבקוק"      )    //habbakuk
            << QString::fromUtf8("צפניה"      )    //zephaniah
            << QString::fromUtf8("חגי"        )    //haggai
            << QString::fromUtf8("זכריה"      )    //zechariah
            << QString::fromUtf8("מלאכי"     );    //malachi

    //standardBookNames << "Matthew" << "Mark" << "Luke" << "John" << "Acts" << "Romans" << "1 Corinthians" << "2 Corinthians" << "Galatians" << "Ephesians" << "Philippians" << "Colossians" << "1 Thessalonians" << "2 Thessalonians" << "1 Timothy" << "2 Timothy" << "Titus" << "Philemon" << "Hebrews" << "James" << "1 Peter" << "2 Peter" << "1 John" << "2 John" << "3 John" << "Jude" << "Revelation";

    QStringList bookNames;
    bookNames << "Gen"
            << "Exod"
            << "Lev"
            << "Num"
            << "Deut"
            << "Josh"
            << "Judg"
            << "Ruth"
            << "1Sam"
            << "2Sam"
            << "1Kgs"
            << "2Kgs"
            << "1Chr"
            << "2Chr"
            << "Ezra"
            << "Neh"
            << "Esth"
            << "Job"
            << "Ps"
            << "Prov"
            << "Eccl"
            << "Song"
            << "Isa"
            << "Jer"
            << "Lam"
            << "Ezek"
            << "Dan"
            << "Hos"
            << "Joel"
            << "Amos"
            << "Obad"
            << "Jonah"
            << "Mic"
            << "Nah"
            << "Hab"
            << "Zeph"
            << "Hag"
            << "Zech"
            << "Mal";

    //    bookNames.clear();
    //    bookNames << "Gen";


    QString folder = "wlc/";

    int normalisedChapter = 1;

    for(int i=0; i<bookNames.size(); i++)
    {
        QDomDocument doc("mydocument");
        QFile file(folder+bookNames.at(i)+".xml");
        if (!file.open(QIODevice::ReadOnly))
            exit(1);

        QByteArray byteArray = file.readAll();

        QString wholeFile = QString::fromUtf8(byteArray.data());

        wholeFile = wholeFile.simplified();

        if(!doc.setContent(wholeFile))
        {
            file.close();
            qDebug() << "xml not well formed";
            exit(1);
        }
        file.close();

        QDomDocument chunk;
        chunk.appendChild(chunk.createElement("chunk"));
        chunk.firstChild().appendChild(chunk.createElement("bookName"));
        chunk.firstChild().firstChild().appendChild(chunk.createTextNode(standardBookNames.at(i)));
        qDebug() << standardBookNames.at(i);
        writeOutChunk(query, i+1, normalisedChapter, 0, 0,  chunk.toString(-1));

        QDomNodeList chapterNodeList = doc.elementsByTagName("chapter");

        for(int j=0; j<chapterNodeList.size(); j++)
        {
            QDomNode chapterNode = chapterNodeList.at(j);
            QString chapterDescription = ((QDomElement&)chapterNode).attribute("osisID");
            int chapterFromDescription = chapterDescription.mid(chapterDescription.indexOf(".")+1).toInt();

            QDomNodeList verseNodeList = ((QDomElement&)chapterNode).elementsByTagName("verse");
            for(int k=0; k<verseNodeList.size(); k++)
            {
                QDomNode verseNode = verseNodeList.at(k);
                QString verseDescription = ((QDomElement&)verseNode).attribute("osisID");
                int verseFromDescription = verseDescription.mid(verseDescription.lastIndexOf(".")+1).toInt();

                QDomDocument chunk;
                QDomNode place;
                place = chunk.appendChild(chunk.createElement("chunk"));
                place = place.appendChild(chunk.createElement("preferredVerse"));
                place = place.appendChild(chunk.createElement("bodyText"));


                if(k==0)
                {
                    QDomNode chapterPlace = place.appendChild(chunk.createElement("chapter"));
                    chapterPlace.appendChild(chunk.createTextNode(QString().setNum(chapterFromDescription)));
                }

                QDomNode versePlace = place.appendChild(chunk.createElement("verse"));
                versePlace.appendChild(chunk.createTextNode(QString().setNum(verseFromDescription)));

                for(int l=0; l<verseNode.childNodes().size(); l++)
                {
                    QDomNode wordNode = verseNode.childNodes().at(l);
                    if(wordNode.isElement())
                    {
                        if(wordNode.nodeName() == "w")
                        {
                            QString text = wordNode.toElement().firstChild().toText().data();

                            text.replace("/","");


                            int strongsNumber = ((QDomElement&)wordNode).attribute("lemma").toInt();

                            QDomElement wordElement = chunk.createElement("word");
                            wordElement.setAttribute("bibleTextId", "3");
                            wordElement.setAttribute("wordId", wordIdCount);

                            writeOutStrongsWord(query, wordIdCount, strongsNumber, 0, 0);

                            wordIdCount++;

                            QDomNode wordPlace = place.appendChild(wordElement);
                            wordPlace.appendChild(chunk.createTextNode(text));

                            place.appendChild(chunk.createTextNode(" "));

                        }
                        else if(wordNode.nodeName() == "seg")
                        {
                            QString punc = wordNode.toElement().firstChild().toText().data();

                            if(wordNode.toElement().attribute("type") == "x-pe")
                            {
                                place.appendChild(chunk.createTextNode(punc));
                                place.appendChild(chunk.createTextNode(" "));
                            }
                            else
                            {
                                if(place.lastChild().toText().data() == " ")
                                    place.removeChild(place.lastChild());

                                if(wordNode.toElement().attribute("type") == "x-maqqef")
                                {
                                    place.appendChild(chunk.createTextNode(punc));
                                }
                                else
                                {
                                    QDomElement puncElement = chunk.createElement("punc");
                                    puncElement.appendChild(chunk.createTextNode(punc));
                                    place.appendChild(puncElement);

                                    place.appendChild(chunk.createTextNode(" "));
                                }
                            }

                        }
                        else if(wordNode.nodeName() == "note")
                        {

                        }
                        else
                        {
                            qDebug() << "fail, not w or seg or note. " << wordNode.nodeName() << endl;
                            exit(1);
                        }
                    }
                    else
                    {
                        qDebug() << "fail, not an element" << endl;
                        exit(1);
                    }
                }

                if(k==verseNodeList.size()-1)
                {
                    place.appendChild(chunk.createElement("br"));
                    place.appendChild(chunk.createElement("br"));
                }

                writeOutChunk(query, i+1, normalisedChapter, chapterFromDescription, verseFromDescription, chunk.toString(-1));

            }
            normalisedChapter = normalisedChapter + 1;
        }
    }

    db.commit();
    db.close();

    return 0;
}
