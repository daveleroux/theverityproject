#include <QtCore/QCoreApplication>
#include <QtSql>

void query(QString queryString)
{
    QSqlQuery query;
    query.exec(queryString) ;
}

void queryAndCheck(QString queryString)
{
    QSqlQuery query;
    if(!query.exec(queryString))
    {
        qDebug() << "failed: "<< query.lastError()  << endl;
        exit(1);
    }
}

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
    db.transaction();


    query("drop table bible_metadata");

    queryAndCheck("create table bible_metadata (bibletext_id integer primary key, rtl int)");

    queryAndCheck("insert into bible_metadata values (1,0)");
    queryAndCheck("insert into bible_metadata values (2,0)");
    queryAndCheck("insert into bible_metadata values (3,1)");


    query("drop table wordhandlers");

    queryAndCheck("create table wordhandlers (bibletext_id integer, wordhandler_id int)");

    queryAndCheck("insert into wordhandlers values (2,0)");
    queryAndCheck("insert into wordhandlers values (2,1)");
    queryAndCheck("insert into wordhandlers values (3,0)");
    queryAndCheck("insert into wordhandlers values (3,2)");

    db.commit();
    db.close();

    return 0;
}
