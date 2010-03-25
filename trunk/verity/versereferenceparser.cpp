#include "versereferenceparser.h"
#include <QDebug>

VerseReferenceParser::VerseReferenceParser()
{
    //should prob split this up, e.g. Thess, Th, ... maps to Thessalonians
    //and then one, i ... maps to 1

    hash.insert( "matthew" ,  1 );
    hash.insert( "matt" ,  1 );
    hash.insert( "mt" ,  1 );
    hash.insert( "mark" ,  2 );
    hash.insert( "mrk" ,  2 );
    hash.insert( "mk" ,  2 );
    hash.insert( "mr" ,  2 );
    hash.insert( "luke" ,  3 );
    hash.insert( "luk" ,  3 );
    hash.insert( "lk" ,  3 );
    hash.insert( "john" ,  4 );
    hash.insert( "joh" ,  4 );
    hash.insert( "jn" ,  4 );
    hash.insert( "jhn" ,  4 );
    hash.insert( "acts" ,  5 );
    hash.insert( "ac" ,  5 );
    hash.insert( "romans" ,  6 );
    hash.insert( "rom" ,  6 );
    hash.insert( "ro" ,  6 );
    hash.insert( "rm" ,  6 );
    hash.insert( "1 corinthians" ,  7 );
    hash.insert( "1 cor" ,  7 );
    hash.insert( "1 co" ,  7 );
    hash.insert( "i co" ,  7 );
    hash.insert( "1co" ,  7 );
    hash.insert( "i cor" ,  7 );
    hash.insert( "1cor" ,  7 );
    hash.insert( "i corinthians" ,  7 );
    hash.insert( "1corinthians" ,  7 );
    hash.insert( "1st corinthians" ,  7 );
    hash.insert( "first corinthians" ,  7 );
    hash.insert( "2 corinthians" ,  8 );
    hash.insert( "2 cor" ,  8 );
    hash.insert( "2 co" ,  8 );
    hash.insert( "ii co" ,  8 );
    hash.insert( "2co" ,  8 );
    hash.insert( "ii cor" ,  8 );
    hash.insert( "2cor" ,  8 );
    hash.insert( "ii corinthians" ,  8 );
    hash.insert( "2corinthians" ,  8 );
    hash.insert( "2nd corinthians" ,  8 );
    hash.insert( "second corinthians" ,  8 );
    hash.insert( "galatians" ,  9 );
    hash.insert( "gal" ,  9 );
    hash.insert( "ga" ,  9 );
    hash.insert( "ephesians" ,  10 );
    hash.insert( "ephes" ,  10 );
    hash.insert( "eph" ,  10 );
    hash.insert( "philippians" ,  11 );
    hash.insert( "phil" ,  11 );
    hash.insert( "php" ,  11 );
    hash.insert( "colossians" ,  12 );
    hash.insert( "col" ,  12 );
    hash.insert( "1 thessalonians" ,  13 );
    hash.insert( "1 thess" ,  13 );
    hash.insert( "1 th" ,  13 );
    hash.insert( "i th" ,  13 );
    hash.insert( "1th" ,  13 );
    hash.insert( "i thes" ,  13 );
    hash.insert( "1thes" ,  13 );
    hash.insert( "i thess" ,  13 );
    hash.insert( "1thess" ,  13 );
    hash.insert( "i thessalonians" ,  13 );
    hash.insert( "1thessalonians" ,  13 );
    hash.insert( "1st thessalonians" ,  13 );
    hash.insert( "first thessalonians" ,  13 );
    hash.insert( "2 thessalonians" ,  14 );
    hash.insert( "2 thess" ,  14 );
    hash.insert( "2 th" ,  14 );
    hash.insert( "ii th" ,  14 );
    hash.insert( "2th" ,  14 );
    hash.insert( "ii thes" ,  14 );
    hash.insert( "2thes" ,  14 );
    hash.insert( "ii thess" ,  14 );
    hash.insert( "2thess" ,  14 );
    hash.insert( "ii thessalonians" ,  14 );
    hash.insert( "2thessalonians" ,  14 );
    hash.insert( "2nd thessalonians" ,  14 );
    hash.insert( "second thessalonians" ,  14 );
    hash.insert( "1 timothy" ,  15 );
    hash.insert( "1 tim" ,  15 );
    hash.insert( "1 ti" ,  15 );
    hash.insert( "i ti" ,  15 );
    hash.insert( "1ti" ,  15 );
    hash.insert( "i tim" ,  15 );
    hash.insert( "1tim" ,  15 );
    hash.insert( "i timothy" ,  15 );
    hash.insert( "1timothy" ,  15 );
    hash.insert( "1st timothy" ,  15 );
    hash.insert( "first timothy" ,  15 );
    hash.insert( "2 timothy" ,  16 );
    hash.insert( "2 tim" ,  16 );
    hash.insert( "2 ti" ,  16 );
    hash.insert( "ii ti" ,  16 );
    hash.insert( "2ti" ,  16 );
    hash.insert( "ii tim" ,  16 );
    hash.insert( "2tim" ,  16 );
    hash.insert( "ii timothy" ,  16 );
    hash.insert( "2timothy" ,  16 );
    hash.insert( "2nd timothy" ,  16 );
    hash.insert( "second timothy" ,  16 );
    hash.insert( "titus" ,  17 );
    hash.insert( "tit" ,  17 );
    hash.insert( "philemon" ,  18 );
    hash.insert( "philem" ,  18 );
    hash.insert( "phm" ,  18 );
    hash.insert( "hebrews" ,  19 );
    hash.insert( "heb" ,  19 );
    hash.insert( "james" ,  20 );
    hash.insert( "jas" ,  20 );
    hash.insert( "jm" ,  20 );
    hash.insert( "1 peter" ,  21 );
    hash.insert( "1 pet" ,  21 );
    hash.insert( "1 pe" ,  21 );
    hash.insert( "i pe" ,  21 );
    hash.insert( "1pe" ,  21 );
    hash.insert( "i pet" ,  21 );
    hash.insert( "1pet" ,  21 );
    hash.insert( "i pt" ,  21 );
    hash.insert( "1 pt" ,  21 );
    hash.insert( "1pt" ,  21 );
    hash.insert( "i peter" ,  21 );
    hash.insert( "1peter" ,  21 );
    hash.insert( "1st peter" ,  21 );
    hash.insert( "first peter" ,  21 );
    hash.insert( "2 peter" ,  22 );
    hash.insert( "2 pet" ,  22 );
    hash.insert( "2 pe" ,  22 );
    hash.insert( "ii pe" ,  22 );
    hash.insert( "2pe" ,  22 );
    hash.insert( "ii pet" ,  22 );
    hash.insert( "2pet" ,  22 );
    hash.insert( "ii pt" ,  22 );
    hash.insert( "2 pt" ,  22 );
    hash.insert( "2pt" ,  22 );
    hash.insert( "ii peter" ,  22 );
    hash.insert( "2peter" ,  22 );
    hash.insert( "2nd peter" ,  22 );
    hash.insert( "second peter" ,  22 );
    hash.insert( "1 john" ,  23 );
    hash.insert( "1 jn" ,  23 );
    hash.insert( "i jn" ,  23 );
    hash.insert( "1jn" ,  23 );
    hash.insert( "i jo" ,  23 );
    hash.insert( "1jo" ,  23 );
    hash.insert( "i joh" ,  23 );
    hash.insert( "1joh" ,  23 );
    hash.insert( "i jhn" ,  23 );
    hash.insert( "1 jhn" ,  23 );
    hash.insert( "1jhn" ,  23 );
    hash.insert( "i john" ,  23 );
    hash.insert( "1john" ,  23 );
    hash.insert( "1st john" ,  23 );
    hash.insert( "first john" ,  23 );
    hash.insert( "2 john" ,  24 );
    hash.insert( "2 jn" ,  24 );
    hash.insert( "ii jn" ,  24 );
    hash.insert( "2jn" ,  24 );
    hash.insert( "ii jo" ,  24 );
    hash.insert( "2jo" ,  24 );
    hash.insert( "ii joh" ,  24 );
    hash.insert( "2joh" ,  24 );
    hash.insert( "ii jhn" ,  24 );
    hash.insert( "2 jhn" ,  24 );
    hash.insert( "2jhn" ,  24 );
    hash.insert( "ii john" ,  24 );
    hash.insert( "2john" ,  24 );
    hash.insert( "2nd john" ,  24 );
    hash.insert( "second john" ,  24 );
    hash.insert( "3 john" ,  25 );
    hash.insert( "3 jn" ,  25 );
    hash.insert( "iii jn" ,  25 );
    hash.insert( "3jn" ,  25 );
    hash.insert( "iii jo" ,  25 );
    hash.insert( "3jo" ,  25 );
    hash.insert( "iii joh" ,  25 );
    hash.insert( "3joh" ,  25 );
    hash.insert( "iii jhn" ,  25 );
    hash.insert( "3 jhn" ,  25 );
    hash.insert( "3jhn" ,  25 );
    hash.insert( "iii john" ,  25 );
    hash.insert( "3john" ,  25 );
    hash.insert( "3rd john" ,  25 );
    hash.insert( "third john" ,  25 );
    hash.insert( "jude" ,  26 );
    hash.insert( "jud" ,  26 );
    hash.insert( "revelation" ,  27 );
    hash.insert( "rev" ,  27 );
    hash.insert( "re" ,  27 );
    hash.insert( "the revelation" ,  27 );

    normalisedBookNames.insert(1, "Matthew"    );
    normalisedBookNames.insert(2, "Mark"    );
    normalisedBookNames.insert(3, "Luke"    );
    normalisedBookNames.insert(4, "John"    );
    normalisedBookNames.insert(5, "Acts" );
    normalisedBookNames.insert(6, "Romans"  );
    normalisedBookNames.insert(7, "1 Corinthians"  );
    normalisedBookNames.insert(8, "2 Corinthians"  );
    normalisedBookNames.insert(9, "Galatians"  );
    normalisedBookNames.insert(10, "Ephesians"  );
    normalisedBookNames.insert(11, "Philippians"  );
    normalisedBookNames.insert(12, "Colossians"  );
    normalisedBookNames.insert(13, "1 Thessalonians"  );
    normalisedBookNames.insert(14, "2 Thessalonians"  );
    normalisedBookNames.insert(15, "1 Timothy"  );
    normalisedBookNames.insert(16, "2 Timothy"  );
    normalisedBookNames.insert(17, "Titus"  );
    normalisedBookNames.insert(18, "Philemon"  );
    normalisedBookNames.insert(19, "Hebrews"  );
    normalisedBookNames.insert(20, "James"  );
    normalisedBookNames.insert(21, "1 Peter" );
    normalisedBookNames.insert(22, "2 Peter" );
    normalisedBookNames.insert(23, "1 John"  );
    normalisedBookNames.insert(24, "2 John"  );
    normalisedBookNames.insert(25, "3 John"  );
    normalisedBookNames.insert(26, "Jude" );
    normalisedBookNames.insert(27, "Revelation"  );

}

QString VerseReferenceParser::calculateStringRepresentation(int book, int chapter, int verse)
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

        while(string.at(numeralIndex-1).digitValue() != -1)
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

    return VerseReference(book,chapter,verse, calculateStringRepresentation(book, chapter, verse));
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

