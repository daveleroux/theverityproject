#ifndef DBUPDATER_H
#define DBUPDATER_H

#include <QMap>
#include "versereference.h"
#include <QtSql>
#include <QDebug>

class Rule;

class DbUpdater
{
public:
    DbUpdater();
    void update();

    QMap<QString, QMap<VerseReference, int> > syncNumberMaps;
    int highestUnusedSyncNumber;

private:
    QSqlQuery query(QString queryString, bool mustCheck=false);
    QSqlQuery queryAndCheck(QString queryString);

    void updateText(QString text, QList<Rule*> rules);


    void updateWlc();
    void updateTisch();
};

#endif // DBUPDATER_H
