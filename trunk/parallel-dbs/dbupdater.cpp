#include "dbupdater.h"
#include "globalvariables.h"
#include <QList>
#include "standardrule.h"
#include <QDateTime>
#include "matchtonothingrule.h"
#include "offsetrule.h"
#include "headingsrule.h"
#include "superscriptionwlcrule.h"

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
    //    qDebug() << queryString;
    return query(queryString, true);
}

void DbUpdater::updateText(QString text, int bibletextId, QList<Rule*> rules)
{
    QSqlQuery query = queryAndCheck("select id, bibletext_id, book_number, chapter, verse, text, parallel from bibles where bibletext_id=" + QString().setNum(bibletextId) + " order by id asc");

    qDebug() << "got refs from "<< QString().setNum(bibletextId) << " " << QDateTime::currentDateTime().time().toString();

    SyncNumberLookup* syncNumberLookup = new SyncNumberLookup();

//    QMap<VerseReference, int>* syncNumberMap = new QMap<VerseReference, int>();
//    QHash<VerseReference, int>* originalSyncMap = new QHash<VerseReference, int>();

    syncNumberMaps.insert(text, syncNumberLookup);
//    originalSyncMaps.insert(text, originalSyncMap);

    int bookNumber = 0;

    while(query.next())
    {

        int id = query.value(0).toInt();
        int book = query.value(2).toInt();
        int chapter = query.value(3).toInt();
        int verse = query.value(4).toInt();

        if(book != bookNumber)
        {
            bookNumber = book;
            syncNumberLookup->insertBookNumber(bookNumber, id);
        }
//        QString text = query.value(5).toString();
//        if(!text.contains("<bookName>"))
//            text = "";

        if(verse != 0)
        {
            VerseReference verseReference(book, chapter, verse, "");
            syncNumberLookup->insert(verseReference, id);
        }


        syncNumberLookup->insertParallel(id, 0);

        int originalSync = query.value(6).toInt();
        syncNumberLookup->insertOriginal(id, originalSync);
    }

    qDebug() << "updating "<< QString().setNum(bibletextId) << QDateTime::currentDateTime().time().toString();

    QList<int> ids = syncNumberLookup->getAllIds();

    for(int i=0; i<ids.size(); i++)
    {
        int id = ids.at(i);

        int syncNumber = 0;

        for(int j=0; j< rules.size(); j++)
        {
            Rule* rule = rules.at(j);
            if(rule->applies(id))
            {
                syncNumber = rule->getSyncNumber(id);
                syncNumberLookup->insertParallel(id, syncNumber);

                if(MUST_PERSIST)
                {
                    if(syncNumberLookup->mustUpdate(id, syncNumber))
                    {
                        queryAndCheck("update bibles set parallel=" + QString::number(syncNumber) +
                                      " where id=" + QString().setNum(id));
                    }
                }

                break;
            }
        }

    }

    qDebug() << "updated " << QString().setNum(bibletextId) << " " << QDateTime::currentDateTime().time().toString();
}

void DbUpdater::updateWlc()
{
    QList<Rule*> rules;

    HeadingsRule headingsRule(this, WLC);
    rules.append(&headingsRule);

//    OffsetRule tempOffsetRuleAddress;
//    MatchToNothingRule tempMatchToNothingRuleAddress;
//    SuperscriptionWlcRule tempSuperscriptionWlcRuleAddress;


    OffsetRule tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(GENESIS, 32, 1), VerseReference(GENESIS, 32, 33), NET, VerseReference(GENESIS, 31, 55));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(EXODUS, 7, 26), VerseReference(EXODUS, 8, 28), NET, VerseReference(EXODUS, 8, 1));
    rules.append(&tempOffsetRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(EXODUS, 21, 37), VerseReference(EXODUS, 22, 30), NET, VerseReference(EXODUS, 22, 1));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(LEV, 5, 20), VerseReference(LEV, 6, 23), NET, VerseReference(LEV, 6, 1));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(NUM, 17, 1), VerseReference(NUM, 17, 28), NET, VerseReference(NUM, 16, 36));
    rules.append(&tempOffsetRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(NUM, 30, 1), VerseReference(NUM, 30, 17), NET, VerseReference(NUM, 29, 40));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(DEUT, 13, 1), VerseReference(DEUT, 13, 19), NET, VerseReference(DEUT, 12, 32));
    rules.append(&tempOffsetRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(DEUT, 23, 1), VerseReference(DEUT, 23, 26), NET, VerseReference(DEUT, 22, 30));
    rules.append(&tempOffsetRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(DEUT, 28, 69), VerseReference(DEUT, 29, 28), NET, VerseReference(DEUT, 29, 1));
    rules.append(&tempOffsetRuleAddress);

    MatchToNothingRule tempMatchToNothingRuleAddress = MatchToNothingRule(this, WLC, VerseReference(ONE_SAM, 21, 1));
    rules.append(&tempMatchToNothingRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(ONE_SAM, 21, 2), VerseReference(ONE_SAM, 21, 16), NET, VerseReference(ONE_SAM, 21, 1));
    rules.append(&tempOffsetRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(ONE_SAM, 24, 1), VerseReference(ONE_SAM, 24, 23), NET, VerseReference(ONE_SAM, 23, 29));
    rules.append(&tempOffsetRuleAddress);



    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(TWO_SAM, 19, 1), VerseReference(TWO_SAM, 19, 44), NET, VerseReference(TWO_SAM, 18, 33));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(ONE_KINGS, 5, 1), VerseReference(ONE_KINGS, 5, 32), NET, VerseReference(ONE_KINGS, 4, 21));
    rules.append(&tempOffsetRuleAddress);
    tempMatchToNothingRuleAddress = MatchToNothingRule(this, WLC, VerseReference(ONE_KINGS, 22, 44));
    rules.append(&tempMatchToNothingRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(ONE_KINGS, 22, 45), VerseReference(ONE_KINGS, 22, 54), NET, VerseReference(ONE_KINGS, 22, 44));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(TWO_KINGS, 12, 1), VerseReference(TWO_KINGS, 12, 22), NET, VerseReference(TWO_KINGS, 11, 21));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(ONE_CHRON, 5, 27), VerseReference(ONE_CHRON, 6, 66), NET, VerseReference(ONE_CHRON, 6, 1));
    rules.append(&tempOffsetRuleAddress);
    tempMatchToNothingRuleAddress = MatchToNothingRule(this, WLC, VerseReference(ONE_CHRON, 12, 5));
    rules.append(&tempMatchToNothingRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(ONE_CHRON, 12, 6), VerseReference(ONE_CHRON, 12, 41), NET, VerseReference(ONE_CHRON, 12, 5));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(TWO_CHRON, 1, 18), VerseReference(TWO_CHRON, 2, 17), NET, VerseReference(TWO_CHRON, 2, 1));
    rules.append(&tempOffsetRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(TWO_CHRON, 13, 23), VerseReference(TWO_CHRON, 14, 14), NET, VerseReference(TWO_CHRON, 14, 1));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(NEH, 3, 33), VerseReference(NEH, 4, 17), NET, VerseReference(NEH, 4, 1));
    rules.append(&tempOffsetRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(NEH, 7, 68), VerseReference(NEH, 7, 72), NET, VerseReference(NEH, 7, 69));
    rules.append(&tempOffsetRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(NEH, 10, 1), VerseReference(NEH, 10, 40), NET, VerseReference(NEH, 9, 38));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(JOB, 40, 25), VerseReference(JOB, 41, 26), NET, VerseReference(JOB, 41, 1));
    rules.append(&tempOffsetRuleAddress);

    SuperscriptionWlcRule tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 3, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 3, 2), VerseReference(PSALMS, 3, 9), NET, VerseReference(PSALMS, 3, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 51, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempMatchToNothingRuleAddress = MatchToNothingRule(this, WLC, VerseReference(PSALMS, 51, 2));
    rules.append(&tempMatchToNothingRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 51, 3), VerseReference(PSALMS, 51, 21), NET, VerseReference(PSALMS, 51, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 52, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempMatchToNothingRuleAddress = MatchToNothingRule(this, WLC, VerseReference(PSALMS, 52, 2));
    rules.append(&tempMatchToNothingRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 52, 3), VerseReference(PSALMS, 52, 11), NET, VerseReference(PSALMS, 52, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 54, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempMatchToNothingRuleAddress = MatchToNothingRule(this, WLC, VerseReference(PSALMS, 54, 2));
    rules.append(&tempMatchToNothingRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 54, 3), VerseReference(PSALMS, 54, 9), NET, VerseReference(PSALMS, 54, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 60, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempMatchToNothingRuleAddress = MatchToNothingRule(this, WLC, VerseReference(PSALMS, 60, 2));
    rules.append(&tempMatchToNothingRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 60, 3), VerseReference(PSALMS, 60, 14), NET, VerseReference(PSALMS, 60, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 4, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 4, 2), VerseReference(PSALMS, 4, 9 ), NET, VerseReference(PSALMS, 4, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 5, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 5, 2), VerseReference(PSALMS, 5, 13 ), NET, VerseReference(PSALMS, 5, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 6, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 6, 2), VerseReference(PSALMS, 6, 11 ), NET, VerseReference(PSALMS, 6, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 7, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 7, 2), VerseReference(PSALMS, 7, 18 ), NET, VerseReference(PSALMS, 7, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 8, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 8, 2), VerseReference(PSALMS, 8, 10 ), NET, VerseReference(PSALMS, 8, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 9, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 9, 2), VerseReference(PSALMS, 9, 21 ), NET, VerseReference(PSALMS, 9, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 12, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 12, 2), VerseReference(PSALMS, 12, 9 ), NET, VerseReference(PSALMS, 12, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 18, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 18, 2), VerseReference(PSALMS, 18, 51 ), NET, VerseReference(PSALMS, 18, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 19, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 19, 2), VerseReference(PSALMS, 19, 15 ), NET, VerseReference(PSALMS, 19, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 20, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 20, 2), VerseReference(PSALMS, 20, 10 ), NET, VerseReference(PSALMS, 20, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 21, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 21, 2), VerseReference(PSALMS, 21, 14 ), NET, VerseReference(PSALMS, 21, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 22, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 22, 2), VerseReference(PSALMS, 22, 32 ), NET, VerseReference(PSALMS, 22, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 30, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 30, 2), VerseReference(PSALMS, 30, 13 ), NET, VerseReference(PSALMS, 30, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 31, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 31, 2), VerseReference(PSALMS, 31, 25 ), NET, VerseReference(PSALMS, 31, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 34, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 34, 2), VerseReference(PSALMS, 34, 23 ), NET, VerseReference(PSALMS, 34, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 36, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 36, 2), VerseReference(PSALMS, 36, 13 ), NET, VerseReference(PSALMS, 36, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 38, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 38, 2), VerseReference(PSALMS, 38, 23 ), NET, VerseReference(PSALMS, 38, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 39, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 39, 2), VerseReference(PSALMS, 39, 14 ), NET, VerseReference(PSALMS, 39, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 40, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 40, 2), VerseReference(PSALMS, 40, 18 ), NET, VerseReference(PSALMS, 40, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 41, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 41, 2), VerseReference(PSALMS, 41, 14 ), NET, VerseReference(PSALMS, 41, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 42, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 42, 2), VerseReference(PSALMS, 42, 12 ), NET, VerseReference(PSALMS, 42, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 44, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 44, 2), VerseReference(PSALMS, 44, 27 ), NET, VerseReference(PSALMS, 44, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 45, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 45, 2), VerseReference(PSALMS, 45, 18 ), NET, VerseReference(PSALMS, 45, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 46, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 46, 2), VerseReference(PSALMS, 46, 12 ), NET, VerseReference(PSALMS, 46, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 47, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 47, 2), VerseReference(PSALMS, 47, 10 ), NET, VerseReference(PSALMS, 47, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 48, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 48, 2), VerseReference(PSALMS, 48, 15 ), NET, VerseReference(PSALMS, 48, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 49, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 49, 2), VerseReference(PSALMS, 49, 21 ), NET, VerseReference(PSALMS, 49, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 53, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 53, 2), VerseReference(PSALMS, 53, 7 ), NET, VerseReference(PSALMS, 53, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 55, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 55, 2), VerseReference(PSALMS, 55, 24 ), NET, VerseReference(PSALMS, 55, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 56, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 56, 2), VerseReference(PSALMS, 56, 14 ), NET, VerseReference(PSALMS, 56, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 57, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 57, 2), VerseReference(PSALMS, 57, 12 ), NET, VerseReference(PSALMS, 57, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 58, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 58, 2), VerseReference(PSALMS, 58, 12 ), NET, VerseReference(PSALMS, 58, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 59, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 59, 2), VerseReference(PSALMS, 59, 18 ), NET, VerseReference(PSALMS, 59, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 61, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 61, 2), VerseReference(PSALMS, 61, 9 ), NET, VerseReference(PSALMS, 61, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 62, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 62, 2), VerseReference(PSALMS, 62, 13 ), NET, VerseReference(PSALMS, 62, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 63, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 63, 2), VerseReference(PSALMS, 63, 12 ), NET, VerseReference(PSALMS, 63, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 64, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 64, 2), VerseReference(PSALMS, 64, 11 ), NET, VerseReference(PSALMS, 64, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 65, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 65, 2), VerseReference(PSALMS, 65, 14 ), NET, VerseReference(PSALMS, 65, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 67, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 67, 2), VerseReference(PSALMS, 67, 8 ), NET, VerseReference(PSALMS, 67, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 68, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 68, 2), VerseReference(PSALMS, 68, 36 ), NET, VerseReference(PSALMS, 68, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 69, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 69, 2), VerseReference(PSALMS, 69, 37 ), NET, VerseReference(PSALMS, 69, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 70, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 70, 2), VerseReference(PSALMS, 70, 6 ), NET, VerseReference(PSALMS, 70, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 75, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 75, 2), VerseReference(PSALMS, 75, 11 ), NET, VerseReference(PSALMS, 75, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 76, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 76, 2), VerseReference(PSALMS, 76, 13 ), NET, VerseReference(PSALMS, 76, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 77, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 77, 2), VerseReference(PSALMS, 77, 21 ), NET, VerseReference(PSALMS, 77, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 80, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 80, 2), VerseReference(PSALMS, 80, 20 ), NET, VerseReference(PSALMS, 80, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 81, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 81, 2), VerseReference(PSALMS, 81, 17 ), NET, VerseReference(PSALMS, 81, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 83, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 83, 2), VerseReference(PSALMS, 83, 19 ), NET, VerseReference(PSALMS, 83, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 84, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 84, 2), VerseReference(PSALMS, 84, 13 ), NET, VerseReference(PSALMS, 84, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 85, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 85, 2), VerseReference(PSALMS, 85, 14 ), NET, VerseReference(PSALMS, 85, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 88, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 88, 2), VerseReference(PSALMS, 88, 19 ), NET, VerseReference(PSALMS, 88, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 89, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 89, 2), VerseReference(PSALMS, 89, 53 ), NET, VerseReference(PSALMS, 89, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 92, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 92, 2), VerseReference(PSALMS, 92, 16 ), NET, VerseReference(PSALMS, 92, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 102, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 102, 2), VerseReference(PSALMS, 102, 29 ), NET, VerseReference(PSALMS, 102, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 108, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 108, 2), VerseReference(PSALMS, 108, 14 ), NET, VerseReference(PSALMS, 108, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 140, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 140, 2), VerseReference(PSALMS, 140, 14 ), NET, VerseReference(PSALMS, 140, 1));
    rules.append(&tempOffsetRuleAddress);

    tempSuperscriptionWlcRuleAddress = SuperscriptionWlcRule(this, VerseReference(PSALMS, 142, 1));
    rules.append(&tempSuperscriptionWlcRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(PSALMS, 142, 2), VerseReference(PSALMS, 142, 8 ), NET, VerseReference(PSALMS, 142, 1));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(ECCL, 4, 17), VerseReference(ECCL, 5, 19 ), NET, VerseReference(ECCL, 5, 1));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(SONG, 7, 1), VerseReference(SONG, 7, 14 ), NET, VerseReference(SONG, 6, 13));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(IS, 8, 23), VerseReference(IS, 9, 20 ), NET, VerseReference(IS, 9, 1));
    rules.append(&tempOffsetRuleAddress);
    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(IS, 64, 1), VerseReference(IS, 64, 11 ), NET, VerseReference(IS, 64, 2));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(JER, 8, 23), VerseReference(JER, 9, 25 ), NET, VerseReference(JER, 9, 1));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(EZ, 21, 1), VerseReference(EZ, 21, 37 ), NET, VerseReference(EZ, 20, 45));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(DAN, 3, 31), VerseReference(DAN, 4, 34 ), NET, VerseReference(DAN, 4, 1));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(DAN, 6, 1), VerseReference(DAN, 6, 29 ), NET, VerseReference(DAN, 5, 31));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(HOS, 2, 1), VerseReference(HOS, 2, 25 ), NET, VerseReference(HOS, 1, 10));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(HOS, 12, 1), VerseReference(HOS, 12, 15 ), NET, VerseReference(HOS, 11, 12));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(HOS, 14, 1), VerseReference(HOS, 14, 10 ), NET, VerseReference(HOS, 13, 16));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(JOEL, 3, 1), VerseReference(JOEL, 3, 5 ), NET, VerseReference(JOEL, 2, 28));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(JOEL, 4, 1), VerseReference(JOEL, 4, 21 ), NET, VerseReference(JOEL, 3, 1));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(JONAH, 2, 1), VerseReference(JONAH, 2, 11 ), NET, VerseReference(JONAH, 1, 17));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(MICAH, 4, 14), VerseReference(MICAH, 5, 14 ), NET, VerseReference(MICAH, 5, 1));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(NAHUM, 2, 1), VerseReference(NAHUM, 2, 14 ), NET, VerseReference(NAHUM, 1, 15));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(ZECH, 2, 1), VerseReference(ZECH, 2, 17 ), NET, VerseReference(ZECH, 1, 18));
    rules.append(&tempOffsetRuleAddress);

    tempOffsetRuleAddress = OffsetRule(this, WLC, VerseReference(MAL, 3, 19), VerseReference(MAL, 3, 24 ), NET, VerseReference(MAL, 4, 1));
    rules.append(&tempOffsetRuleAddress);

    StandardRule standardRule(this, WLC);
    rules.append(&standardRule);

    updateText(WLC, 3, rules);
}

//void DbUpdater::updateKjv()
//{
//    QList<Rule*> rules;

//    StandardRule standardRule(this);
//    rules.append(&standardRule);

//    updateText(KJV, rules);
//}

void DbUpdater::updateTisch()
{
    QList<Rule*> rules;

    HeadingsRule headingsRule(this, TISCH);
    rules.append(&headingsRule);

    MatchToNothingRule tmtnr = MatchToNothingRule(this, TISCH, VerseReference(JOHN, 1, 39));
    rules.append(&tmtnr);
    OffsetRule tor = OffsetRule(this, TISCH, VerseReference(JOHN, 1, 40), VerseReference(JOHN, 1, 52 ), NET, VerseReference(JOHN, 1, 39));
    rules.append(&tor);

    tor = OffsetRule(this, TISCH, VerseReference(MATTHEW, 5, 4), VerseReference(MATTHEW, 5, 4), NET, VerseReference(MATTHEW, 5, 5));
    rules.append(&tor);
    tor = OffsetRule(this, TISCH, VerseReference(MATTHEW, 5, 5), VerseReference(MATTHEW, 5, 5), NET, VerseReference(MATTHEW, 5, 4));
    rules.append(&tor);

//    SecondCo13v13TischRule secondCo13v13TischRule(this);
//    rules.append(&secondCo13v13TischRule);

    StandardRule standardRule(this, TISCH);
    rules.append(&standardRule);      

    updateText(TISCH, 2, rules);
}


void DbUpdater::update()
{
    MUST_PERSIST = true;

    queryCount = 0;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("verity.sqlite");
    if (!db.open())
    {
        qDebug() << "couldn't open db" << endl;
        exit(1);
    }


    db.transaction();

    QSqlQuery query;

//CHEAT

//    query = queryAndCheck("select id, parallel from parallel_backup");

//    int count = 0;

//    while(query.next())
//    {
//      int id = query.value(0).toInt();
//      int parallel = query.value(1).toInt();

//      queryAndCheck("update bibles set parallel=" + QString::number(parallel) +
//                    " where id=" + QString().setNum(id));

//      count++;
//      if(count%5000 == 0)
//      {
//          db.commit();
//          db.transaction();
//      }
//    }

//    db.commit();
//    db.close();

//    return;

//END CHEAT


    queryAndCheck("drop index idx_bibles");

    query = queryAndCheck("select id, bibletext_id, book_number, chapter, verse, text, parallel from bibles where bibletext_id=1 order by id asc");

    qDebug() << "got refs from net" << QDateTime::currentDateTime().time().toString();

//    QMap<VerseReference, int>* netSyncNumberMap = new QMap<VerseReference, int>();
//    QHash<VerseReference, int>* originalNetSyncMap = new QHash<VerseReference, int>();

    SyncNumberLookup* netSyncNumberLookup = new SyncNumberLookup();
    //    highestUnusedSyncNumber = 1;


    int bookNumber = 0;

    while(query.next())
    {
        int id = query.value(0).toInt();
        int book = query.value(2).toInt();
        int chapter = query.value(3).toInt();
        int verse = query.value(4).toInt();

        QString text = query.value(5).toString();

        int parallel = query.value(6).toInt();

        if(bookNumber != book)
        {
            bookNumber = book;
            netSyncNumberLookup->insertBookNumber(bookNumber, id);
        }

//        if(chapter==0 && verse==0)
//        {
//            QString standardName = GlobalsHelper::standardNameForBookNumber(book);
//            if(text.contains("<bookName>" + standardName + "</bookName>"))
//            {
//                GlobalsHelper::insertNetBookNumberAndParallel(book, parallel);
//            }
//        }

        if(verse != 0)
        {
            VerseReference verseReference(book, chapter, verse, text);
            netSyncNumberLookup->insert(verseReference, id);
        }


        netSyncNumberLookup->insertParallel(id, parallel);

        netSyncNumberLookup->insertOriginal(id, parallel);



        highestUnusedSyncNumber = parallel;
    }

    highestUnusedSyncNumber++;

//    syncNumberMaps.insert(NET, netSyncNumberMap);
//    originalSyncMaps.insert(NET, originalNetSyncMap);

    syncNumberMaps.insert(NET, netSyncNumberLookup);


    updateTisch();
    updateWlc();


    qDebug() << "finished: " << QDateTime::currentDateTime().time().toString();

    query.exec("create index idx_bibles on bibles (id, bibletext_id, book_number, normalised_chapter, chapter, verse, parallel)");

    db.commit();
    db.close();

}
