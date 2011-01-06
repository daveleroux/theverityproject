#include <QtCore/QCoreApplication>
#include <QtSql>
#include <QDebug>
#include <QStringList>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("bibles.sqlite");
    if (!db.open())
    {
        qDebug() << "couldn't open db" << endl;
        return 1;
    }

    QStringList texts;
    texts << "esv" << "kjv" << "tisch" << "wlc";

    QSqlQuery query;
    query.exec("drop table performance");

    if(!query.exec("create table performance (text varchar(50), normalised_chapter int, min_id int, max_id int)"))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

    for(int i=0; i<texts.size(); i++)
    {
        QString text = texts.at(i);

        if(!query.exec("insert into performance select \""+ text +"\", normalised_chapter, min(id), max(id) from "+text+" group by normalised_chapter"))
        {
            qDebug() << "failed: " << query.lastError() << endl;
            exit(1);
        }

    }



    db.commit();

    db.close();


    return 0;

}
