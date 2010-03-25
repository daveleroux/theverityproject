#include "basetextunit.h"

BaseTextUnit::BaseTextUnit(int start, int end)
{
    this->start = start;
    this->end = end;
}

bool BaseTextUnit::operator<(const BaseTextUnit& rhs) const
{
    return end < rhs.start;
}

