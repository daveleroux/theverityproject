#ifndef PARALLELDTO_H
#define PARALLELDTO_H

#include <QMap>
#include <QString>

struct ParallelDTO
{
    QString xml;
    QMap<int, int> firstIdsMap;
    QMap<int, int> lastIdsMap;

    ParallelDTO(QString xml, QMap<int, int> firstIdsMap, QMap<int, int> lastIdsMap);
};

#endif // PARALLELDTO_H
