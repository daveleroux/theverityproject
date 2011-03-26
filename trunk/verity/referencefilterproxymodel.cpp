#include "referencefilterproxymodel.h"

#include <QDebug>

ReferenceFilterProxyModel::ReferenceFilterProxyModel(QObject *parent) :
        QSortFilterProxyModel(parent)
{
}

bool ReferenceFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);

    //check for known tag (and exclude if text is known tag for something else?)
    //check for starting text (minus spaces etc.)
    //check for contains

    QString fish = sourceModel()->data(index0).toString().toLower();
    fish.replace(" ", "", Qt::CaseSensitive);



    if (fish.contains(
                this->filterRegExp().pattern().toAscii()
            ))
    {
        return true;
    }
    else
    {
        return false;
    }
}
