#ifndef PARALLELDTO_H
#define PARALLELDTO_H

#include <QMap>
#include <QString>

struct ParallelDTO
{
    QString xml;
    QMap<int, int> firstIdsMap;
    QMap<int, int> lastIdsMap;
    bool evenNumberOfRows;

    ParallelDTO(QString xml, QMap<int, int> firstIdsMap, QMap<int, int> lastIdsMap, bool evenNumberOfRows);
};

#endif // PARALLELDTO_H
