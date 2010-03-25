#include "morphattribute.h"

MorphAttribute::MorphAttribute(QBitArray bitArray, QString type, QString value)
{
    this->bitArray = bitArray;
    this->type = type;
    this->value = value;
}
