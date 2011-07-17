#ifndef DBUPDATER_H
#define DBUPDATER_H

#include <QMap>
#include <QHash>
#include "versereference.h"
#include <QtSql>
#include <QDebug>

class Rule;

class DbUpdater
{
public:
    DbUpdater();
    void update();

    QMap<QString, QMap<VerseReference, int>* > syncNumberMaps;

    QMap<QString, QHash<VerseReference, int>* > originalSyncMaps;

    int highestUnusedSyncNumber;

private:
    QSqlDatabase db;
    int queryCount;

    QSqlQuery query(QString queryString, bool mustCheck=false);
    QSqlQuery queryAndCheck(QString queryString);

    void updateText(QList<Rule*> rules);


    void updateEsv();

    void updateKjv();
    void updateWlc();
    void updateTisch();

    bool mustUpdate(QString, VerseReference, int);

    bool MUST_PERSIST;
};

#endif // DBUPDATER_H
