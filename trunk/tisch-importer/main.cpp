#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QtSql>
#include <QBitArray>
#include <QList>
#include <QDataStream>
#include <QByteArray>
#include <QDomDocument>
#include "book.h"
#include "chapter.h"
#include "verse.h"
#include "word.h"

int HEBREW_COUNT = 8674;

QList<Book*> books;
Word* lastWordAdded;

int wordIdCount;

struct MorphAttribute
{
    QBitArray bitArray;
    QString code;

    MorphAttribute(QBitArray b, QString c)
    {
        bitArray = b;
        code = c;
    }
};

QList<MorphAttribute> posAttributes;
QList<MorphAttribute> suffixAttributes;
QList<MorphAttribute> caseAttributes;
QList<MorphAttribute> numberAttributes;
QList<MorphAttribute> genderAttributes;
QList<MorphAttribute> tenseAttributes;
QList<MorphAttribute> voiceAttributes;
QList<MorphAttribute> moodAttributes;
QList<MorphAttribute> personAttributes;
QList<MorphAttribute> verbExtraAttributes;

QBitArray addAttribute(QList<MorphAttribute> attributes, QBitArray current, QString code)
{
    bool found = false;
    for(int i=0; i<attributes.size(); i++)
    {
        MorphAttribute morphAttribute = attributes.at(i);
        if(code == morphAttribute.code)
        {            
            current = current | morphAttribute.bitArray;
            found = true;
        }
    }
    if(!found)
    {
        qDebug() << "not found";
        exit(1);
    }
    return current;
}


bool containsPosAttributeInRange(QBitArray bitArray, int startIndex, int endIndex)
{
    QBitArray zero(81);

    for(int i=startIndex; i<=endIndex; i++)
    {
        if((bitArray & posAttributes.at(i).bitArray) != zero)
        {
            return true;
        }
    }
    return false;
}

bool containsMoodAttributeInRange(QBitArray bitArray, int startIndex, int endIndex)
{
    QBitArray zero(81);

    for(int i=startIndex; i<=endIndex; i++)
    {
        if((bitArray & moodAttributes.at(i).bitArray) != zero)
        {
            return true;
        }
    }
    return false;
}

QBitArray convertMorphToBitArray(QString morphString)
{
    //    QString origMorphString = morphString;

    //    qDebug() << morphString;

    if(morphString.startsWith("N-PRI") ||
       morphString.startsWith("A-NUI") ||
       morphString.startsWith("N-LI") ||
       morphString.startsWith("N-OI"))
    {
        morphString = morphString.replace(morphString.indexOf("-"),1,"");
    }

    //    qDebug() << morphString;

    //    if(origMorphString == "N-PRI")
    //        exit(0);

    QStringList chunks = morphString.split("-");


    QBitArray result(81);

    result = addAttribute(posAttributes, result, chunks.at(0));

    if(containsPosAttributeInRange(result,14,25))
    {
        if(chunks.size()==2)
        {
            result = addAttribute(suffixAttributes, result, chunks.at(1));
        }
    }
    else if (containsPosAttributeInRange(result, 0, 2))
    {
        result = addAttribute(caseAttributes, result, chunks.at(1).at(0));
        result = addAttribute(numberAttributes, result, chunks.at(1).at(1));
        result = addAttribute(genderAttributes, result, chunks.at(1).at(2));

        if(chunks.size()==3)
        {
            result = addAttribute(suffixAttributes, result, chunks.at(2));
        }
    }
    else if (containsPosAttributeInRange(result, 3, 3))
    {
        QString tense;
        QString voice;
        QString mood;

        if(chunks.at(1).at(0) == '2')
        {
            tense = chunks.at(1).mid(0,2);
            voice = chunks.at(1).at(2);
            mood = chunks.at(1).at(3);
        }
        else
        {
            tense = chunks.at(1).at(0);
            voice = chunks.at(1).at(1);
            mood = chunks.at(1).at(2);
        }

        result = addAttribute(tenseAttributes, result, tense);
        result = addAttribute(voiceAttributes, result, voice);
        result = addAttribute(moodAttributes, result, mood);

        if (containsMoodAttributeInRange(result, 0, 3))
        {
            result = addAttribute(personAttributes, result, chunks.at(2).at(0));
            result = addAttribute(numberAttributes, result, chunks.at(2).at(1));

            if(chunks.size() == 4)
            {
                result = addAttribute(verbExtraAttributes, result, chunks.at(3));
            }
        }
        else if (containsMoodAttributeInRange(result, 5, 6))
        {
            result = addAttribute(caseAttributes, result, chunks.at(2).at(0));
            result = addAttribute(numberAttributes, result, chunks.at(2).at(1));
            result = addAttribute(genderAttributes, result, chunks.at(2).at(2));

            if(chunks.size() == 4)
            {
                result = addAttribute(verbExtraAttributes, result, chunks.at(3));
            }
        }
    }
    else if (containsPosAttributeInRange(result,4,13))
    {        
        if(chunks.at(1).at(0) == '1' || chunks.at(1).at(0) == '2' || chunks.at(1).at(0) == '3')
        {
            result = addAttribute(personAttributes, result, chunks.at(1).at(0));
            result = addAttribute(caseAttributes, result, chunks.at(1).at(1));
            result = addAttribute(numberAttributes, result, chunks.at(1).at(2));
            if(chunks.at(1).length() == 4)
                result = addAttribute(genderAttributes, result, chunks.at(1).at(3));
        }
        else
        {
            result = addAttribute(caseAttributes, result, chunks.at(1).at(0));
            result = addAttribute(numberAttributes, result, chunks.at(1).at(1));
            if(chunks.at(1).length() == 3)
                result = addAttribute(genderAttributes, result, chunks.at(1).at(2));
        }

        if(chunks.size() == 3)
        {
            result = addAttribute(suffixAttributes, result, chunks.at(2));
        }
    }

    return result;
}

MorphAttribute nextMorphAttribute(QString s)
{
    static int index = 0;
    QBitArray bitArray(81);
    bitArray.setBit(index);
    index = index + 1;

    MorphAttribute result(bitArray, s);

    return result;
}

void initialiseAttributes()
{
    posAttributes.append(nextMorphAttribute("N"   ));//0  noun
    posAttributes.append(nextMorphAttribute("A"   ));//1  adjective
    posAttributes.append(nextMorphAttribute("T"   ));//2  article
    posAttributes.append(nextMorphAttribute("V"   ));//3  verb
    posAttributes.append(nextMorphAttribute("P"   ));//4  personal pronoun
    posAttributes.append(nextMorphAttribute("R"   ));//5  relative pronoun
    posAttributes.append(nextMorphAttribute("C"   ));//6  reciprocal pronoun
    posAttributes.append(nextMorphAttribute("D"   ));//7  demonstrative pronoun
    posAttributes.append(nextMorphAttribute("K"   ));//8  correlative pronoun
    posAttributes.append(nextMorphAttribute("I"   ));//9  interrogative pronoun
    posAttributes.append(nextMorphAttribute("X"   ));//10  indefinite pronoun
    posAttributes.append(nextMorphAttribute("Q"   ));//11  correlative or interrogative pronoun
    posAttributes.append(nextMorphAttribute("F"   ));//12  reflexive pronoun
    posAttributes.append(nextMorphAttribute("S"   ));//13  possessive pronoun
    posAttributes.append(nextMorphAttribute("ADV" ));//14  adverb
    posAttributes.append(nextMorphAttribute("CONJ"));//15  conjunction
    posAttributes.append(nextMorphAttribute("COND"));//16  cond
    posAttributes.append(nextMorphAttribute("PRT" ));//17  particle
    posAttributes.append(nextMorphAttribute("PREP"));//18  preposition
    posAttributes.append(nextMorphAttribute("INJ" ));//19  interjection
    posAttributes.append(nextMorphAttribute("ARAM"));//20  aramaic
    posAttributes.append(nextMorphAttribute("HEB" ));//21  hebrew
    posAttributes.append(nextMorphAttribute("NPRI"));//22  proper noun indeclinable
    posAttributes.append(nextMorphAttribute("ANUI"));//23  numeral indeclinable
    posAttributes.append(nextMorphAttribute("NLI" ));//24  letter indeclinable
    posAttributes.append(nextMorphAttribute("NOI" ));//25  noun other type indeclinable

    voiceAttributes.append(nextMorphAttribute("A"));//active
    voiceAttributes.append(nextMorphAttribute("M"));//middle
    voiceAttributes.append(nextMorphAttribute("P"));//passive
    voiceAttributes.append(nextMorphAttribute("E"));//middle or passive
    voiceAttributes.append(nextMorphAttribute("D"));//middle deponent
    voiceAttributes.append(nextMorphAttribute("O"));//passive deponent
    voiceAttributes.append(nextMorphAttribute("N"));//middle or passive deponent
    voiceAttributes.append(nextMorphAttribute("Q"));//impersonal active
    voiceAttributes.append(nextMorphAttribute("X"));//no voice

    moodAttributes.append(nextMorphAttribute("I"));//indicative ............0
    moodAttributes.append(nextMorphAttribute("S"));//subjunctive ...........1
    moodAttributes.append(nextMorphAttribute("O"));//optative ..............2
    moodAttributes.append(nextMorphAttribute("M"));//imperative ............3
    moodAttributes.append(nextMorphAttribute("N"));//infinitive ............4
    moodAttributes.append(nextMorphAttribute("P"));//participle ............5
    moodAttributes.append(nextMorphAttribute("R"));//imperative participle .6

    tenseAttributes.append(nextMorphAttribute("P"  )); //present
    tenseAttributes.append(nextMorphAttribute("I"  )); //imperfect
    tenseAttributes.append(nextMorphAttribute("F"  )); //future
    tenseAttributes.append(nextMorphAttribute("2F" )); //second future
    tenseAttributes.append(nextMorphAttribute("A"  )); //aorist
    tenseAttributes.append(nextMorphAttribute("2A" )); //second aorist
    tenseAttributes.append(nextMorphAttribute("R"  )); //perfect
    tenseAttributes.append(nextMorphAttribute("2R" )); //second perfect
    tenseAttributes.append(nextMorphAttribute("L"  )); //pluperfect
    tenseAttributes.append(nextMorphAttribute("2L" )); //second pluperfect
    tenseAttributes.append(nextMorphAttribute("X"  )); //no tense stated

    personAttributes.append(nextMorphAttribute("1")); //first person
    personAttributes.append(nextMorphAttribute("2")); //second person
    personAttributes.append(nextMorphAttribute("3")); //third person

    caseAttributes.append(nextMorphAttribute("N")); //nominative
    caseAttributes.append(nextMorphAttribute("V")); //vocative
    caseAttributes.append(nextMorphAttribute("G")); //genitive
    caseAttributes.append(nextMorphAttribute("D")); //dative
    caseAttributes.append(nextMorphAttribute("A")); //accusative

    genderAttributes.append(nextMorphAttribute("M")); //masculine
    genderAttributes.append(nextMorphAttribute("F")); //feminine
    genderAttributes.append(nextMorphAttribute("N")); //neuter

    numberAttributes.append(nextMorphAttribute("S")); //singular
    numberAttributes.append(nextMorphAttribute("P")); //plural

    suffixAttributes.append(nextMorphAttribute("S"));   //superlative
    suffixAttributes.append(nextMorphAttribute("C"));   //comparative
    suffixAttributes.append(nextMorphAttribute("ABB")); //abbreviated
    suffixAttributes.append(nextMorphAttribute("I"));   //interrogative
    suffixAttributes.append(nextMorphAttribute("N"));   //negative
    suffixAttributes.append(nextMorphAttribute("ATT")); //attic
    suffixAttributes.append(nextMorphAttribute("P"));   //particle attached
    suffixAttributes.append(nextMorphAttribute("K"));   //crasis

    verbExtraAttributes.append(nextMorphAttribute("M"));  //middle significance
    verbExtraAttributes.append(nextMorphAttribute("C"));  //contracted form
    verbExtraAttributes.append(nextMorphAttribute("T"));  //transitive
    verbExtraAttributes.append(nextMorphAttribute("A"));  //aeolic
    verbExtraAttributes.append(nextMorphAttribute("ATT"));//attic
    verbExtraAttributes.append(nextMorphAttribute("AP")); //apocopated form
    verbExtraAttributes.append(nextMorphAttribute("IRR"));//irregular or impure form

}

//void readInSbl()
//{
//    QDomDocument doc("mydocument");
//    QFile file("sblgnt.xml");
//    if (!file.open(QIODevice::ReadOnly))
//        exit(1);

//    QByteArray byteArray = file.readAll();

//    QString wholeFile = QString::fromUtf8(byteArray.data());


//    QString errorMsg;
//    int errorLine;
//    int errorColumn;
//    if(!doc.setContent(wholeFile, &errorMsg , &errorLine, &errorColumn))
//    {
//        file.close();
//        qDebug() << "xml not well formed";
//        qDebug() << errorMsg;
//        qDebug() << errorLine;
//        qDebug() << errorColumn;
//        exit(1);
//    }

//    QDomNodeList list = doc.elementsByTagName("title");
//    for(int i=0; i<list.size(); i++)
//    {
//        QDomNode node = list.at(i);
//        qDebug() << node.firstChild().toText().data();
//    }
//}

void assert(bool b)
{
    if(!b)
    {
        qDebug() << "assert failed";
        exit(1);
    }
}

void writeOutStrongsWord(QSqlQuery query, int wordId, int strongsNumber, QString strongsLemma, QString fribergLemma)
{
    query.prepare("insert into strongs_word values(:bibletext_id, :word_id, :strongsNumber, :strongsLemma, :fribergLemma)");

    query.bindValue(":bibletext_id", 2);
    query.bindValue(":word_id", wordId);
    query.bindValue(":strongsNumber", strongsNumber + HEBREW_COUNT);
    query.bindValue(":strongsLemma", strongsLemma);
    query.bindValue(":fribergLemma", fribergLemma);

    if(!query.exec())
    {
        qDebug() << "failed: "<< query.lastError()  << endl;
        exit(1);
    }
}

void writeOutParsing(QSqlQuery query, int wordId, QByteArray* morph)
{
    query.prepare("insert into type_t_parsing values(:bibletext_id, :word_id, :morph)");

    query.bindValue(":bibletext_id", 2);
    query.bindValue(":word_id", wordId);
    query.bindValue(":morph", morph->toBase64(), QSql::Binary | QSql::In);

    if(!query.exec())
    {
        qDebug() << "failed: "<< query.lastError()  << endl;
        exit(1);
    }
}

void writeOutChunk(QSqlQuery query, int bookNumber, int normalisedChapter,int chapter, int verse, QString text)
{
    query.prepare("insert into bibles values(:id, :bibletext_id, :book_number, :normalised_chapter, :chapter, :verse, :text, :parallel)");

    query.bindValue(":id", QVariant(QVariant::Int));
    query.bindValue(":bibletext_id", 2);
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


void writeOut(QSqlDatabase db, QSqlQuery query)
{
    int normalisedChapter = 1;

    for(int i=0; i<books.size(); i++)
    {
        Book* book = books.at(i);

        QDomDocument chunk;
        chunk.appendChild(chunk.createElement("chunk"));
        chunk.firstChild().appendChild(chunk.createElement("bookName"));
        chunk.firstChild().firstChild().appendChild(chunk.createElement("greek"));
        chunk.firstChild().firstChild().firstChild().appendChild(chunk.createTextNode(book->name));
        qDebug() << book->name;

        writeOutChunk(query, i+40, normalisedChapter, 0, 0,  chunk.toString(-1));


        for(int j=0; j<book->chapters.keys().size(); j++)
        {
            int chapterInt = book->chapters.keys().at(j);
            Chapter* chapter = book->chapters.value(chapterInt);

            for(int k=0; k<chapter->verses.keys().size(); k++)
            {
                int verseInt = chapter->verses.keys().at(k);
                QList<Verse*>* verses = chapter->verses.value(verseInt);

                QDomDocument chunk;
                chunk.appendChild(chunk.createElement("chunk"));

                for(int l=0; l<verses->size(); l++)
                {
                    QString name = "preferredVerse";
                    if(l>0)
                        name = "alternateVerse";

                    QDomNode place = chunk.createElement(name);
                    chunk.firstChild().appendChild(place);

                    place = place.appendChild(chunk.createElement("bodyText"));
                    place = place.appendChild(chunk.createElement("greek"));
                    if(verseInt == 1 && l == 0)
                    {
                        QDomNode chapterPlace = place.appendChild(chunk.createElement("chapter"));
                        chapterPlace.appendChild(chunk.createTextNode(QString().setNum(chapterInt)));
                    }

                    QDomNode versePlace = place.appendChild(chunk.createElement("verse"));
                    versePlace.appendChild(chunk.createTextNode(QString().setNum(verseInt)));

                    Verse* verse = verses->at(l);
                    for(int m=0; m<verse->words.size(); m++)
                    {
                        Word* word = verse->words.at(m);

                        QDomElement wordElement = chunk.createElement("word");
                        wordElement.setAttribute("bibleTextId", "2");
                        wordElement.setAttribute("wordId", wordIdCount);

                        writeOutParsing(query, wordIdCount, word->normalisedMorphTag);
                        writeOutStrongsWord(query, wordIdCount, word->strongsNumber, word->strongsLemma, word->fribergLemma);

                        wordIdCount++;

                        QDomNode wordPlace = place.appendChild(wordElement);
                        wordPlace.appendChild(chunk.createTextNode(word->text));

                        place.appendChild(chunk.createTextNode(" "));

                        if(word->isEndofParagraph)
                        {
                            chunk.firstChild().appendChild(chunk.createElement("br"));
                            chunk.firstChild().appendChild(chunk.createElement("br"));
                        }
                    }

                    if(k==chapter->verses.keys().size()-1 && l==verses->size()-1)
                    {
                        chunk.firstChild().appendChild(chunk.createElement("br"));
                        chunk.firstChild().appendChild(chunk.createElement("br"));
                    }

                }

                writeOutChunk(query, i+40, normalisedChapter, chapterInt, verseInt, chunk.toString(-1));

            }


            normalisedChapter++;
        }

        db.commit();
        db.transaction();
    }
}


void addToBook(Book* book, int chapterInt, int verseInt, int numberInVerse, QString text, QByteArray* normalisedMorphTag, int strongsNumber, QString strongsLemma, QString fribergLemma)
{
    assert(chapterInt > 0 && verseInt > 0 && numberInVerse > 0 && strongsNumber > 0);
    Chapter* chapter = book->chapters.value(chapterInt);
    if(chapter == 0)
    {
        chapter = new Chapter();
        book->chapters.insert(chapterInt, chapter);
    }

    Verse* verse;
    if(numberInVerse == 1)
    {
        verse = new Verse();
        if(chapter->verses.keys().contains(verseInt))
        {
            chapter->verses.value(verseInt)->append(verse);
        }
        else
        {
            QList<Verse*>* verseAndAlts = new QList<Verse*>;
            verseAndAlts->append(verse);
            chapter->verses.insert(verseInt, verseAndAlts);
        }
    }
    else
    {
        verse = chapter->verses.value(verseInt)->last();
    }

    lastWordAdded = new Word(text, normalisedMorphTag, strongsNumber, strongsLemma, fribergLemma);
    verse->words.append(lastWordAdded);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    wordIdCount = 1;

    initialiseAttributes();

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("verity.sqlite");
    if (!db.open())
    {
        qDebug() << "couldn't open db" << endl;
        exit(1);
    }

    db.transaction();

    QSqlQuery query;
    if(!query.exec("delete from bibles where bibletext_id=2"))
    {
        qDebug() << "failed: " << query.lastError();
        exit(1);
    }


    query.exec("drop table type_t_parsing");

    if(!query.exec("create table type_t_parsing ( "
                   "bibletext_id int, word_id int, "
                   "normalised_morph_tag blob, "
                   "primary key (bibletext_id, word_id))"))
    {
        qDebug() << "failed: " << query.lastError();
        exit(1);
    }

    query.exec("drop table strongs_word");

    if(!query.exec("create table strongs_word ( "
                   "bibletext_id int, word_id int, "
                   "strongs_number integer, strongs_lemma varchar(50), friberg_lemma varchar(50), "
                   "primary key (bibletext_id, word_id))"))
    {
        qDebug() << "failed: " << query.lastError();
        exit(1);
    }

    QStringList bookNames;
    bookNames << "MT"
            << "MR"
            << "LU"
            << "JOH"
            << "AC"
            << "RO"
            << "1CO"
            << "2CO"
            << "GA"
            << "EPH"
            << "PHP"
            << "COL"
            << "1TH"
            << "2TH"
            << "1TI"
            << "2TI"
            << "TIT"
            << "PHM"
            << "HEB"
            << "JAS"
            << "1PE"
            << "2PE"
            << "1JO"
            << "2JO"
            << "3JO"
            << "JUDE"
            << "RE";

    QStringList greekBookNames;
    greekBookNames
            << QString::fromUtf8("ΚΑΤΑ ΜΑΘΘΑΙΟΝ"         )
            << QString::fromUtf8("ΚΑΤΑ ΜΑΡΚΟΝ"           )
            << QString::fromUtf8("ΚΑΤΑ ΛΟΥΚΑΝ"           )
            << QString::fromUtf8("ΚΑΤΑ ΙΩΑΝΝΗΝ"          )
            << QString::fromUtf8("ΠΡΑΞΕΙΣ ΑΠΟΣΤΟΛΩΝ"     )
            << QString::fromUtf8("ΠΡΟΣ ΡΩΜΑΙΟΥΣ"         )
            << QString::fromUtf8("ΠΡΟΣ ΚΟΡΙΝΘΙΟΥΣ Α"     )
            << QString::fromUtf8("ΠΡΟΣ ΚΟΡΙΝΘΙΟΥΣ Β"     )
            << QString::fromUtf8("ΠΡΟΣ ΓΑΛΑΤΑΣ"          )
            << QString::fromUtf8("ΠΡΟΣ ΕΦΕΣΙΟΥΣ"         )
            << QString::fromUtf8("ΠΡΟΣ ΦΙΛΙΠΠΗΣΙΟΥΣ"     )
            << QString::fromUtf8("ΠΡΟΣ ΚΟΛΟΣΣΑΕΙΣ"       )
            << QString::fromUtf8("ΠΡΟΣ ΘΕΣΣΑΛΟΝΙΚΕΙΣ Α"  )
            << QString::fromUtf8("ΠΡΟΣ ΘΕΣΣΑΛΟΝΙΚΕΙΣ Β"  )
            << QString::fromUtf8("ΠΡΟΣ ΤΙΜΟΘΕΟΝ Α"       )
            << QString::fromUtf8("ΠΡΟΣ ΤΙΜΟΘΕΟΝ Β"       )
            << QString::fromUtf8("ΠΡΟΣ ΤΙΤΟΝ"            )
            << QString::fromUtf8("ΠΡΟΣ ΦΙΛΗΜΟΝΑ"         )
            << QString::fromUtf8("ΠΡΟΣ ΕΒΡΑΙΟΥΣ"         )
            << QString::fromUtf8("ΙΑΚΩΒΟΥ"               )
            << QString::fromUtf8("ΠΕΤΡΟΥ Α"              )
            << QString::fromUtf8("ΠΕΤΡΟΥ Β"              )
            << QString::fromUtf8("ΙΩΑΝΝΟΥ Α"             )
            << QString::fromUtf8("ΙΩΑΝΝΟΥ Β"             )
            << QString::fromUtf8("ΙΩΑΝΝΟΥ Γ"             )
            << QString::fromUtf8("ΙΟΥΔΑ"                 )
            << QString::fromUtf8("ΑΠΟΚΑΛΥΨΙΣ ΙΩΑΝΝΟΥ"    );

    QString folder = "Tischendorf-2.5/Unicode/";

    for(int i=0; i<bookNames.size(); i++)
    {
        Book* book = new Book(greekBookNames.at(i));
        books.append(book);

        QFile bookFile(folder+bookNames.at(i)+".txt");
        if(bookFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&bookFile);
            while(!stream.atEnd())
            {
                QString line = stream.readLine();
                for(int j=0; j<7; j++)
                {
                    line.replace(line.indexOf(" "), 1, "|");                    
                }
                line.replace(line.indexOf("!"),1,"|");
                QStringList fragments = line.split("|");


                QStringList chapterAndVerse = fragments.at(1).split(":");

                QString chapter = chapterAndVerse.at(0);
                QString verse = chapterAndVerse.at(1).split(".").at(0);
                QString numberInVerse = chapterAndVerse.at(1).split(".").at(1);


                QString text = fragments.at(4); //editor chosen

                if(fragments.at(2) == "P")
                {
                    lastWordAdded->isEndofParagraph = true;
                }

                QBitArray normalisedMorphTag = convertMorphToBitArray(fragments.at(5));

                QByteArray* byteArray = new QByteArray();
                QDataStream out(byteArray, QIODevice::ReadWrite);   // write the data
                out << normalisedMorphTag;


                QString strongsNumber = fragments.at(6);

                QString strongsLemma = fragments.at(7).trimmed();

                QString fribergLemma = fragments.at(8).trimmed();

                addToBook(book, chapter.toInt(), verse.toInt(), numberInVerse.toInt(), text, byteArray, strongsNumber.toInt(), strongsLemma, fribergLemma);


                //                query.prepare("insert into tisch_bible values(:id,:book_number,:book,:normalised_chapter,"
                //                              ":chapter, :verse, :number_in_verse , :paragraph, :text, :morphological_tag, "
                //                              ":normalised_morph_tag, :strongs_number, :strongs_lemma, :friberg_lemma)");

                //                query.bindValue(":id", QVariant(QVariant::Int));
                //                query.bindValue(":book_number", i+40);
                //                query.bindValue(":book", book     );
                //                query.bindValue(":normalised_chapter", normalisedChapter);
                //                query.bindValue(":chapter", chapter);
                //                query.bindValue(":verse", verse);
                //                query.bindValue(":number_in_verse", numberInVerse);
                //                //                                   query.bindValue(":paragraph_number", paragraphNumber);
                //                query.bindValue(":paragraph",paragraph);
                //                query.bindValue(":text", text);
                //                query.bindValue(":morphological_tag", morphologicalTag);
                //                query.bindValue(":normalised_morph_tag", byteArray->toBase64(), QSql::Binary | QSql::In);
                //                query.bindValue(":strongs_number", strongsNumber);
                //                query.bindValue(":strongs_lemma", strongsLemma);
                //                query.bindValue(":friberg_lemma", fribergLemma);

                //                if(!query.exec())
                //                {
                //                    qDebug() << "failed: "<< query.lastError()  << endl;
                //                    exit(1);

                //                }


            }
        }
        else
        {
            qDebug() << "could not open file" << endl;
            exit(1);
        }
    }

    writeOut(db, query);

    query.exec("create index idx_type_t_parsing on type_t_parsing (bibletext_id, word_id)");
    query.exec("create index idx_strongs_word on strongs_word (bibletext_id, word_id, strongs_number, strongs_lemma, friberg_lemma)");
    query.exec("create index idx_bibles on bibles (id, bibletext_id, book_number, normalised_chapter, chapter, verse, parallel)");

    db.commit();
    db.close();
    return 0;
}
