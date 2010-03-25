#ifndef BASETEXTUNIT_H
#define BASETEXTUNIT_H

#include <QString>
#include "textinfo.h"

class BaseTextUnit
{
public:
    int start,end;
    BaseTextUnit(int start, int end);
    bool operator<(const BaseTextUnit& rhs) const;

};

#endif // BASETEXTUNIT_H
