#include <QtCore/QCoreApplication>
#include <QtSql>
#include <QDomDocument>
#include <QDomNodeList>
#include <QDomNode>
#include <QDebug>
#include <QFile>
#include <QList>



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("bibles.sqlite");
    if (!db.open())
    {
        qDebug() << "couldn't open db" << endl;
        return 1;
    }

    QSqlQuery query;
    query.exec("drop table wlc");
    if(!query.exec("create table wlc (id integer primary key autoincrement, "
                   "book_number int, book varchar(20), normalised_chapter integer, chapter integer, "
                   "verse integer, number_in_verse integer, paragraph bool, text varchar(50), "
                   "morphological_tag varchar(20), normalised_morph_tag blob, strongs_number integer)"))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

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

    int normalisedChapter = 0;

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


        QDomNodeList chapterNodeList = doc.elementsByTagName("chapter");


        for(int j=0; j<chapterNodeList.size(); j++)
        {
            normalisedChapter = normalisedChapter + 1;

            QDomNode chapterNode = chapterNodeList.at(j);
            QString chapterDescription = ((QDomElement&)chapterNode).attribute("osisID");
            int chapterFromDescription = chapterDescription.mid(chapterDescription.indexOf(".")+1).toInt();

            QDomNodeList verseNodeList = ((QDomElement&)chapterNode).elementsByTagName("verse");
            for(int k=0; k<verseNodeList.size(); k++)
            {
                QDomNode verseNode = verseNodeList.at(k);
                QString verseDescription = ((QDomElement&)verseNode).attribute("osisID");
                int verseFromDescription = verseDescription.mid(verseDescription.lastIndexOf(".")+1).toInt();

                QStringList wordsInVerse;
                QList<int> strongsNumbers;

                for(int l=0; l<verseNode.childNodes().size(); l++)
                {

                    QDomNode wordNode = verseNode.childNodes().at(l);
                    if(wordNode.isElement())
                    {
                        if(wordNode.nodeName() == "w")
                        {
                            QString word = wordNode.toElement().firstChild().toText().data();

                            word.replace("/","");
                            wordsInVerse.append(word);

                            strongsNumbers.append(((QDomElement&)wordNode).attribute("lemma").toInt());


                        }
                        else if(wordNode.nodeName() == "seg")
                        {
                            QString punc = wordNode.toElement().firstChild().toText().data();
                            (QString)(wordsInVerse[wordsInVerse.size()-1]).append(punc);

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
                for(int m=0; m<wordsInVerse.size(); m++)
                {
                    query.prepare("insert into wlc values(:id,:book_number,:book,:normalised_chapter,"
                                  ":chapter, :verse, :number_in_verse , :paragraph, :text, :morphological_tag, "
                                  ":normalised_morph_tag, :strongs_number)");

                    query.bindValue(":id", QVariant(QVariant::Int));
                    query.bindValue(":book_number", i+1);
                    query.bindValue(":book", bookNames.at(i));
                    query.bindValue(":normalised_chapter", normalisedChapter);
                    query.bindValue(":chapter", chapterFromDescription);
                    query.bindValue(":verse", verseFromDescription);
                    query.bindValue(":number_in_verse", m+1);
                    query.bindValue(":paragraph",false);
                    query.bindValue(":text", wordsInVerse.at(m));
                    query.bindValue(":morphological_tag", 0);
                    query.bindValue(":normalised_morph_tag", 0);
                    query.bindValue(":strongs_number", strongsNumbers.at(m));

                    if(!query.exec())
                    {
                        qDebug() << "failed: "<< query.lastError()  << endl;
                        exit(1);

                    }
                }

            }

        }

        qDebug() << "done: " << bookNames.at(i) << endl;
    }

    query.exec("create index idx_wlc on wlc (book_number, normalised_chapter, chapter, verse, number_in_verse)");

    db.close();

    return 0;
}
