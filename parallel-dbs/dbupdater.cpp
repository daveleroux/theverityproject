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
