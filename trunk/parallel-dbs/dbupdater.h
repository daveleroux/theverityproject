#ifndef DBUPDATER_H
#define DBUPDATER_H

#include <QMap>
#include <QHash>
#include "versereference.h"
#include <QtSql>
#include <QDebug>
#include "syncnumberlookup.h"

class Rule;

class DbUpdater
{
public:
    DbUpdater();
    void update();

//    QMap<QString, QMap<VerseReference, int>* > syncNumberMaps;
    QMap<QString, SyncNumberLookup* > syncNumberMaps;

//    QMap<QString, QHash<VerseReference, int>* > originalSyncMaps;

    int highestUnusedSyncNumber;

private:
    QSqlDatabase db;
    int queryCount;

    QSqlQuery query(QString queryString, bool mustCheck=false);
    QSqlQuery queryAndCheck(QString queryString);

    void updateText(QString text, int bibletextId, QList<Rule*> rules);


    void updateEsv();

    void updateKjv();
    void updateWlc();
    void updateTisch();

    bool mustUpdate(QString, VerseReference, int);

    bool MUST_PERSIST;
};

#endif // DBUPDATER_H
