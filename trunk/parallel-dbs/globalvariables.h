#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H
#include <QString>
#include <QStringList>
#include <QMap>

extern QString NET;
extern QString KJV;
extern QString WLC;
extern QString TISCH;

extern int GENESIS;
extern int EXODUS;
extern int LEV;
extern int NUM;
extern int DEUT;

extern int ONE_SAM;
extern int TWO_SAM;

extern int ONE_KINGS;
extern int TWO_KINGS;
extern int ONE_CHRON;
extern int TWO_CHRON;

extern int NEH;
extern int JOB;
extern int PSALMS;

extern int ECCL;
extern int SONG;
extern int IS;
extern int JER;

extern int EZ;
extern int DAN;
extern int HOS;
extern int JOEL;

extern int JONAH;
extern int MICAH;
extern int NAHUM;

extern int ZECH;
extern int MAL;

extern int MATTHEW;

extern int JOHN;

extern int TWO_COR;

struct GlobalsHelper
{
    QMap<int, int> netBookNamesToParallel;

    QStringList bookNames;

    QStringList standardBookNames;

    GlobalsHelper()
    {
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

        standardBookNames << "Genesis" << "Exodus" << "Leviticus" << "Numbers" << "Deuteronomy" << "Joshua" << "Judges" << "Ruth" << "1 Samuel" << "2 Samuel" << "1 Kings" << "2 Kings" << "1 Chronicles" << "2 Chronicles" << "Ezra" << "Nehemiah" << "Esther" << "Job" << "Psalms" << "Proverbs" << "Ecclesiastes" << "Song of Songs" << "Isaiah" << "Jeremiah" << "Lamentations" << "Ezekiel" << "Daniel" << "Hosea" << "Joel" << "Amos" << "Obadiah" << "Jonah" << "Micah" << "Nahum" << "Habakkuk" << "Zephaniah" << "Haggai" << "Zechariah" << "Malachi";
        standardBookNames << "Matthew" << "Mark" << "Luke" << "John" << "Acts" << "Romans" << "1 Corinthians" << "2 Corinthians" << "Galatians" << "Ephesians" << "Philippians" << "Colossians" << "1 Thessalonians" << "2 Thessalonians" << "1 Timothy" << "2 Timothy" << "Titus" << "Philemon" << "Hebrews" << "James" << "1 Peter" << "2 Peter" << "1 John" << "2 John" << "3 John" << "Jude" << "Revelation";

    }

    QString _nameForBookNumber(int bookNumber)
    {
        return bookNames.at(bookNumber-1);
    }

    void _insertNetBookNumberAndParallel(int bookNumber, int parallel)
    {
        netBookNamesToParallel.insert(bookNumber, parallel);
    }

    int _getParallelForBookNumber(int bookNumber)
    {
        return netBookNamesToParallel.value(bookNumber);
    }

    QString _standardNameForBookNumber(int bookNumber)
    {
        return standardBookNames.at(bookNumber-1);
    }

    static GlobalsHelper& instance()
    {
        static GlobalsHelper singleton;
        return singleton;
    }    

    static QString nameForBookNumber(int bookNumber)
    {
        return instance()._nameForBookNumber(bookNumber);
    }

    static void insertNetBookNumberAndParallel(int bookNumber, int parallel)
    {
        instance()._insertNetBookNumberAndParallel(bookNumber, parallel);
    }

    static int getParallelForBookNumber(int bookNumber)
    {
        return instance()._getParallelForBookNumber(bookNumber);
    }

    static QString standardNameForBookNumber(int bookNumber)
    {
        return instance()._standardNameForBookNumber(bookNumber);
    }
};
#endif // GLOBALVARIABLES_H
