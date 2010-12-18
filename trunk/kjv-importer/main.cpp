#include <QtCore/QCoreApplication>
#include <QtSql>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QStringList>

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
    query.exec("drop table kjv");
    if(!query.exec("create table kjv (id integer primary key autoincrement, "
                   "book_number int, book varchar(20), normalised_chapter integer, chapter integer, "
                   "verse integer, number_in_verse integer, paragraph bool, text varchar(50), "
                   "morphological_tag varchar(20), normalised_morph_tag blob, strongs_number integer)"))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

    QStringList bookNames;
    bookNames << "Genesis"
            << "Exodus"
            << "Lev"
            << "Num"
            << "Deut"
            << "Joshua"
            << "Judges"
            << "Ruth"
            << "1Sam"
            << "2Sam"
            << "1Kings"
            << "2Kings"
            << "1Chron"
            << "2Chron"
            << "Ezra"
            << "Nehemiah"
            << "Esther"
            << "Job"
            << "Psalms"
            << "Proverbs"
            << "Eccl"
            << "Song"
            << "Isaiah"
            << "Jeremiah"
            << "Lament"
            << "Ezekiel"
            << "Daniel"
            << "Hosea"
            << "Joel"
            << "Amos"
            << "Obadiah"
            << "Jonah"
            << "Micah"
            << "Nahum"
            << "Habakkuk"
            << "Zeph"
            << "Haggai"
            << "Zech"
            << "Malachi"
            << "Matthew"
            << "Mark"
            << "Luke"
            << "John"
            << "Acts"
            << "Romans"
            << "1Cor"
            << "2Cor"
            << "Gal"
            << "Eph"
            << "Philip"
            << "Col"
            << "1Thes"
            << "2Thes"
            << "1Tim"
            << "2Tim"
            << "Titus"
            << "Philemon"
            << "Hebrews"
            << "James"
            << "1Peter"
            << "2Peter"
            << "1John"
            << "2John"
            << "3John"
            << "Jude"
            << "Rev";

//    bookNames.clear();
//    bookNames << "Genesis";

    QString folder = "kjv/";

    int normalisedChapter = 0;

    bool newParagraph = false;

    int insertCount = 0;
    db.transaction();

    for(int i=0; i<bookNames.size(); i++)
    {
        int currentRefChapter = 0;

        QFile file(folder+bookNames.at(i)+".txt");
        if (!file.open(QIODevice::ReadOnly))
            exit(1);

        QByteArray byteArray = file.readAll();
        QString wholeFile = QString::fromUtf8(byteArray.data());
        file.close();

        wholeFile.replace("\r\n\r\n", "|");
        wholeFile = wholeFile.simplified();

        int refStartIndex = 0;
        refStartIndex = wholeFile.indexOf("{", refStartIndex);


        while(refStartIndex != -1)
        {
            int refEndIndex = wholeFile.indexOf("}", refStartIndex);
            QString reference = wholeFile.mid(refStartIndex+1,  refEndIndex - refStartIndex -1);

            int verseStartIndex = refEndIndex + 2;

            refStartIndex = wholeFile.indexOf("{", verseStartIndex);
            int verseEndIndex = refStartIndex == -1 ? wholeFile.length() - 1 : refStartIndex -1;

            QString verse = wholeFile.mid(verseStartIndex, verseEndIndex - verseStartIndex);

            QStringList refList = reference.split(":");
            int refChapter = refList.at(0).toInt();
            int refVerse = refList.at(1).toInt();

            if(refChapter != currentRefChapter)
            {
                normalisedChapter++;
                currentRefChapter = refChapter;
                qDebug() << "chapter: " << refChapter;
            }

            QStringList wordsInVerse = verse.split(" ");

            for(int m=0; m<wordsInVerse.size(); m++)
            {
                insertCount++;
                if(insertCount % 1000 == 0)
                {
                    db.commit();
                    db.transaction();
                }

                QString originalWord = wordsInVerse.at(m);
                QString word = originalWord.endsWith("|") ? originalWord.mid(0, originalWord.length() - 1) : originalWord;

                query.prepare("insert into kjv values(:id,:book_number,:book,:normalised_chapter,"
                              ":chapter, :verse, :number_in_verse , :paragraph, :text, :morphological_tag, "
                              ":normalised_morph_tag, :strongs_number)");

                query.bindValue(":id", QVariant(QVariant::Int));
                query.bindValue(":book_number", i+1);
                query.bindValue(":book", bookNames.at(i));
                query.bindValue(":normalised_chapter", normalisedChapter);
                query.bindValue(":chapter", refChapter);
                query.bindValue(":verse", refVerse);
                query.bindValue(":number_in_verse", m+1);
                query.bindValue(":paragraph",newParagraph);
                query.bindValue(":text", word);
                query.bindValue(":morphological_tag", 0);
                query.bindValue(":normalised_morph_tag", 0);
                query.bindValue(":strongs_number", 0);

                if(!query.exec())
                {
                    qDebug() << "failed: "<< query.lastError()  << endl;
                    exit(1);

                }

                newParagraph = originalWord.endsWith("|") ? true : false;
            }

        }


        qDebug() << "done: " << bookNames.at(i) << endl;
    }

    db.commit();
    query.exec("create index idx_kjv on kjv (book_number, normalised_chapter, chapter, verse, number_in_verse)");

    db.close();


    return 0;
}
