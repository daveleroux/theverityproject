#include <QtCore/QCoreApplication>
#include <QtSql>
#include <QDebug>
#include <QStringList>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("verity.sqlite");
    if (!db.open())
    {
        qDebug() << "couldn't open db" << endl;
        return 1;
    }


    QSqlQuery query;
    query.exec("drop table performance");

    if(!query.exec("create table performance (bibletext_id int, normalised_chapter int, min_id int, max_id int)"))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }


    if(!query.exec("insert into performance select bibletext_id, normalised_chapter, min(id), max(id) from bibles group by normalised_chapter, bibletext_id"))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }


    //query.exec("create index idx_performance on performance (bibletext_id)");

    db.commit();

    db.close();


    return 0;

}
