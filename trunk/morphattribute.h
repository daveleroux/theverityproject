#ifndef MORPHATTRIBUTE_H
#define MORPHATTRIBUTE_H

#include <QString>
#include <QBitArray>

class MorphAttribute
{
public:
    QBitArray bitArray;
    QString type;
    QString value;

    MorphAttribute(QBitArray bitArray, QString type, QString value);
};

#endif // MORPHATTRIBUTE_H
