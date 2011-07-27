#include "paralleldto.h"

ParallelDTO::ParallelDTO(QString xml, QMap<int, int> firstIdsMap, QMap<int, int> lastIdsMap)
{
    this->xml = xml;
    this->firstIdsMap = firstIdsMap;
    this->lastIdsMap = lastIdsMap;
}
