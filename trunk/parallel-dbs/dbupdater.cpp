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

void DbUpdater::updateTisch()
{
    QList<Rule*> rules;

    StandardRule standardRule(this);
    rules.append(&standardRule);

    updateText(TISCH, rules);
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

    highestUnusedSyncNumber = 1;

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
    updateTisch();

    qDebug() << "finished: " << QDateTime::currentDateTime().time().toString();

    db.close();

}
