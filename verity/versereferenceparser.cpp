#include "versereferenceparser.h"
#include <QDebug>

VerseReferenceParser::VerseReferenceParser()
{
    //should prob split this up, e.g. Thess, Th, ... maps to Thessalonians
    //and then one, i ... maps to 1

    hash.insert("genesis", 1   );
    hash.insert("gen", 1   );
    hash.insert("exodus", 2    );
    hash.insert("ex", 2    );
    hash.insert("leviticus",3    );
    hash.insert("lev",3    );
    hash.insert("numbers",4    );
    hash.insert("num",4    );
    hash.insert("deuteronomy",5    );
    hash.insert("deut",5    );
    hash.insert("joshua",6    );
    hash.insert("josh",6    );
    hash.insert("judges",7    );
    hash.insert("jud",7    );
    hash.insert("ruth",8    );
    hash.insert("rut",8    );
    hash.insert("1 samuel",9    );
    hash.insert("1sam",9    );
    hash.insert("2 samuel",10    );
    hash.insert("2sam",10    );
    hash.insert("1 kings",11    );
    hash.insert("1ki",11    );
    hash.insert("2 kings",12    );
    hash.insert("2ki",12    );
    hash.insert("1 chronicles",13    );
    hash.insert("1ch",13    );
    hash.insert("2 chronicles",14    );
    hash.insert("2ch",14    );
    hash.insert("ezra",15    );
    hash.insert("ez",15    );
    hash.insert("nehemiah",16    );
    hash.insert("neh",16    );
    hash.insert("esther",17    );
    hash.insert("est",17    );
    hash.insert("job",18    );
    hash.insert("psalms",19    );
    hash.insert("ps",19    );
    hash.insert("proverbs",20    );
    hash.insert("pr",20    );
    hash.insert("ecclesiastes",21    );
    hash.insert("ecc",21    );
    hash.insert("song of songs",22    );
    hash.insert("ss",22    );
    hash.insert("isaiah",23    );
    hash.insert("is",23    );
    hash.insert("jeremiah",24    );
    hash.insert("jer",24    );
    hash.insert("lamentations",25    );
    hash.insert("lam",25    );
    hash.insert("ezekiel",26    );
    hash.insert("ez",26    );
    hash.insert("daniel",27    );
    hash.insert("dan",27    );
    hash.insert("hosea",28    );
    hash.insert("hos",28    );
    hash.insert("joel",29    );
    hash.insert("amos",30    );
    hash.insert("obadiah",31    );
    hash.insert("ob",31    );
    hash.insert("jonah",32    );
    hash.insert("micah",33    );
    hash.insert("mic",33    );
    hash.insert("nahum",34    );
    hash.insert("nah",34    );
    hash.insert("habakkuk",35    );
    hash.insert("hab",35    );
    hash.insert("zephaniah",36    );
    hash.insert("zep",36    );
    hash.insert("haggai",37    );
    hash.insert("hag",37    );
    hash.insert("zechariah",38    );
    hash.insert("zec",38    );
    hash.insert("malachi",39    );
    hash.insert("mal",39    );


    hash.insert( "matthew" ,  40 );
    hash.insert( "matt" ,  40 );
    hash.insert( "mt" ,  40 );
    hash.insert( "mark" ,  41 );
    hash.insert( "mrk" ,  41 );
    hash.insert( "mk" ,  41 );
    hash.insert( "mr" ,  41 );
    hash.insert( "luke" ,  42 );
    hash.insert( "luk" ,  42 );
    hash.insert( "lk" ,  42 );
    hash.insert( "john" ,  43 );
    hash.insert( "joh" ,  43 );
    hash.insert( "jn" ,  43 );
    hash.insert( "jhn" ,  43 );
    hash.insert( "acts" ,  44 );
    hash.insert( "ac" ,  44 );
    hash.insert( "romans" ,  45 );
    hash.insert( "rom" ,  45 );
    hash.insert( "ro" ,  45 );
    hash.insert( "rm" ,  45 );
    hash.insert( "1 corinthians" ,  46 );
    hash.insert( "1 cor" ,  46 );
    hash.insert( "1 co" ,  46 );
    hash.insert( "i co" ,  46 );
    hash.insert( "1co" ,  46 );
    hash.insert( "i cor" ,  46 );
    hash.insert( "1cor" ,  46 );
    hash.insert( "i corinthians" ,  46 );
    hash.insert( "1corinthians" ,  46 );
    hash.insert( "1st corinthians" ,  46 );
    hash.insert( "first corinthians" ,  46 );
    hash.insert( "2 corinthians" ,  47 );
    hash.insert( "2 cor" ,  47 );
    hash.insert( "2 co" ,  47 );
    hash.insert( "ii co" ,  47 );
    hash.insert( "2co" ,  47 );
    hash.insert( "ii cor" ,  47 );
    hash.insert( "2cor" ,  47 );
    hash.insert( "ii corinthians" ,  47 );
    hash.insert( "2corinthians" ,  47 );
    hash.insert( "2nd corinthians" ,  47 );
    hash.insert( "second corinthians" ,  47 );
    hash.insert( "galatians" ,  48 );
    hash.insert( "gal" ,  48 );
    hash.insert( "ga" ,  48 );
    hash.insert( "ephesians" ,  49 );
    hash.insert( "ephes" ,  49 );
    hash.insert( "eph" ,  49 );
    hash.insert( "philippians" ,  50 );
    hash.insert( "phil" ,  50 );
    hash.insert( "php" ,  50 );
    hash.insert( "colossians" ,  51 );
    hash.insert( "col" ,  51 );
    hash.insert( "1 thessalonians" ,  52 );
    hash.insert( "1 thess" ,  52 );
    hash.insert( "1 thes" ,  52 );
    hash.insert( "1 th" ,  52 );
    hash.insert( "i th" ,  52 );
    hash.insert( "1th" ,  52 );
    hash.insert( "i thes" ,  52 );
    hash.insert( "1thes" ,  52 );
    hash.insert( "i thess" ,  52 );
    hash.insert( "1thess" ,  52 );
    hash.insert( "i thessalonians" ,  52 );
    hash.insert( "1thessalonians" ,  52 );
    hash.insert( "1st thessalonians" ,  52 );
    hash.insert( "first thessalonians" ,  52 );
    hash.insert( "2 thessalonians" ,  53 );
    hash.insert( "2 thess" ,  53 );
    hash.insert( "2 th" ,  53 );
    hash.insert( "ii th" ,  53 );
    hash.insert( "2th" ,  53 );
    hash.insert( "ii thes" ,  53 );
    hash.insert( "2thes" ,  53 );
    hash.insert( "ii thess" ,  53 );
    hash.insert( "2thess" ,  53 );
    hash.insert( "ii thessalonians" ,  53 );
    hash.insert( "2thessalonians" ,  53 );
    hash.insert( "2nd thessalonians" ,  53 );
    hash.insert( "second thessalonians" ,  53 );
    hash.insert( "1 timothy" ,  54 );
    hash.insert( "1 tim" ,  54 );
    hash.insert( "1 ti" ,  54 );
    hash.insert( "i ti" ,  54 );
    hash.insert( "1ti" ,  54 );
    hash.insert( "i tim" ,  54 );
    hash.insert( "1tim" ,  54 );
    hash.insert( "i timothy" ,  54 );
    hash.insert( "1timothy" ,  54 );
    hash.insert( "1st timothy" ,  54 );
    hash.insert( "first timothy" ,  54 );
    hash.insert( "2 timothy" ,  55 );
    hash.insert( "2 tim" ,  55 );
    hash.insert( "2 ti" ,  55 );
    hash.insert( "ii ti" ,  55 );
    hash.insert( "2ti" ,  55 );
    hash.insert( "ii tim" ,  55 );
    hash.insert( "2tim" ,  55 );
    hash.insert( "ii timothy" ,  55 );
    hash.insert( "2timothy" ,  55 );
    hash.insert( "2nd timothy" ,  55 );
    hash.insert( "second timothy" ,  55 );
    hash.insert( "titus" ,  56 );
    hash.insert( "tit" ,  56 );
    hash.insert( "philemon" ,  57 );
    hash.insert( "philem" ,  57 );
    hash.insert( "phm" ,  57 );
    hash.insert( "hebrews" ,  58 );
    hash.insert( "heb" ,  58 );
    hash.insert( "james" ,  59 );
    hash.insert( "jas" ,  59 );
    hash.insert( "jm" ,  59 );
    hash.insert( "1 peter" ,  60 );
    hash.insert( "1 pet" ,  60 );
    hash.insert( "1 pe" ,  60 );
    hash.insert( "i pe" ,  60 );
    hash.insert( "1pe" ,  60 );
    hash.insert( "i pet" ,  60 );
    hash.insert( "1pet" ,  60 );
    hash.insert( "i pt" ,  60 );
    hash.insert( "1 pt" ,  60 );
    hash.insert( "1pt" ,  60 );
    hash.insert( "i peter" ,  60 );
    hash.insert( "1peter" ,  60 );
    hash.insert( "1st peter" ,  60 );
    hash.insert( "first peter" ,  60 );
    hash.insert( "2 peter" ,  61 );
    hash.insert( "2 pet" ,  61 );
    hash.insert( "2 pe" ,  61 );
    hash.insert( "ii pe" ,  61 );
    hash.insert( "2pe" ,  61 );
    hash.insert( "ii pet" ,  61 );
    hash.insert( "2pet" ,  61 );
    hash.insert( "ii pt" ,  61 );
    hash.insert( "2 pt" ,  61 );
    hash.insert( "2pt" ,  61 );
    hash.insert( "ii peter" ,  61 );
    hash.insert( "2peter" ,  61 );
    hash.insert( "2nd peter" ,  61 );
    hash.insert( "second peter" ,  61 );
    hash.insert( "1 john" ,  62 );
    hash.insert( "1 jn" ,  62 );
    hash.insert( "i jn" ,  62 );
    hash.insert( "1jn" ,  62 );
    hash.insert( "i jo" ,  62 );
    hash.insert( "1jo" ,  62 );
    hash.insert( "i joh" ,  62 );
    hash.insert( "1joh" ,  62 );
    hash.insert( "i jhn" ,  62 );
    hash.insert( "1 jhn" ,  62 );
    hash.insert( "1jhn" ,  62 );
    hash.insert( "i john" ,  62 );
    hash.insert( "1john" ,  62 );
    hash.insert( "1st john" ,  62 );
    hash.insert( "first john" ,  62 );
    hash.insert( "2 john" ,  63 );
    hash.insert( "2 jn" ,  63 );
    hash.insert( "ii jn" ,  63 );
    hash.insert( "2jn" ,  63 );
    hash.insert( "ii jo" ,  63 );
    hash.insert( "2jo" ,  63 );
    hash.insert( "ii joh" ,  63 );
    hash.insert( "2joh" ,  63 );
    hash.insert( "ii jhn" ,  63 );
    hash.insert( "2 jhn" ,  63 );
    hash.insert( "2jhn" ,  63 );
    hash.insert( "ii john" ,  63 );
    hash.insert( "2john" ,  63 );
    hash.insert( "2nd john" ,  63 );
    hash.insert( "second john" ,  63 );
    hash.insert( "3 john" ,  64 );
    hash.insert( "3 jn" ,  64 );
    hash.insert( "iii jn" ,  64 );
    hash.insert( "3jn" ,  64 );
    hash.insert( "iii jo" ,  64 );
    hash.insert( "3jo" ,  64 );
    hash.insert( "iii joh" ,  64 );
    hash.insert( "3joh" ,  64 );
    hash.insert( "iii jhn" ,  64 );
    hash.insert( "3 jhn" ,  64 );
    hash.insert( "3jhn" ,  64 );
    hash.insert( "iii john" ,  64 );
    hash.insert( "3john" ,  64 );
    hash.insert( "3rd john" ,  64 );
    hash.insert( "third john" ,  64 );
    hash.insert( "jude" ,  65 );
    hash.insert( "jud" ,  65 );
    hash.insert( "revelation" ,  66);
    hash.insert( "rev" ,  66 );
    hash.insert( "re" ,  66 );
    hash.insert( "the revelation" ,  66 );

    normalisedBookNames.insert(1, "Genesis"    );
    normalisedBookNames.insert(2, "Exodus"    );
    normalisedBookNames.insert(3, "Leviticus"    );
    normalisedBookNames.insert(4, "Numbers"    );
    normalisedBookNames.insert(5, "Deuteronomy"    );
    normalisedBookNames.insert(6, "Joshua"    );
    normalisedBookNames.insert(7, "Judges"    );
    normalisedBookNames.insert(8, "Ruth"    );
    normalisedBookNames.insert(9, "1 Samuel"    );
    normalisedBookNames.insert(10, "2 Samuel"    );
    normalisedBookNames.insert(11, "1 Kings"    );
    normalisedBookNames.insert(12, "2 Kings"    );
    normalisedBookNames.insert(13, "1 Chronicles"    );
    normalisedBookNames.insert(14, "2 Chronicles"    );
    normalisedBookNames.insert(15, "Ezra"    );
    normalisedBookNames.insert(16, "Nehemiah"    );
    normalisedBookNames.insert(17, "Esther"    );
    normalisedBookNames.insert(18, "Job"    );
    normalisedBookNames.insert(19, "Psalms"    );
    normalisedBookNames.insert(20, "Proverbs"    );
    normalisedBookNames.insert(21, "Ecclesiastes"    );
    normalisedBookNames.insert(22, "Song of Songs"    );
    normalisedBookNames.insert(23, "Isaiah"    );
    normalisedBookNames.insert(24, "Jeremiah"    );
    normalisedBookNames.insert(25, "Lamentations"    );
    normalisedBookNames.insert(26, "Ezekiel"    );
    normalisedBookNames.insert(27, "Daniel"    );
    normalisedBookNames.insert(28, "Hosea"    );
    normalisedBookNames.insert(29, "Joel"    );
    normalisedBookNames.insert(30, "Amos"    );
    normalisedBookNames.insert(31, "Obadiah"    );
    normalisedBookNames.insert(32, "Jonah"    );
    normalisedBookNames.insert(33, "Micah"    );
    normalisedBookNames.insert(34, "Nahum"    );
    normalisedBookNames.insert(35, "Habakkuk"    );
    normalisedBookNames.insert(36, "Zephaniah"    );
    normalisedBookNames.insert(37, "Haggai"    );
    normalisedBookNames.insert(38, "Zechariah"    );
    normalisedBookNames.insert(39, "Malachi"    );

    normalisedBookNames.insert(40, "Matthew"    );
    normalisedBookNames.insert(41, "Mark"    );
    normalisedBookNames.insert(42, "Luke"    );
    normalisedBookNames.insert(43, "John"    );
    normalisedBookNames.insert(44, "Acts" );
    normalisedBookNames.insert(45, "Romans"  );
    normalisedBookNames.insert(46, "1 Corinthians"  );
    normalisedBookNames.insert(47, "2 Corinthians"  );
    normalisedBookNames.insert(48, "Galatians"  );
    normalisedBookNames.insert(49, "Ephesians"  );
    normalisedBookNames.insert(50, "Philippians"  );
    normalisedBookNames.insert(51, "Colossians"  );
    normalisedBookNames.insert(52, "1 Thessalonians"  );
    normalisedBookNames.insert(53, "2 Thessalonians"  );
    normalisedBookNames.insert(54, "1 Timothy"  );
    normalisedBookNames.insert(55, "2 Timothy"  );
    normalisedBookNames.insert(56, "Titus"  );
    normalisedBookNames.insert(57, "Philemon"  );
    normalisedBookNames.insert(58, "Hebrews"  );
    normalisedBookNames.insert(59, "James"  );
    normalisedBookNames.insert(60, "1 Peter" );
    normalisedBookNames.insert(61, "2 Peter" );
    normalisedBookNames.insert(62, "1 John"  );
    normalisedBookNames.insert(63, "2 John"  );
    normalisedBookNames.insert(64, "3 John"  );
    normalisedBookNames.insert(65, "Jude" );
    normalisedBookNames.insert(66, "Revelation"  );

}

QString VerseReferenceParser::_calculateStringRepresentation(int book, int chapter, int verse)
{    
    return  normalisedBookNames.value(book) + " " + QString().setNum(chapter) + ":" + QString().setNum(verse);
}

VerseReference VerseReferenceParser::_parse(QString string)
{
    string = string.trimmed();

    int book = 1;
    int chapter = 1;
    int verse = 1;
    QString bookString = string;

    int separatorIndex = qMax(string.indexOf("."), string.indexOf(":"));
    if(separatorIndex == -1)
    {
        string.append(".1");
    }

    separatorIndex = qMax(string.indexOf("."), string.indexOf(":"));

    bool ok=false;
    int verseTest = string.mid(separatorIndex+1).toInt(&ok);
    if(ok)
        verse = verseTest;

    int numeralIndex = separatorIndex;

    while(numeralIndex>0 && string.at(numeralIndex-1).digitValue() != -1)
    {
        numeralIndex = numeralIndex - 1;
    }

    int chapterTest = string.mid(numeralIndex, separatorIndex-numeralIndex).toInt(&ok);
    if(ok)
        chapter = chapterTest;

    bookString = string.mid(0, numeralIndex);


    bookString = bookString.trimmed().toLower();

    if(hash.contains(bookString))
    {
        book = hash.value(bookString);
    }

    return VerseReference(book,chapter,verse, _calculateStringRepresentation(book, chapter, verse));
}

VerseReferenceParser& VerseReferenceParser::instance()
{
    static VerseReferenceParser singleton;
    return singleton;
}

VerseReference VerseReferenceParser::parse(QString string)
{
    return instance()._parse(string);
}

QString VerseReferenceParser::calculateStringRepresentation(int book, int chapter, int verse)
{
    return instance()._calculateStringRepresentation(book, chapter, verse);
}
