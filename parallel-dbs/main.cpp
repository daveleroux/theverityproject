#include <QtCore/QCoreApplication>
#include "dbupdater.h"
#include "versereference.h"
#include <QMap>
#include <QDebug>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DbUpdater dbUpdater;
    dbUpdater.update();

    return 0;
}
