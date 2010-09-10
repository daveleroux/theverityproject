#include <QtCore/QCoreApplication>
#include "dbupdater.h"



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DbUpdater dbUpdater;
    dbUpdater.update();

    return 0;
}
