#include "dbupdater.h"
#include "globalvariables.h"
#include <QList>
#include "standardrule.h"
#include <QDateTime>
#include "gen32wlcrule.h"
#include "jn1tischrule.h"

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

    QSqlQuery query = queryAndCheck("select book_number, chapter, verse from " + text + " group by book_number, chapter, verse");

    qDebug() << "got refs from " << text << " " << QDateTime::currentDateTime().time().toString();

    QMap<VerseReference, int> syncNumberMap;

    while(query.next())
    {
        int book = query.value(0).toInt();
        int chapter = query.value(1).toInt();
        int verse = query.value(2).toInt();


        VerseReference verseReference(book, chapter, verse);

        for(int i=0; i< rules.size(); i++)
        {
            Rule* rule = rules.at(i);
            if(rule->applies(verseReference))
            {
                syncNumberMap.insert(verseReference, rule->getSyncNumber(verseReference));
                break;
            }
        }

    }

    qDebug() << "applied rules to " << text << ", now updating " << QDateTime::currentDateTime().time().toString();

    for(int i=0; i<syncNumberMap.keys().size(); i++)
    {
        VerseReference verseReference = syncNumberMap.keys().at(i);
        int syncNumber = syncNumberMap.value(verseReference);

        queryAndCheck("update " + text + " set parallel=" + QString::number(syncNumber) +
                      " where book_number=" + QString::number(verseReference.book) +
                      " and chapter="+ QString::number(verseReference.chapter) +
                      " and verse=" + QString::number(verseReference.verse));
    }

    qDebug() << "updated " << text << " " << QDateTime::currentDateTime().time().toString();

    syncNumberMaps.insert(text, syncNumberMap);

}

void DbUpdater::updateWlc()
{
    QList<Rule*> rules;

    Gen32WlcRule gen32WlcRule(this);
    rules.append(&gen32WlcRule);

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

    StandardRule standardRule(this);
    rules.append(&standardRule);

    updateText(TISCH, rules);
}

void DbUpdater::updateEsv()
{
    QMap<VerseReference, int> esvSyncNumberMap = syncNumberMaps.value(ESV);
    for(int i=0; i<esvSyncNumberMap.keys().size(); i++)
    {
        VerseReference verseReference = esvSyncNumberMap.keys().at(i);
        int syncNumber = esvSyncNumberMap.value(verseReference);

        queryAndCheck("update esv set parallel=" + QString::number(syncNumber) +
                      " where book_number=" + QString::number(verseReference.book) +
                      " and chapter="+ QString::number(verseReference.chapter) +
                      " and verse=" + QString::number(verseReference.verse));
    }
}

void DbUpdater::update()
{
    queryCount = 0;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("bibles.sqlite");
    if (!db.open())
    {
        qDebug() << "couldn't open db" << endl;
        exit(1);
    }

    db.transaction();

    //queryAndCheck("alter table esv drop column parallel");//AAAH can't do this in sqlite

    query("alter table esv add column parallel int");

    QSqlQuery query = queryAndCheck("select book_number, chapter, verse from esv group by book_number, chapter, verse");

    qDebug() << "got refs from esv" << QDateTime::currentDateTime().time().toString();

    QMap<VerseReference, int> esvSyncNumberMap;

    highestUnusedSyncNumber = 1;

    while(query.next())
    {
        int book = query.value(0).toInt();
        int chapter = query.value(1).toInt();
        int verse = query.value(2).toInt();


        VerseReference verseReference(book, chapter, verse);
        esvSyncNumberMap.insert(verseReference, highestUnusedSyncNumber++);
    }

    syncNumberMaps.insert(ESV, esvSyncNumberMap);

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
