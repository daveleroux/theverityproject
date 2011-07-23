#include "biblequerier.h"
#include "globalvariables.h"
#include <QtSql>
#include "timer.h"
#include "minandmaxidsforchapter.h"
#include "versereferenceparser.h"
#include <QStringList>
#include <iostream>

using namespace std;

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
        return 0;
    }
}

TextSpecificData* BibleQuerier::__getTextSpecificData(QString text)
{
    int minChapter;
    int maxChapter;

    QSqlQuery query;
    if(!query.exec("select min(normalised_chapter), max(normalised_chapter) from " + text))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

    if(query.next())
    {
        minChapter = query.value(0).toInt();
        maxChapter = query.value(1).toInt();
    }
    else
    {
        qDebug() << "could not find min and max chapters" << endl;
        exit(1);
    }


    QHash<int, MinAndMaxIds> minAndMaxChapterHash;

    //    if(!query.exec("select min(id), max(id), normalised_chapter from "+text+" group by normalised_chapter"))
    if(!query.exec("select min_id, max_id, normalised_chapter from performance where text=\""+text+"\""))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

    while(query.next())
    {
        int min = query.value(0).toInt();
        int max = query.value(1).toInt();
        int chapter = query.value(2).toInt();
        MinAndMaxIds minAndMaxIds(min, max);
        minAndMaxChapterHash.insert(chapter, minAndMaxIds);
    }
//    else
//    {
//        qDebug() << "could not find min and max ids for normalised chapters" << endl;
//        exit(1);
//    }

    return new TextSpecificData(text, minChapter, maxChapter, minAndMaxChapterHash);
}

TextSpecificData* BibleQuerier::_getTextSpecificData(QString text)
{
    if(textSpecificDataMap.contains(text))
    {
        return textSpecificDataMap.value(text);
    }
    else
    {
        TextSpecificData* textSpecificData = __getTextSpecificData(text);
        textSpecificDataMap.insert(text, textSpecificData);
        return textSpecificData;
    }
}

QString BibleQuerier::asString(QList<int> list)
{
    QString string;
    foreach (int value, list)
    {
        string.append(QString().setNum(value) + ",");
    }
    string.remove(string.length()-1, 1);

    return string;
}

QList<TextInfo> BibleQuerier::_readInChapterDataForParallel(QString bibleText, QSet<int> parallelSet, int idToInclude)
{
//    timer t;
//    t.start();

    QList<TextInfo> result;


    QSqlQuery query;
    query.setForwardOnly(true);

    query.prepare("select min(id), max(id) from " + bibleText + " where parallel in ("+asString(parallelSet.toList())+") ");

    if(!query.exec())
    {
        qDebug() << "failed: " << query.lastError();
        exit(1);
    }
    query.next();
    int min  = query.value(0).toInt();
    int max =  query.value(1).toInt();

//    cout << "\t min and max:" << t << endl;

    if(min > 0 && max > 0)
    {
        if(idToInclude != -1)
        {
            if(idToInclude < min)
                min = idToInclude;
            if(idToInclude > max)
                max = idToInclude;
        }

        if(bibleText == "tisch")
            return readInTisch(min, max);
        else if (bibleText == "wlc")
            return readInWlc(min, max);
        else if (bibleText == "esv" || bibleText == "kjv")
            return readInEsvOrKjv(min, max, bibleText);

    }
    return result;
}

QList<TextInfo> BibleQuerier::readInTisch(int idFrom, int idTo) //must clean up
{
    QList<TextInfo> textInfos;

    QSqlQuery query;
    query.setForwardOnly(true);

    if(!query.exec("select id, book, chapter, verse, number_in_verse, paragraph, text "
                   ", morphological_tag, normalised_morph_tag, strongs_number, strongs_lemma, friberg_lemma, parallel "
                   "from tisch where id >= "
                   + QString().setNum(idFrom) +
                   " and id <= " + QString().setNum(idTo) +
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
        int parallel = query.value(12).toInt();

        TextInfo textInfo("tisch",
                          id,
                          book,
                          chapter,
                          verse,
                          numberInVerse,
                          paragraph,
                          text,
                          morphologicalTag,
                          normalisedMorphTag,
                          strongsNumber,
                          strongsLemma,
                          fribergLemma,
                          parallel);
        textInfos.append(textInfo);
    }

    return textInfos;
}

QList<TextInfo> BibleQuerier::readInWlc(int idFrom, int idTo) //must clean up
{
    QList<TextInfo> textInfos;

    QSqlQuery query;
    query.setForwardOnly(true);

    if(!query.exec("select id, book, chapter, verse, number_in_verse, paragraph, text "
                   ", morphological_tag, normalised_morph_tag, strongs_number, parallel "
                   "from wlc where id >= "
                   + QString().setNum(idFrom) +
                   " and id <= " + QString().setNum(idTo) +
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
        int parallel = query.value(10).toInt();

        TextInfo textInfo("wlc", id, book, chapter, verse, numberInVerse, paragraph, text, "", QBitArray(), strongsNumber, "", "", parallel);
        textInfos.append(textInfo);
    }
    return textInfos;
}

QList<TextInfo> BibleQuerier::readInEsvOrKjv(int idFrom, int idTo, QString bibleText) //must clean up
{
    QList<TextInfo> textInfos;

    QSqlQuery query;
    query.setForwardOnly(true);

    if(!query.exec("select id, book, chapter, verse, number_in_verse, paragraph, text "
                   ", morphological_tag, normalised_morph_tag, strongs_number, parallel "
                   "from " + bibleText + " where id >= "
                   + QString().setNum(idFrom) +
                   " and id <= " + QString().setNum(idTo) +
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
        int parallel = query.value(10).toInt();

        TextInfo textInfo(bibleText, id, book, chapter, verse, numberInVerse, paragraph, text, "", QBitArray(), 0, "", "", parallel);
        textInfos.append(textInfo);
    }

    return textInfos;

}

QList<TextInfo> BibleQuerier::_readInChapterData(QString bibleText, int normalisedChapter)
{
//    timer t;
//    t.start();

//    QSqlQuery query;
//    query.setForwardOnly(true);

//    if(!query.exec("select min(id), max(id) from " + bibleText + " where normalised_chapter =" + QString().setNum(normalisedChapter)))
//    {
//        qDebug() << "failed: " << query.lastError() << endl;
//        exit(1);
//    }

//    query.next();
//    int min  = query.value(0).toInt();
//    int max =  query.value(1).toInt();
    TextSpecificData* textSpecificData = _getTextSpecificData(bibleText);
    MinAndMaxIds minAndMaxIds = textSpecificData->hash.value(normalisedChapter);
    int min = minAndMaxIds.min;
    int max = minAndMaxIds.max;

//    cout << "\t min and max:" << t << endl;

    if(min > 0 && max > 0)
    {
        if(bibleText == "tisch")
            return readInTisch(min, max);
        else if (bibleText == "wlc")
            return readInWlc(min, max);
        else if (bibleText == "esv" || bibleText == "kjv")
            return readInEsvOrKjv(min, max, bibleText);
    }

    return QList<TextInfo>();
}


BibleQuerier& BibleQuerier::instance()
{
    static BibleQuerier singleton;
    return singleton;
}

QList<TextInfo> BibleQuerier::readInChapterData(QString text, int normalisedChapter)
{
//    timer t
//    t.start();
    QList<TextInfo> result = instance()._readInChapterData(text, normalisedChapter);
//    cout << text.toStdString() << " ch " << normalisedChapter << ": " << t << endl;
    return result;
}

QList<TextInfo> BibleQuerier::readInChapterDataForParallel(QString text, QSet<int> parallels, int idToInclude)
{
//    timer t;
//    t.start();
    QList<TextInfo> result = instance()._readInChapterDataForParallel(text, parallels, idToInclude);
//    cout << text.toStdString() <<": " << t << endl;
    return result;
}

VerseLocation* BibleQuerier::getVerseLocation(QString text, VerseReference verseReference)
{
    return instance()._getVerseLocation(text, verseReference);
}

TextSpecificData* BibleQuerier::getTextSpecificData(QString text)
{
    return instance()._getTextSpecificData(text);
}

QStringList BibleQuerier::search(QString searchTerms)
{
    return instance()._search(searchTerms);
}

QStringList BibleQuerier::_search(QString searchTerms)
{
    /*
        searchTerms is in the format:
        ---
        "s" (to distinguish from a normal reference)
        [(not to be implemented yet - assume filled) "<>" (an optional book to search within - otherwise will search all of them - we should only search appropriate languages though)]
        "<words>"

        each element separated by a fullstop
        words with parsing information will be "θεος@MAS" or "ἀκουω@IAP3S" (or something like that for parsing - there can be a gui to generate stuff for people unfamiliar with the syntax)

        e.g. "s.esv.the first and the last"
        (the second "the" is redundant though in our "database")
    */
    QStringList *searchResultList = new QStringList();
    QStringList searchTermList = searchTerms.split(".");
    QStringList eachWord = ((QString) searchTermList.at(2)).split(" ");

//    QString sqlQuery = "select t0.id, t0.book, t0.chapter, t0.verse, t0.text";
//    QString fromTables;
//    QString whereCondition;
//
//    for (int i = 0; i < eachWord.count(); i++)
//    {
//        fromTables.append(", " + searchTermList.at(1) + " as t" + QString().setNum(i));
//        whereCondition.append(" and t" + QString().setNum(i) + ".text like(\"%" + eachWord.at(i) + "%\") and t" + QString().setNum(i) + ".book = t0.book and t" + QString().setNum(i) + ".chapter = t0.chapter and t" + QString().setNum(i) + ".verse = t0.verse");
//    }
//    fromTables = " from" + fromTables.mid(1);
//    whereCondition = " where" + whereCondition.mid(4);
//    qDebug() << sqlQuery;
//    qDebug() << "from: " << fromTables;
//    qDebug() << "where: " << whereCondition << endl;
//
//    sqlQuery = sqlQuery.append(fromTables).append(whereCondition);
//
//    qDebug() << sqlQuery;

    QString sqlQuery;
    /*QString sqlQuery = "select book, chapter, verse, text from searchTermList.at(1)";
    QString whereCondition;
    for (int i = 0; i < eachWord.count(); i++)
    {
        whereCondition.append(" or text like(\"" + eachWord.at(i) + "\")");
    }
    whereCondition = " where" + whereCondition.mid(3);
    sqlQuery = sqlQuery.append(whereCondition).append(" group by book, chapter, verse");

    qDebug() << sqlQuery;*/

    QSqlQuery query;
    query.setForwardOnly(true);

//    if (!query.exec(sqlQuery.append(" order by t0.id asc")))
    if (((QString)searchTermList.at(2)).contains(" ", Qt::CaseSensitive))
    {
        sqlQuery = "select id, book_number, chapter, verse, count(id) as hits, text, (book_number || chapter || verse) as fish "
                   "from " + ((QString) searchTermList.at(1)) + " where";

        for (int i = 0; i < eachWord.count(); i++)
        {
            if (i != 0)
            {
                sqlQuery.append(" or");
            }
            QString thisWord = eachWord.at(i);
            QString parsing = eachWord.at(i);
            QString column = "text";
            if (thisWord.indexOf("@") != -1)
            {
                thisWord = thisWord.left(thisWord.indexOf("@"));
                parsing = parsing.right(parsing.indexOf("@") + 1);
                column = "strongs_lemma";
            }
            sqlQuery.append(" " + column + " like(\"%" + thisWord + "%\")");
        }
        sqlQuery.append(" group by fish order by hits desc");
    }
    else
    {
        QString thisWord = searchTermList.at(2);
        QString parsing = searchTermList.at(2);
        QString column = "text";
        if (thisWord.indexOf("@") != -1)
        {
            thisWord = thisWord.left(thisWord.indexOf("@"));
            parsing = parsing.right(parsing.indexOf("@") + 1);
            column = "strongs_lemma";
        }
        sqlQuery = "select id, book_number, chapter, verse, text "
                   "from " + ((QString) searchTermList.at(1)) +
                   " where " + column + " like (\"%" + thisWord + "%\")"
                   " order by id asc";
    }
    qDebug() << sqlQuery;
    if(!query.exec(sqlQuery))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

    while(query.next())
    {
        QString bookNumber = query.value(1).toString();
        QString chapter = query.value(2).toString();
        QString verse = query.value(3).toString();
        QString text = query.value(4).toString();

        searchResultList->append("<a href=\"search://" + bookNumber +"." + chapter + "." + verse + "\">" + VerseReferenceParser::calculateStringRepresentation(bookNumber.toInt(), chapter.toInt(), verse.toInt()) + "</a> \"" + text + "\"");
    }
    return *searchResultList;
}