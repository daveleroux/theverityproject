#include <QtCore/QCoreApplication>
#include <QtSql>
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
    query.exec("drop table esv");
    if(!query.exec("create table esv (id integer primary key autoincrement, "
                   "book_number int, book varchar(20), normalised_chapter integer, chapter integer, "
                   "verse integer, number_in_verse integer, paragraph bool, text varchar(50), "
                   "morphological_tag varchar(20), normalised_morph_tag blob, strongs_number integer)"))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

    QStringList bookNames;
    bookNames <<"Genesis"
            <<"Exodus"
            <<"Leviticus"
            <<"Numbers"
            <<"Deuteronomy"
            <<"Joshua"
            <<"Judges"
            <<"Ruth"
            <<"1 Samuel"
            << "2 Samuel"
            << "1 Kings"
            << "2 Kings"
            << "1 Chronicles"
            << "2 Chronicles"
            << "Ezra"
            << "Nehemiah"
            << "Esther"
            << "Job"
            << "Psalms"
            << "Proverbs"
            << "Ecclesiastes"
            << "Song of Songs"
            << "Isaiah"
            << "Jeremiah"
            << "Lamentations"
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
            << "Zephaniah"
            << "Haggai"
            << "Zechariah"
            << "Malachi"
            << "Matthew"
            << "Mark"
            << "Luke"
            << "John"
            << "Acts"
            << "Romans"
            << "1 Corinthians"
            << "2 Corinthians"
            << "Galatians"
            << "Ephesians"
            << "Philippians"
            << "Colossians"
            << "1 Thessalonians"
            << "2 Thessalonians"
            << "1 Timothy"
            << "2 Timothy"
            << "Titus"
            << "Philemon"
            << "Hebrews"
            << "James"
            << "1 Peter"
            << "2 Peter"
            << "1 John"
            << "2 John"
            << "3 John"
            << "Jude"
            << "Revelation";

    QFile file("esv.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "couldn't open esv.csv";
        exit(1);
    }

    int currentChapter = 1;
    int normalisedChapter = 1;
    int currentBookNumber = 1;

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();

        QStringList stringList = line.split("`");


        int bookNumber = ((QString)(stringList.at(3))).toInt();

        int chapterNumber = ((QString)(stringList.at(5))).toInt();

        if(chapterNumber != currentChapter || bookNumber != currentBookNumber)
        {
            normalisedChapter++;
            currentChapter = chapterNumber;
        }


        if(bookNumber != currentBookNumber)
        {
            qDebug() << "done: " << bookNames.at(currentBookNumber-1);
            currentBookNumber = bookNumber;
        }

        int verseNumber = ((QString)(stringList.at(7))).toInt();

        QString verse = stringList.at(9);

        verse.replace("{\\cf6 ","");
        verse.replace("{\\cf2 1:15} ","");

        verse.replace("}", "");


        if(!verse.contains("{\\cf15\\I (OMITTED TEXT)"))
        {
            QStringList wordsInVerse = verse.split(" ");
            wordsInVerse.removeAt(0);
            wordsInVerse.removeAt(wordsInVerse.size()-1);

            for(int i=0; i<wordsInVerse.size(); i++)
            {
                query.prepare("insert into esv values(:id,:book_number,:book,:normalised_chapter,"
                              ":chapter, :verse, :number_in_verse , :paragraph, :text, :morphological_tag, "
                              ":normalised_morph_tag, :strongs_number)");

                query.bindValue(":id", QVariant(QVariant::Int));
                query.bindValue(":book_number", bookNumber);
                query.bindValue(":book", bookNames.at(bookNumber-1));
                query.bindValue(":normalised_chapter", normalisedChapter);
                query.bindValue(":chapter", chapterNumber);
                query.bindValue(":verse", verseNumber);
                query.bindValue(":number_in_verse", i+1);
                query.bindValue(":paragraph",false);
                query.bindValue(":text", wordsInVerse.at(i));
                query.bindValue(":morphological_tag", 0);
                query.bindValue(":normalised_morph_tag", 0);
                query.bindValue(":strongs_number", 0);

                if(!query.exec())
                {
                    qDebug() << "failed: "<< query.lastError()  << endl;
                    exit(1);
                }
            }
        }
    }

    query.exec("create index idx_esv on esv (book_number, normalised_chapter, chapter, verse, number_in_verse)");

    db.close();

    return 0;
}
