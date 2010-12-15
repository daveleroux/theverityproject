#include "biblequerier.h"
#include "globalvariables.h"
#include <QtSql>

BibleQuerier::BibleQuerier()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(DATA_PATH + "/bibles.sqlite");
    if (!db.open())
    {
        qDebug() << "couldn't open db" << endl;
        exit(1);
    }
}

VerseLocation* BibleQuerier::_getVerseLocation(QString text, VerseReference verseReference)
{
    QSqlQuery query;

    QString queryString;
    QTextStream(&queryString) << "select normalised_chapter, id from " << text << " where book_number=" << verseReference.book
            << " and chapter=" << verseReference.chapter
            << " and verse=" << verseReference.verse << " and number_in_verse=1";

    if(!query.exec(queryString))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }
    if(query.next())
    {
        int normalisedChapter = query.value(0).toInt();
        int id = query.value(1).toInt();

        return new VerseLocation(id, normalisedChapter);
    }
    else
    {
        qDebug() << "did not find chapter bible verse";
        return 0;
    }
}

TextSpecificData* BibleQuerier::calculateMinAndMaxChapters(QString text)
{
    QSqlQuery query;
    if(!query.exec("select min(normalised_chapter), max(normalised_chapter) from " + text))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

    if(query.next())
    {
        int minChapter = query.value(0).toInt();
        int maxChapter = query.value(1).toInt();
        return new TextSpecificData(text, minChapter, maxChapter);
    }
    else
    {
        qDebug() << "could not find min and max chapters" << endl;
        exit(1);
    }

}

TextSpecificData* BibleQuerier::_getTextSpecificData(QString text)
{
   if(textSpecificDataMap.contains(text))
    {
       return textSpecificDataMap.value(text);
   }
   else
   {
       TextSpecificData* textSpecificData = calculateMinAndMaxChapters(text);
       textSpecificDataMap.insert(text, textSpecificData);
       return textSpecificData;
   }
}

QList<TextInfo> BibleQuerier::_readInChapterData(QString bibleText, int normalisedChapter)
{
    QList<TextInfo> textInfos;

    if(bibleText == "tisch") //DODGY - should remove this kind of thing, needs some thinking
    {
        QSqlQuery query;
        query.setForwardOnly(true);

        if(!query.exec("select id, book, chapter, verse, number_in_verse, paragraph, text "
                       ", morphological_tag, normalised_morph_tag, strongs_number, strongs_lemma, friberg_lemma "
                       "from tisch where normalised_chapter = "
                       + QString().setNum(normalisedChapter) +
                       " order by id asc"))


        {
            qDebug() << "failed: " << query.lastError() << endl;
            exit(1);
        }

        while(query.next())
        {
            int id = query.value(0).toInt();
            QString book = query.value(1).toString();
            int chapter = query.value(2).toInt();
            int verse = query.value(3).toInt();
            int numberInVerse = query.value(4).toInt();
            bool paragraph = query.value(5).toBool();
            QString text = query.value(6).toString();
            QString morphologicalTag = query.value(7).toString();


            QByteArray normalisedMorphTagBytes = QByteArray::fromBase64(query.value(8).toByteArray());
            QDataStream stream(normalisedMorphTagBytes);
            QBitArray normalisedMorphTag(81);
            stream >> normalisedMorphTag;

            int strongsNumber = query.value(9).toInt();
            QString strongsLemma = query.value(10).toString();
            QString fribergLemma = query.value(11).toString();

            TextInfo textInfo(bibleText, id, book, chapter, verse, numberInVerse, paragraph, text, morphologicalTag, normalisedMorphTag, strongsNumber, strongsLemma, fribergLemma);
            textInfos.append(textInfo);
        }
    }
    else if(bibleText == "wlc")
    {
        QSqlQuery query;
        query.setForwardOnly(true);

        if(!query.exec("select id, book, chapter, verse, number_in_verse, paragraph, text "
                       ", morphological_tag, normalised_morph_tag, strongs_number "
                       "from wlc where normalised_chapter = "
                       + QString().setNum(normalisedChapter) +
                       " order by id asc"))


        {
            qDebug() << "failed: " << query.lastError() << endl;
            exit(1);
        }

        while(query.next())
        {
            int id = query.value(0).toInt();
            QString book = query.value(1).toString();
            int chapter = query.value(2).toInt();
            int verse = query.value(3).toInt();
            int numberInVerse = query.value(4).toInt();
            bool paragraph = query.value(5).toBool();
            QString text = query.value(6).toString();


            int strongsNumber = query.value(9).toInt();

            TextInfo textInfo(bibleText, id, book, chapter, verse, numberInVerse, paragraph, text, "", QBitArray(), strongsNumber, "", "");
            textInfos.append(textInfo);
        }
    }
    else if(bibleText == "esv")
    {
        QSqlQuery query;
        query.setForwardOnly(true);

        if(!query.exec("select id, book, chapter, verse, number_in_verse, paragraph, text "
                       ", morphological_tag, normalised_morph_tag, strongs_number "
                       "from esv where normalised_chapter = "
                       + QString().setNum(normalisedChapter) +
                       " order by id asc"))


        {
            qDebug() << "failed: " << query.lastError() << endl;
            exit(1);
        }

        while(query.next())
        {
            int id = query.value(0).toInt();
            QString book = query.value(1).toString();
            int chapter = query.value(2).toInt();
            int verse = query.value(3).toInt();
            int numberInVerse = query.value(4).toInt();
            bool paragraph = query.value(5).toBool();
            QString text = query.value(6).toString();

            TextInfo textInfo(bibleText, id, book, chapter, verse, numberInVerse, paragraph, text, "", QBitArray(), 0, "", "");
            textInfos.append(textInfo);
        }
    }
    return textInfos;
}


BibleQuerier& BibleQuerier::instance()
{
    static BibleQuerier singleton;
    return singleton;
}

QList<TextInfo> BibleQuerier::readInChapterData(QString text, int normalisedChapter)
{
    return instance()._readInChapterData(text, normalisedChapter);
}

VerseLocation* BibleQuerier::getVerseLocation(QString text, VerseReference verseReference)
{
    return instance()._getVerseLocation(text, verseReference);
}

TextSpecificData* BibleQuerier::getTextSpecificData(QString text)
{
    return instance()._getTextSpecificData(text);
}

