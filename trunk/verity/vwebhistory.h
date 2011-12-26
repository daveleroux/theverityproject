#ifndef VWEBHISTORY_H
#define VWEBHISTORY_H

#include <QObject>
#include "vwebhistoryitem.h"
#include "vwebview.h"

class VWebHistory : public QObject
{
    Q_OBJECT

private:
    int currentIndex;
    QList<VWebHistoryItem*> items;
    int limit;

    bool currentNotNull();

    void addItemAndDisplay(VWebHistoryItem* item);
    void deleteItemsAheadOfCurrent();

    bool canGoForward();
    bool canGoBackward();

public:
    VWebHistory(int limit=20);
    ~VWebHistory();
    void display(VWebHistoryItem* item);
    void backward();
    void forward();

    VWebHistoryItem* getCurrentOrNull();

signals:
    void backwardAvailable(bool);
    void forwardAvailable(bool);

};

#endif // VWEBHISTORY_H
