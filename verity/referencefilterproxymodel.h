#ifndef REFERENCEFILTERPROXYMODEL_H
#define REFERENCEFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class ReferenceFilterProxyModel : public QSortFilterProxyModel
{
Q_OBJECT
public:
    ReferenceFilterProxyModel(QObject *parent = 0);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
signals:

public slots:

};

#endif // REFERENCEFILTERPROXYMODEL_H
