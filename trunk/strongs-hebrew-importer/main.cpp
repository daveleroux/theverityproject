#include <QtCore/QCoreApplication>
#include <QtSql>
#include <QDomDocument>
#include <QDomNodeList>
#include <QDomNode>
#include <QDebug>
#include <QFile>

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

    QSqlQuery query;
    query.exec("drop table strongs_hebrew");

    if(!query.exec("create table strongs_hebrew (strongs_number integer primary key, hebrew varchar(100), definition varchar(5000))"))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }


    QDomDocument doc("mydocument");
    QFile file("StrongHebrewG.xml");
    if (!file.open(QIODevice::ReadOnly))
        exit(1);

    QByteArray byteArray = file.readAll();

    QString wholeFile = QString::fromUtf8(byteArray.data());

    wholeFile = wholeFile.simplified();


    if(!doc.setContent(wholeFile))
    {
        file.close();
        qDebug() << "xml not well formed";
        exit(1);
    }
    file.close();

    QDomNodeList domNodeList = doc.elementsByTagName("div");

    qDebug() << "domNodeList.size() " << domNodeList.size();

    for(int i=0; i<domNodeList.size(); i++)
    {
        QDomNode divNode = domNodeList.at(i);
        QString type = ((QDomElement&)divNode).attribute("type");
        if(type == "entry")
        {
            QString number = ((QDomElement&)divNode).attribute("n");
            QString entryAsString;
            QTextStream stream(&entryAsString);
            stream << divNode;

            QDomNodeList wNodeList = ((QDomElement&)divNode).elementsByTagName("w");
            QDomNode firstWNode = wNodeList.at(0);
            QString hebrew = ((QDomElement&)firstWNode).attribute("lemma");

            entryAsString.remove(0, entryAsString.indexOf(">")+1);
            entryAsString.remove(entryAsString.lastIndexOf("<"),6);

            entryAsString.insert(0, "<?xml version='1.0' encoding='UTF-8'?><entry>");
            entryAsString.append("</entry>");

            query.prepare("insert into strongs_hebrew values(?,?,?)");

            query.addBindValue(number);
            query.addBindValue(hebrew);
            query.addBindValue(entryAsString);

            if(!query.exec())
            {
                qDebug() << "failed: "<< query.lastError()  << endl;
                exit(1);

            }


        }

    }

    query.exec("create index idx_strongs_hebrew on strongs_hebrew (strongs_number)");
    db.close();

    return 0;
}
