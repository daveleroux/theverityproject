#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H
#include <QString>
#include <QStringList>

extern QString ESV;
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


struct GlobalsHelper
{
    QStringList bookNames;
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
    }

    static GlobalsHelper& instance()
    {
        static GlobalsHelper singleton;
        return singleton;
    }

    QString _nameForBookNumber(int bookNumber)
    {
        return bookNames.at(bookNumber-1);
    }

    static QString nameForBookNumber(int bookNumber)
    {
        return instance()._nameForBookNumber(bookNumber);
    }

};
#endif // GLOBALVARIABLES_H
