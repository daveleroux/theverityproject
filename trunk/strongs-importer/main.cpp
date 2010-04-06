#include <QtCore/QCoreApplication>
#include <QDomDocument>
#include <QDomNodeList>
#include <QDomNode>
#include <QDebug>
#include <QFile>
#include <QtSql>

QString combineAllTextChildren(QDomNode node)
{
    QString result;

         QDomNodeList children = node.childNodes();
         for(int j=0; j<children.size(); j++)
         {
             QDomNode child = children.at(j);
             if(child.isText())
             {
                 result = result + child.toText().data();
             }
             else
             {
                 if(result.length()>0)
                    result = result + "[]";
             }
         }

    return result;
}

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
     query.exec("drop table strongs");

     if(!query.exec("create table strongs (strongs_number integer primary key, "
                "greek varchar(50), strongs_def varchar(1000), kjv_def varchar(1000), strongs_derivation varchar(1000), strongs_other varchar(1000))"))
     {
         qDebug() << "failed: " << query.lastError() << endl;
         exit(1);
     }


     QDomDocument doc("mydocument");
     QFile file("StrongsGreekDictionaryXML_1.4/strongsgreek.xml");
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

     QDomNodeList domNodeList = doc.elementsByTagName("entry");

     for(int i=0; i<domNodeList.size(); i++)
     {
         QDomNode entryNode = domNodeList.at(i);

         QDomNode strongsNumberNode = ((QDomElement&)entryNode).elementsByTagName("strongs").at(0);
         QString strongsNumberString = strongsNumberNode.toElement().firstChild().toText().data();
         int strongsNumber = strongsNumberString.toInt();

         QDomNode greekNode = ((QDomElement&)entryNode).elementsByTagName("greek").at(0);
         QString greek = ((QDomElement&)greekNode).attribute("unicode");


         QDomNode strongsDefNode = ((QDomElement&)entryNode).elementsByTagName("strongs_def").at(0);
         QString strongsDef = combineAllTextChildren(strongsDefNode);

         QDomNode kjvDefNode = ((QDomElement&)entryNode).elementsByTagName("kjv_def").at(0);
         QString kjvDef = combineAllTextChildren(kjvDefNode);

         if(kjvDef.startsWith("--"))
             kjvDef = kjvDef.mid(2);

         if(kjvDef.startsWith(":--"))
             kjvDef = kjvDef.mid(3);

         if(kjvDef.endsWith("."))
             kjvDef = kjvDef.left(kjvDef.length()-1);

         QDomNode strongsDerivationNode = ((QDomElement&)entryNode).elementsByTagName("strongs_derivation").at(0);
         QString strongsDerivation = combineAllTextChildren(strongsDerivationNode);

         QString strongsOther = combineAllTextChildren(entryNode);

          query.prepare("insert into strongs values(?,?,?,?,?,?)");

                query.addBindValue(strongsNumber);
                query.addBindValue(greek);
                query.addBindValue(strongsDef);
                query.addBindValue(kjvDef);
                query.addBindValue(strongsDerivation);
                query.addBindValue(strongsOther);

                if(!query.exec())
                {
                    qDebug() << "failed: "<< query.lastError()  << endl;
                    exit(1);

                }

     }

    query.exec("create index idx_strongs on strongs (strongs_number)");
    db.close();

    return 0;
}
