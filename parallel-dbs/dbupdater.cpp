#include "dbupdater.h"
#include "globalvariables.h"
#include <QList>
#include "standardrule.h"
#include <QDateTime>
#include "gen32wlcrule.h"

DbUpdater::DbUpdater()
{
}

QSqlQuery DbUpdater::query(QString queryString, bool mustCheck)
{
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

void DbUpdater::updateWlc()
{
    QList<Rule*> rules;

    Gen32WlcRule gen32WlcRule(this);
    rules.append(&gen32WlcRule);

    StandardRule standardRule(this);
    rules.append(&standardRule);

    query("alter table wlc add column parallel int");

    QSqlQuery query = queryAndCheck("select book_number, chapter, verse from wlc group by book_number, chapter, verse");

    qDebug() << "got refs from wlc" << QDateTime::currentDateTime().time().toString();

    QMap<VerseReference, int> wlcSyncNumberMap;

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
                wlcSyncNumberMap.insert(verseReference, rule->getSyncNumber(verseReference));
                break;
            }
        }

    }

    qDebug() << "applied rules to wlc, now updating wlc " << QDateTime::currentDateTime().time().toString();

    for(int i=0; i<wlcSyncNumberMap.keys().size(); i++)
    {
        VerseReference verseReference = wlcSyncNumberMap.keys().at(i);
        int syncNumber = wlcSyncNumberMap.value(verseReference);

        queryAndCheck("update wlc set parallel=" + QString::number(syncNumber) +
                      " where book_number=" + QString::number(verseReference.book) +
                      " and chapter="+ QString::number(verseReference.chapter) +
                      " and verse=" + QString::number(verseReference.verse));
    }

    qDebug() << "updated wlc" << QDateTime::currentDateTime().time().toString();

    syncNumberMaps.insert(WLC, wlcSyncNumberMap);

}

void DbUpdater::update()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("bibles.sqlite");
    if (!db.open())
    {
        qDebug() << "couldn't open db" << endl;
        exit(1);
    }

    //queryAndCheck("alter table esv drop column parallel");//AAAH can't do this in sqlite

    query("alter table esv add column parallel int");

    QSqlQuery query = queryAndCheck("select book_number, chapter, verse from esv group by book_number, chapter, verse");

    qDebug() << "got refs from esv" << QDateTime::currentDateTime().time().toString();

    QMap<VerseReference, int> esvSyncNumberMap;

    highestUnusedSyncNumber = 0;

    while(query.next())
    {
        int book = query.value(0).toInt();
        int chapter = query.value(1).toInt();
        int verse = query.value(2).toInt();


        VerseReference verseReference(book, chapter, verse);
        esvSyncNumberMap.insert(verseReference, highestUnusedSyncNumber++);
    }

    qDebug() << "starting to update esv" << QDateTime::currentDateTime().time().toString();

    for(int i=0; i<esvSyncNumberMap.keys().size(); i++)
    {
        VerseReference verseReference = esvSyncNumberMap.keys().at(i);
        int syncNumber = esvSyncNumberMap.value(verseReference);

        queryAndCheck("update esv set parallel=" + QString::number(syncNumber) +
                      " where book_number=" + QString::number(verseReference.book) +
                      " and chapter="+ QString::number(verseReference.chapter) +
                      " and verse=" + QString::number(verseReference.verse));
    }

    qDebug() << "updated esv" << QDateTime::currentDateTime().time().toString();

    syncNumberMaps.insert(ESV, esvSyncNumberMap);

    updateWlc();

    qDebug() << "finished: " << QDateTime::currentDateTime().time().toString();

    db.close();

}
