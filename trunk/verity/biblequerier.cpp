#include "biblequerier.h"
#include "globalvariables.h"
#include <QtSql>
#include "timer.h"
#include "minandmaxidsforchapter.h"
#include "versereferenceparser.h"
#include <QStringList>
#include <iostream>
#include "parallelgridconstructor.h"

using namespace std;

BibleQuerier::BibleQuerier()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(DATA_PATH + "/verity.sqlite");
    if (!db.open())
    {
        qDebug() << "couldn't open db" << endl;
        exit(1);
    }
}

VerseLocation* BibleQuerier::_getVerseLocation(int bibletextId, VerseReference verseReference)
{
    QSqlQuery query;

    QString queryString;
    QTextStream(&queryString) << "select normalised_chapter, id from bibles where book_number=" << verseReference.book
            << " and chapter=" << verseReference.chapter
            << " and verse=" << verseReference.verse << " and bibletext_id="+QString().setNum(bibletextId);

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

TextSpecificData* BibleQuerier::__getTextSpecificData(int bibletextId)
{
    int minChapter;
    int maxChapter;

    QSqlQuery query;
    if(!query.exec("select min(normalised_chapter), max(normalised_chapter) from bibles where bibletext_id=" + QString().setNum(bibletextId)))
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

    if(!query.exec("select min_id, max_id, normalised_chapter from performance where bibletext_id=" + QString().setNum(bibletextId)))
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

    if(!query.exec("select rtl from bible_metadata where bibletext_id=" + QString().setNum(bibletextId)))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

    bool rtl = false;

    if(query.next())
    {
        rtl = query.value(0).toBool();
    }
    else
    {
        qDebug() << "could not find rtl" << endl;
        exit(1);
    }

    return new TextSpecificData(bibletextId, minChapter, maxChapter, minAndMaxChapterHash, rtl);
}

TextSpecificData* BibleQuerier::_getTextSpecificData(int bibletextId)
{
    if(textSpecificDataMap.contains(bibletextId))
    {
        return textSpecificDataMap.value(bibletextId);
    }
    else
    {
        TextSpecificData* textSpecificData = __getTextSpecificData(bibletextId);
        textSpecificDataMap.insert(bibletextId, textSpecificData);
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

VerseNode* BibleQuerier::_readInFromMinToMax(int bibletextId, int idFrom, int idTo, QSet<int>& parallelIds, int selectedId)
{
    QSqlQuery query;
    query.setForwardOnly(true);

    if(!query.exec("select id, text, parallel from bibles where bibletext_id="+ QString().setNum(bibletextId) +" and id >= "
                   + QString().setNum(idFrom) +
                   " and id <= " + QString().setNum(idTo) +
                   " order by id asc"))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }


    VerseNode* result = new VerseNode();
    VerseNode* previousVerseNode = result;

    while(query.next())
    {
        int id = query.value(0).toInt();
        QString xml = query.value(1).toString();
        if(id == selectedId)
            xml = "<selectedId>" + xml + "</selectedId>";
        int parallel = query.value(2).toInt();

        parallelIds.insert(parallel);

        VerseNode* verseNode = new VerseNode(parallel, xml);

        previousVerseNode->insertBelowMe(verseNode);

        previousVerseNode = verseNode;

    }

    return result;
}

VerseNode* BibleQuerier::_readInChapterDataForParallelText(int bibletextId, QSet<int>& parallelIds, int idToInclude, QMap<int, int>& firstIdsMap, QMap<int, int>& lastIdsMap)
{
    QSqlQuery query;
    //    query.setForwardOnly(true);

    //    query.prepare("select min(id), max(id) from bibles where bibletext_id=" + QString().setNum(bibletextId) + " and parallel in ("+asString(parallelIds.toList())+") ");

    //    if(!query.exec())
    //    {
    //        qDebug() << "failed: " << query.lastError();
    //        exit(1);
    //    }
    //    query.next();

    //    int min  = query.value(0).toInt();
    //    int max =  query.value(1).toInt();

    query.prepare("select id from bibles where bibletext_id=" + QString().setNum(bibletextId) + " and parallel in ("+asString(parallelIds.toList())+") order by id asc");


    if(!query.exec())
    {
        qDebug() << "failed: " << query.lastError();
        exit(1);
    }

    int min = 0;
    int max = 0;

    query.first();
    if(query.isValid())
        min = query.value(0).toInt();

    query.last();
    if(query.isValid())
        max = query.value(0).toInt();




    if(min > 0 && max > 0)
    {
        if(idToInclude > 0)
        {
            if(idToInclude < min)
                min = idToInclude;
            if(idToInclude > max)
                max = idToInclude;
        }

        firstIdsMap.insert(bibletextId, min);
        lastIdsMap.insert(bibletextId, max);

        return _readInFromMinToMax(bibletextId, min, max, parallelIds);

    }
    return new VerseNode();
}

QString BibleQuerier::_constructXml(QList<int> bibletextIds, VerseNode* grid)
{
    QString wholeChapter = "<normalisedChapter>";
    wholeChapter += "<table>";

    VerseNode* headingDownNode = grid->down;

    while(headingDownNode != 0)
    {
        VerseNode* cell = headingDownNode;

        wholeChapter += "<tr>";

        int index = 0;

        while(cell != 0)
        {
            bool rtl = _getTextSpecificData(bibletextIds.at(index))->rtl;

            wholeChapter += "<td>";

            if(rtl)
                wholeChapter += "<rtl>";

            wholeChapter += cell->xml;

            if(rtl)
                wholeChapter += "</rtl>";

            wholeChapter += "</td>";

            cell = cell->right;
            index++;
        }

        wholeChapter += "</tr>";

        headingDownNode = headingDownNode->down;
    }
    wholeChapter += "</table>";
    wholeChapter += "</normalisedChapter>";

    return wholeChapter;
}

ParallelDTO BibleQuerier::_readInChapterDataForParallelTexts(QList<int> bibletextIds, QMap<int, int> idsToInclude, int normalisedChapter, int selectedId)
{
    int bibletextId = bibletextIds.at(0);

    TextSpecificData* textSpecificData = _getTextSpecificData(bibletextId);
    MinAndMaxIds minAndMaxIds = textSpecificData->hash.value(normalisedChapter);
    int idFrom = minAndMaxIds.min;
    int idTo = minAndMaxIds.max;


    if(idFrom > 0 && idTo > 0)
    {
        QSet<int> parallelIds;
        QList<VerseNode*> chainHeads;

        QMap<int, int> firstIdsMap;
        QMap<int, int> lastIdsMap;

        firstIdsMap.insert(bibletextId, idFrom); //these two lines actually not necessary but done for tidiness
        lastIdsMap.insert(bibletextId, idTo);

        QList<int> usingBibletextIds;
        usingBibletextIds.append(bibletextId);

        chainHeads.append(_readInFromMinToMax(bibletextId, idFrom, idTo, parallelIds, selectedId));

        for(int i=1; i<bibletextIds.size(); i++)
        {
            int bibletextId = bibletextIds.at(i);
            VerseNode* verseNode = _readInChapterDataForParallelText(bibletextId, parallelIds, idsToInclude.value(bibletextId), firstIdsMap, lastIdsMap);
            if(verseNode->down != 0) //that is, are there any chunks with content at all
            {
                usingBibletextIds.append(bibletextId);
                chainHeads.append(verseNode);
            }
        }

        Grid grid = ParallelGridConstructor::constructGrid(chainHeads);
        QString xml = _constructXml(usingBibletextIds, grid.verseNode);

        VerseNode* value;
        foreach(value, chainHeads)
            delete value;

        return ParallelDTO(xml, firstIdsMap, lastIdsMap, grid.evenNumberOfRows);
    }

    return ParallelDTO("", QMap<int, int>(), QMap<int,int>(), false);


    //    QList<TextInfo> result;

    //    QSqlQuery query;
    //    query.setForwardOnly(true);

    //    query.prepare("select min(id), max(id) from " + bibleText + " where parallel in ("+asString(parallelSet.toList())+") ");

    //    if(!query.exec())
    //    {
    //        qDebug() << "failed: " << query.lastError();
    //        exit(1);
    //    }
    //    query.next();
    //    int min  = query.value(0).toInt();
    //    int max =  query.value(1).toInt();


    //    if(min > 0 && max > 0)
    //    {
    //        if(idToInclude != -1)
    //        {
    //            if(idToInclude < min)
    //                min = idToInclude;
    //            if(idToInclude > max)
    //                max = idToInclude;
    //        }

    //        if(bibleText == "tisch")
    //            return readInTisch(min, max);
    //        else if (bibleText == "wlc")
    //            return readInWlc(min, max);
    //        else if (bibleText == "esv" || bibleText == "kjv")
    //            return readInEsvOrKjv(min, max, bibleText);

    //    }
    //    return result;
}

//QList<TextInfo> BibleQuerier::readInTisch(int idFrom, int idTo) //must clean up
//{
//    QList<TextInfo> textInfos;

//    QSqlQuery query;
//    query.setForwardOnly(true);

//    if(!query.exec("select id, book, chapter, verse, number_in_verse, paragraph, text "
//                   ", morphological_tag, normalised_morph_tag, strongs_number, strongs_lemma, friberg_lemma, parallel "
//                   "from tisch where id >= "
//                   + QString().setNum(idFrom) +
//                   " and id <= " + QString().setNum(idTo) +
//                   " order by id asc"))


//    {
//        qDebug() << "failed: " << query.lastError() << endl;
//        exit(1);
//    }

//    while(query.next())
//    {
//        int id = query.value(0).toInt();
//        QString book = query.value(1).toString();
//        int chapter = query.value(2).toInt();
//        int verse = query.value(3).toInt();
//        int numberInVerse = query.value(4).toInt();
//        bool paragraph = query.value(5).toBool();
//        QString text = query.value(6).toString();
//        QString morphologicalTag = query.value(7).toString();


//        QByteArray normalisedMorphTagBytes = QByteArray::fromBase64(query.value(8).toByteArray());
//        QDataStream stream(normalisedMorphTagBytes);
//        QBitArray normalisedMorphTag(81);
//        stream >> normalisedMorphTag;

//        int strongsNumber = query.value(9).toInt();
//        QString strongsLemma = query.value(10).toString();
//        QString fribergLemma = query.value(11).toString();
//        int parallel = query.value(12).toInt();

//        TextInfo textInfo("tisch",
//                          id,
//                          book,
//                          chapter,
//                          verse,
//                          numberInVerse,
//                          paragraph,
//                          text,
//                          morphologicalTag,
//                          normalisedMorphTag,
//                          strongsNumber,
//                          strongsLemma,
//                          fribergLemma,
//                          parallel);
//        textInfos.append(textInfo);
//    }

//    return textInfos;
//}

//QList<TextInfo> BibleQuerier::readInWlc(int idFrom, int idTo) //must clean up
//{
//    QList<TextInfo> textInfos;

//    QSqlQuery query;
//    query.setForwardOnly(true);

//    if(!query.exec("select id, book, chapter, verse, number_in_verse, paragraph, text "
//                   ", morphological_tag, normalised_morph_tag, strongs_number, parallel "
//                   "from wlc where id >= "
//                   + QString().setNum(idFrom) +
//                   " and id <= " + QString().setNum(idTo) +
//                   " order by id asc"))


//    {
//        qDebug() << "failed: " << query.lastError() << endl;
//        exit(1);
//    }

//    while(query.next())
//    {
//        int id = query.value(0).toInt();
//        QString book = query.value(1).toString();
//        int chapter = query.value(2).toInt();
//        int verse = query.value(3).toInt();
//        int numberInVerse = query.value(4).toInt();
//        bool paragraph = query.value(5).toBool();
//        QString text = query.value(6).toString();


//        int strongsNumber = query.value(9).toInt();
//        int parallel = query.value(10).toInt();

//        TextInfo textInfo("wlc", id, book, chapter, verse, numberInVerse, paragraph, text, "", QBitArray(), strongsNumber, "", "", parallel);
//        textInfos.append(textInfo);
//    }
//    return textInfos;
//}

QString BibleQuerier::readInBible(int bibletextId, int idFrom, int idTo, bool rtl, int selectedId)
{
    QSqlQuery query;
    query.setForwardOnly(true);


    if(!query.exec("select id, text from bibles where bibletext_id="+ QString().setNum(bibletextId) +" and id >= "
                   + QString().setNum(idFrom) +
                   " and id <= " + QString().setNum(idTo) +
                   " order by id asc"))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

    QString result = "<normalisedChapter>";
    if(rtl)
        result += "<rtl>";
    while(query.next())
    {
        int id = query.value(0).toInt();
        QString text = query.value(1).toString();

        if(id == selectedId)
            result += "<selectedId>";

        result += text;

        if(id == selectedId)
            result += "</selectedId>";
    }
    if(rtl)
        result += "</rtl>";
    result += "</normalisedChapter>";

    return result;
}

QString BibleQuerier::_readInChapterData(int bibletextId, int normalisedChapter, int selectedId)
{
    TextSpecificData* textSpecificData = _getTextSpecificData(bibletextId);
    MinAndMaxIds minAndMaxIds = textSpecificData->hash.value(normalisedChapter);
    int min = minAndMaxIds.min;
    int max = minAndMaxIds.max;


    if(min > 0 && max > 0)
    {
        return readInBible(bibletextId, min, max, textSpecificData->rtl, selectedId);
    }

    return "";
}

QList<int> BibleQuerier::_getWordHandlerIds(int bibletextId)
{
    QList<int> result;

    QSqlQuery query;

    if(!query.exec("select wordhandler_id from wordhandlers where bibletext_id="+ QString().setNum(bibletextId)))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

    while(query.next())
    {
        result.append(query.value(0).toInt());
    }

    return result;
}

int BibleQuerier::_getStrongsNum(int bibletextId, int wordId)
{

    QSqlQuery query;

    if(!query.exec("select strongs_number from strongs_word where bibletext_id="+ QString().setNum(bibletextId) + " and word_id=" + QString().setNum(wordId)))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

    query.next();
    return query.value(0).toInt();
}

QBitArray BibleQuerier::_getNormalisedMorphTag(int bibletextId, int wordId)
{

    QSqlQuery query;

    if(!query.exec("select normalised_morph_tag from type_t_parsing where bibletext_id="+ QString().setNum(bibletextId) + " and word_id=" + QString().setNum(wordId)))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

    query.next();


    QByteArray normalisedMorphTagBytes = QByteArray::fromBase64(query.value(0).toByteArray());
    QDataStream stream(normalisedMorphTagBytes);
    QBitArray normalisedMorphTag(81);
    stream >> normalisedMorphTag;

    return normalisedMorphTag;
}

QString BibleQuerier::_getNetNote(int id)
{
    QSqlQuery query;

    if(!query.exec("select note from net_notes where id="+ QString().setNum(id) ))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

    query.next();
    return query.value(0).toString();
}

BibleQuerier& BibleQuerier::instance()
{
    static BibleQuerier singleton;
    return singleton;
}

QString BibleQuerier::readInChapterData(int bibletextId, int normalisedChapter, int selectedId)
{
    return instance()._readInChapterData(bibletextId, normalisedChapter, selectedId);
}

ParallelDTO BibleQuerier::readInChapterDataForParallel(QList<int> bibletextIds, QMap<int, int> idsToInclude, int normalisedChapter, int selectedId)
{
    return instance()._readInChapterDataForParallelTexts(bibletextIds, idsToInclude, normalisedChapter, selectedId);
}

VerseLocation* BibleQuerier::getVerseLocation(int bibletextId, VerseReference verseReference)
{
    return instance()._getVerseLocation(bibletextId, verseReference);
}

TextSpecificData* BibleQuerier::getTextSpecificData(int bibletextId)
{
    return instance()._getTextSpecificData(bibletextId);
}

QList<int> BibleQuerier::getWordHandlerIds(int bibletextId)
{
    return instance()._getWordHandlerIds(bibletextId);
}

int BibleQuerier::getStrongsNum(int bibletextId, int wordId)
{
    return instance()._getStrongsNum(bibletextId, wordId);
}

QBitArray BibleQuerier::getNormalisedMorphTag(int bibletextId, int wordId)
{
    return instance()._getNormalisedMorphTag(bibletextId, wordId);
}


QStringList BibleQuerier::search(QString searchTerms)
{
    return instance()._search(searchTerms);
}

QString BibleQuerier::getNetNote(int id)
{
    return instance()._getNetNote(id);
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
