#ifndef DBUPDATER_H
#define DBUPDATER_H

#include <QMap>
#include "versereference.h"
#include <QtSql>
#include <QDebug>

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

    void updateWlc();

};

#endif // DBUPDATER_H
