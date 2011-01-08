#include "dbupdater.h"
#include "globalvariables.h"
#include <QList>
#include "standardrule.h"
#include <QDateTime>
#include "gen32wlcrule.h"
#include "matchtonothingrule.h"
#include "offsetrule.h"
#include "jn1tischrule.h"
#include "mt12tischrule.h"

DbUpdater::DbUpdater()
{
}

QSqlQuery DbUpdater::query(QString queryString, bool mustCheck)
{
    queryCount++;
    if(queryCount == 1000)
    {
        db.commit();
        db.transaction();
        queryCount = 0;
    }
    QSqlQuery query;
    bool passed = query.exec(queryString);
    if(mustCheck && !passed)
    {
        qDebug() << "failed: " << query.lastError();
        exit(0);
    }

    return query;
}

QSqlQuery DbUpdater::queryAndCheck(QString queryString)
{
    return query(queryString, true);
}

void DbUpdater::updateText(QString text, QList<Rule*> rules)
{
    query("alter table " + text + " add column parallel int");

    QSqlQuery query = queryAndCheck("select book_number, chapter, verse, parallel from " + text + " group by book_number, chapter, verse");

    qDebug() << "got refs from " << text << " " << QDateTime::currentDateTime().time().toString();

    QMap<VerseReference, int>* syncNumberMap = new QMap<VerseReference, int>();
    QHash<VerseReference, int>* originalSyncMap = new QHash<VerseReference, int>();

    syncNumberMaps.insert(text, syncNumberMap);
    originalSyncMaps.insert(text, originalSyncMap);

    while(query.next())
    {
        int book = query.value(0).toInt();
        int chapter = query.value(1).toInt();
        int verse = query.value(2).toInt();


        VerseReference verseReference(book, chapter, verse);

        syncNumberMap->insert(verseReference, 0);

        int originalSync = query.value(3).toInt();
        originalSyncMap->insert(verseReference, originalSync);
    }

    qDebug() << "updating " << text << QDateTime::currentDateTime().time().toString();

    QMapIterator<VerseReference, int> it(*syncNumberMap);
    while (it.hasNext())
    {
        it.next();

        VerseReference verseReference = it.key();

        int syncNumber = 0;

        for(int j=0; j< rules.size(); j++)
        {
            Rule* rule = rules.at(j);
            if(rule->applies(verseReference))
            {
                syncNumber = rule->getSyncNumber(verseReference);
                syncNumberMap->insert(verseReference, syncNumber);
                break;
            }
        }

        if(MUST_PERSIST)
        {
            if(mustUpdate(text, verseReference, syncNumber))
            {
                queryAndCheck("update " + text + " set parallel=" + QString::number(syncNumber) +
                              " where book_number=" + QString::number(verseReference.book) +
                              " and chapter="+ QString::number(verseReference.chapter) +
                              " and verse=" + QString::number(verseReference.verse));
            }
        }
    }

    qDebug() << "updated " << text << " " << QDateTime::currentDateTime().time().toString();



}

void DbUpdater::updateWlc()
{
    QList<Rule*> rules;

    Gen32WlcRule gen32WlcRule(this);
    rules.append(&gen32WlcRule);

    rules.append(&OffsetRule(this, WLC, VerseReference(EXODUS, 7, 26), VerseReference(EXODUS, 8, 28), ESV, VerseReference(EXODUS, 8, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(EXODUS, 21, 37), VerseReference(EXODUS, 22, 30), ESV, VerseReference(EXODUS, 22, 1)));

    rules.append(&OffsetRule(this, WLC, VerseReference(LEV, 5, 20), VerseReference(LEV, 6, 23), ESV, VerseReference(LEV, 6, 1)));

    rules.append(&OffsetRule(this, WLC, VerseReference(NUM, 17, 1), VerseReference(NUM, 17, 28), ESV, VerseReference(NUM, 16, 36)));
    rules.append(&OffsetRule(this, WLC, VerseReference(NUM, 30, 1), VerseReference(NUM, 30, 17), ESV, VerseReference(NUM, 29, 40)));

    rules.append(&OffsetRule(this, WLC, VerseReference(DEUT, 13, 1), VerseReference(DEUT, 13, 19), ESV, VerseReference(DEUT, 12, 32)));
    rules.append(&OffsetRule(this, WLC, VerseReference(DEUT, 23, 1), VerseReference(DEUT, 23, 26), ESV, VerseReference(DEUT, 22, 30)));
    rules.append(&OffsetRule(this, WLC, VerseReference(DEUT, 28, 69), VerseReference(DEUT, 29, 28), ESV, VerseReference(DEUT, 29, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(ONE_SAM, 21, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(ONE_SAM, 21, 2), VerseReference(ONE_SAM, 21, 16), ESV, VerseReference(ONE_SAM, 21, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(ONE_SAM, 24, 1), VerseReference(ONE_SAM, 24, 23), ESV, VerseReference(ONE_SAM, 23, 29)));

    rules.append(&OffsetRule(this, WLC, VerseReference(TWO_SAM, 19, 1), VerseReference(TWO_SAM, 19, 44), ESV, VerseReference(TWO_SAM, 18, 33)));

    rules.append(&OffsetRule(this, WLC, VerseReference(ONE_KINGS, 5, 1), VerseReference(ONE_KINGS, 5, 32), ESV, VerseReference(ONE_KINGS, 4, 21)));
    rules.append(&MatchToNothingRule(this, VerseReference(ONE_KINGS, 22, 44)));
    rules.append(&OffsetRule(this, WLC, VerseReference(ONE_KINGS, 22, 45), VerseReference(ONE_KINGS, 22, 54), ESV, VerseReference(ONE_KINGS, 22, 44)));

    rules.append(&OffsetRule(this, WLC, VerseReference(TWO_KINGS, 12, 1), VerseReference(TWO_KINGS, 12, 22), ESV, VerseReference(TWO_KINGS, 11, 21)));

    rules.append(&OffsetRule(this, WLC, VerseReference(ONE_CHRON, 5, 27), VerseReference(ONE_CHRON, 6, 66), ESV, VerseReference(ONE_CHRON, 6, 1)));
    rules.append(&MatchToNothingRule(this, VerseReference(ONE_CHRON, 12, 5)));
    rules.append(&OffsetRule(this, WLC, VerseReference(ONE_CHRON, 12, 6), VerseReference(ONE_CHRON, 12, 41), ESV, VerseReference(ONE_CHRON, 12, 5)));

    rules.append(&OffsetRule(this, WLC, VerseReference(TWO_CHRON, 1, 18), VerseReference(TWO_CHRON, 2, 17), ESV, VerseReference(TWO_CHRON, 2, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(TWO_CHRON, 13, 23), VerseReference(TWO_CHRON, 14, 14), ESV, VerseReference(TWO_CHRON, 14, 1)));

    rules.append(&OffsetRule(this, WLC, VerseReference(NEH, 3, 33), VerseReference(NEH, 4, 17), ESV, VerseReference(NEH, 4, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(NEH, 10, 1), VerseReference(NEH, 10, 40), ESV, VerseReference(NEH, 9, 38)));

    rules.append(&OffsetRule(this, WLC, VerseReference(JOB, 40, 25), VerseReference(JOB, 41, 26), ESV, VerseReference(JOB, 41, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 3, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 3, 2), VerseReference(PSALMS, 3, 9), ESV, VerseReference(PSALMS, 3, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 51, 1)));
    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 51, 2)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 51, 3), VerseReference(PSALMS, 51, 21), ESV, VerseReference(PSALMS, 51, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 52, 1)));
    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 52, 2)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 52, 3), VerseReference(PSALMS, 52, 11), ESV, VerseReference(PSALMS, 52, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 54, 1)));
    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 54, 2)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 54, 3), VerseReference(PSALMS, 54, 9), ESV, VerseReference(PSALMS, 54, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 60, 1)));
    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 60, 2)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 60, 3), VerseReference(PSALMS, 60, 14), ESV, VerseReference(PSALMS, 60, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 4, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 4, 2), VerseReference(PSALMS, 4, 9 ), ESV, VerseReference(PSALMS, 4, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 5, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 5, 2), VerseReference(PSALMS, 5, 13 ), ESV, VerseReference(PSALMS, 5, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 6, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 6, 2), VerseReference(PSALMS, 6, 11 ), ESV, VerseReference(PSALMS, 6, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 7, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 7, 2), VerseReference(PSALMS, 7, 18 ), ESV, VerseReference(PSALMS, 7, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 8, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 8, 2), VerseReference(PSALMS, 8, 10 ), ESV, VerseReference(PSALMS, 8, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 9, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 9, 2), VerseReference(PSALMS, 9, 21 ), ESV, VerseReference(PSALMS, 9, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 12, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 12, 2), VerseReference(PSALMS, 12, 9 ), ESV, VerseReference(PSALMS, 12, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 18, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 18, 2), VerseReference(PSALMS, 18, 51 ), ESV, VerseReference(PSALMS, 18, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 19, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 19, 2), VerseReference(PSALMS, 19, 15 ), ESV, VerseReference(PSALMS, 19, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 20, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 20, 2), VerseReference(PSALMS, 20, 10 ), ESV, VerseReference(PSALMS, 20, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 21, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 21, 2), VerseReference(PSALMS, 21, 14 ), ESV, VerseReference(PSALMS, 21, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 22, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 22, 2), VerseReference(PSALMS, 22, 32 ), ESV, VerseReference(PSALMS, 22, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 30, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 30, 2), VerseReference(PSALMS, 30, 13 ), ESV, VerseReference(PSALMS, 30, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 31, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 31, 2), VerseReference(PSALMS, 31, 25 ), ESV, VerseReference(PSALMS, 31, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 34, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 34, 2), VerseReference(PSALMS, 34, 23 ), ESV, VerseReference(PSALMS, 34, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 36, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 36, 2), VerseReference(PSALMS, 36, 13 ), ESV, VerseReference(PSALMS, 36, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 38, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 38, 2), VerseReference(PSALMS, 38, 23 ), ESV, VerseReference(PSALMS, 38, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 39, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 39, 2), VerseReference(PSALMS, 39, 14 ), ESV, VerseReference(PSALMS, 39, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 40, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 40, 2), VerseReference(PSALMS, 40, 18 ), ESV, VerseReference(PSALMS, 40, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 41, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 41, 2), VerseReference(PSALMS, 41, 14 ), ESV, VerseReference(PSALMS, 41, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 42, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 42, 2), VerseReference(PSALMS, 42, 12 ), ESV, VerseReference(PSALMS, 42, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 44, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 44, 2), VerseReference(PSALMS, 44, 27 ), ESV, VerseReference(PSALMS, 44, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 45, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 45, 2), VerseReference(PSALMS, 45, 18 ), ESV, VerseReference(PSALMS, 45, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 46, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 46, 2), VerseReference(PSALMS, 46, 12 ), ESV, VerseReference(PSALMS, 46, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 47, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 47, 2), VerseReference(PSALMS, 47, 10 ), ESV, VerseReference(PSALMS, 47, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 48, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 48, 2), VerseReference(PSALMS, 48, 15 ), ESV, VerseReference(PSALMS, 48, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 49, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 49, 2), VerseReference(PSALMS, 49, 21 ), ESV, VerseReference(PSALMS, 49, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 53, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 53, 2), VerseReference(PSALMS, 53, 7 ), ESV, VerseReference(PSALMS, 53, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 55, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 55, 2), VerseReference(PSALMS, 55, 24 ), ESV, VerseReference(PSALMS, 55, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 56, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 56, 2), VerseReference(PSALMS, 56, 14 ), ESV, VerseReference(PSALMS, 56, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 57, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 57, 2), VerseReference(PSALMS, 57, 12 ), ESV, VerseReference(PSALMS, 57, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 58, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 58, 2), VerseReference(PSALMS, 58, 12 ), ESV, VerseReference(PSALMS, 58, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 59, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 59, 2), VerseReference(PSALMS, 59, 18 ), ESV, VerseReference(PSALMS, 59, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 61, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 61, 2), VerseReference(PSALMS, 61, 9 ), ESV, VerseReference(PSALMS, 61, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 62, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 62, 2), VerseReference(PSALMS, 62, 13 ), ESV, VerseReference(PSALMS, 62, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 63, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 63, 2), VerseReference(PSALMS, 63, 12 ), ESV, VerseReference(PSALMS, 63, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 64, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 64, 2), VerseReference(PSALMS, 64, 11 ), ESV, VerseReference(PSALMS, 64, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 65, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 65, 2), VerseReference(PSALMS, 65, 14 ), ESV, VerseReference(PSALMS, 65, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 67, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 67, 2), VerseReference(PSALMS, 67, 8 ), ESV, VerseReference(PSALMS, 67, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 68, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 68, 2), VerseReference(PSALMS, 68, 36 ), ESV, VerseReference(PSALMS, 68, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 69, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 69, 2), VerseReference(PSALMS, 69, 37 ), ESV, VerseReference(PSALMS, 69, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 70, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 70, 2), VerseReference(PSALMS, 70, 6 ), ESV, VerseReference(PSALMS, 70, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 75, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 75, 2), VerseReference(PSALMS, 75, 11 ), ESV, VerseReference(PSALMS, 75, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 76, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 76, 2), VerseReference(PSALMS, 76, 13 ), ESV, VerseReference(PSALMS, 76, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 77, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 77, 2), VerseReference(PSALMS, 77, 21 ), ESV, VerseReference(PSALMS, 77, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 80, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 80, 2), VerseReference(PSALMS, 80, 20 ), ESV, VerseReference(PSALMS, 80, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 81, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 81, 2), VerseReference(PSALMS, 81, 17 ), ESV, VerseReference(PSALMS, 81, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 83, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 83, 2), VerseReference(PSALMS, 83, 19 ), ESV, VerseReference(PSALMS, 83, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 84, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 84, 2), VerseReference(PSALMS, 84, 13 ), ESV, VerseReference(PSALMS, 84, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 85, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 85, 2), VerseReference(PSALMS, 85, 14 ), ESV, VerseReference(PSALMS, 85, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 88, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 88, 2), VerseReference(PSALMS, 88, 19 ), ESV, VerseReference(PSALMS, 88, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 89, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 89, 2), VerseReference(PSALMS, 89, 53 ), ESV, VerseReference(PSALMS, 89, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 92, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 92, 2), VerseReference(PSALMS, 92, 16 ), ESV, VerseReference(PSALMS, 92, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 102, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 102, 2), VerseReference(PSALMS, 102, 29 ), ESV, VerseReference(PSALMS, 102, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 108, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 108, 2), VerseReference(PSALMS, 108, 14 ), ESV, VerseReference(PSALMS, 108, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 140, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 140, 2), VerseReference(PSALMS, 140, 14 ), ESV, VerseReference(PSALMS, 140, 1)));

    rules.append(&MatchToNothingRule(this, VerseReference(PSALMS, 142, 1)));
    rules.append(&OffsetRule(this, WLC, VerseReference(PSALMS, 142, 2), VerseReference(PSALMS, 142, 8 ), ESV, VerseReference(PSALMS, 142, 1)));

    rules.append(&OffsetRule(this, WLC, VerseReference(ECCL, 4, 17), VerseReference(ECCL, 5, 19 ), ESV, VerseReference(ECCL, 5, 1)));

    rules.append(&OffsetRule(this, WLC, VerseReference(SONG, 7, 1), VerseReference(SONG, 7, 14 ), ESV, VerseReference(SONG, 6, 13)));

    rules.append(&OffsetRule(this, WLC, VerseReference(IS, 8, 23), VerseReference(IS, 9, 20 ), ESV, VerseReference(IS, 9, 1)));

    rules.append(&OffsetRule(this, WLC, VerseReference(JER, 8, 23), VerseReference(JER, 9, 25 ), ESV, VerseReference(JER, 9, 1)));

    rules.append(&OffsetRule(this, WLC, VerseReference(EZ, 21, 1), VerseReference(EZ, 21, 37 ), ESV, VerseReference(EZ, 20, 45)));

    rules.append(&OffsetRule(this, WLC, VerseReference(DAN, 3, 31), VerseReference(DAN, 4, 34 ), ESV, VerseReference(DAN, 4, 1)));

    rules.append(&OffsetRule(this, WLC, VerseReference(DAN, 6, 1), VerseReference(DAN, 6, 29 ), ESV, VerseReference(DAN, 5, 31)));

    rules.append(&OffsetRule(this, WLC, VerseReference(HOS, 2, 1), VerseReference(HOS, 2, 25 ), ESV, VerseReference(HOS, 1, 10)));

    rules.append(&OffsetRule(this, WLC, VerseReference(HOS, 12, 1), VerseReference(HOS, 12, 15 ), ESV, VerseReference(HOS, 11, 12)));

    rules.append(&OffsetRule(this, WLC, VerseReference(HOS, 14, 1), VerseReference(HOS, 14, 10 ), ESV, VerseReference(HOS, 13, 16)));

    rules.append(&OffsetRule(this, WLC, VerseReference(JOEL, 3, 1), VerseReference(JOEL, 3, 5 ), ESV, VerseReference(JOEL, 2, 28)));

    rules.append(&OffsetRule(this, WLC, VerseReference(JOEL, 4, 1), VerseReference(JOEL, 4, 21 ), ESV, VerseReference(JOEL, 3, 1)));

    rules.append(&OffsetRule(this, WLC, VerseReference(JONAH, 2, 1), VerseReference(JONAH, 2, 11 ), ESV, VerseReference(JONAH, 1, 17)));

    rules.append(&OffsetRule(this, WLC, VerseReference(MICAH, 4, 14), VerseReference(MICAH, 5, 14 ), ESV, VerseReference(MICAH, 5, 1)));

    rules.append(&OffsetRule(this, WLC, VerseReference(NAHUM, 2, 1), VerseReference(NAHUM, 2, 14 ), ESV, VerseReference(NAHUM, 1, 15)));

    rules.append(&OffsetRule(this, WLC, VerseReference(ZECH, 2, 1), VerseReference(ZECH, 2, 17 ), ESV, VerseReference(ZECH, 1, 18)));

    rules.append(&OffsetRule(this, WLC, VerseReference(MAL, 3, 19), VerseReference(MAL, 3, 24 ), ESV, VerseReference(MAL, 4, 1)));

    StandardRule standardRule(this);
    rules.append(&standardRule);

    updateText(WLC, rules);
}

void DbUpdater::updateKjv()
{
    QList<Rule*> rules;

    StandardRule standardRule(this);
    rules.append(&standardRule);

    updateText(KJV, rules);
}

void DbUpdater::updateTisch()
{
    QList<Rule*> rules;

    Jn1TischRule jn1TischRule(this);
    rules.append(&jn1TischRule);

    Mt12TischRule mt12TischRule(this);
    rules.append(&mt12TischRule);

    StandardRule standardRule(this);
    rules.append(&standardRule);

    updateText(TISCH, rules);
}

bool DbUpdater::mustUpdate(QString text, VerseReference verseReference, int newSync)
{
    QHash<VerseReference, int>* originalHash = originalSyncMaps.value(text);
    int oldSync = originalHash->value(verseReference);
    return oldSync != newSync;
}

void DbUpdater::updateEsv()
{
    QMapIterator<VerseReference, int> it(*syncNumberMaps.value(ESV));
    while (it.hasNext())
    {
        it.next();

        VerseReference verseReference = it.key();
        int syncNumber = it.value();

        if(MUST_PERSIST)
        {
            if(mustUpdate(ESV, verseReference, syncNumber))
            {
                queryAndCheck("update esv set parallel=" + QString::number(syncNumber) +
                              " where book_number=" + QString::number(verseReference.book) +
                              " and chapter="+ QString::number(verseReference.chapter) +
                              " and verse=" + QString::number(verseReference.verse));
            }
        }
    }
}

void DbUpdater::update()
{
    MUST_PERSIST = true;

    queryCount = 0;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("bibles.sqlite");
    if (!db.open())
    {
        qDebug() << "couldn't open db" << endl;
        exit(1);
    }

    db.transaction();

    query("alter table esv add column parallel int");

    QSqlQuery query = queryAndCheck("select book_number, chapter, verse, parallel from esv group by book_number, chapter, verse");

    qDebug() << "got refs from esv" << QDateTime::currentDateTime().time().toString();

    QMap<VerseReference, int>* esvSyncNumberMap = new QMap<VerseReference, int>();
    QHash<VerseReference, int>* originalEsvSyncMap = new QHash<VerseReference, int>();

    highestUnusedSyncNumber = 1;

    while(query.next())
    {
        int book = query.value(0).toInt();
        int chapter = query.value(1).toInt();
        int verse = query.value(2).toInt();

        VerseReference verseReference(book, chapter, verse);

        esvSyncNumberMap->insert(verseReference, highestUnusedSyncNumber++);

        int originalSync = query.value(3).toInt();
        originalEsvSyncMap->insert(verseReference, originalSync);
    }

    syncNumberMaps.insert(ESV, esvSyncNumberMap);
    originalSyncMaps.insert(ESV, originalEsvSyncMap);

    qDebug() << "starting to update esv" << QDateTime::currentDateTime().time().toString();

    updateEsv();

    qDebug() << "updated esv" << QDateTime::currentDateTime().time().toString();


    updateKjv();
    updateWlc();
    updateTisch();

    qDebug() << "finished: " << QDateTime::currentDateTime().time().toString();

    db.commit();
    db.close();

}
