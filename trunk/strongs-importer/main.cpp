#include <QtCore/QCoreApplication>
#include <QDomDocument>
#include <QDomNodeList>
#include <QDomNode>
#include <QDebug>
#include <QFile>
#include <QtSql>


QMap<int, QString> strongsNumberToGreek;
QMap<int, QString> strongsNumberToHebrew;

QString convertToXmlStringStart(QDomNode node)
{
    if(node.isText())
    {
        QString result = node.toText().data();

        if(result.startsWith("--"))
            return result.mid(2);

        if(result.startsWith(":--"))
            return result.mid(3);

        return result;
    }
    else if (node.isElement())
    {
        if(node.nodeName() == "strongsref")
        {
            QString language = node.toElement().attribute("language");
            language = language.toLower();
            QString strongsNumber = node.toElement().attribute("strongs");
            int strongsNumberInt = strongsNumber.toInt();
            strongsNumber = QString().setNum(strongsNumberInt);

            QString lemma;
            if(language == "greek")
            {
                lemma = strongsNumberToGreek.value(strongsNumberInt);
            }
            else if(language == "hebrew")
            {
                lemma = strongsNumberToHebrew.value(strongsNumberInt);
            }

            return "<strongsref language='" + language + "' strongs_number='" + strongsNumber + "'>"
                    + lemma + "</strongsref>";
        }
        else if(node.nodeName() == "see")
        {
            QString language = node.toElement().attribute("language");
            language = language.toLower();
            QString strongsNumber = node.toElement().attribute("strongs");
            int strongsNumberInt = strongsNumber.toInt();
            strongsNumber = QString().setNum(strongsNumberInt);

            QString lemma;
            if(language == "greek")
            {
                lemma = strongsNumberToGreek.value(strongsNumberInt);
            }
            else if(language == "hebrew")
            {
                lemma = strongsNumberToHebrew.value(strongsNumberInt);
            }
                return "<see language='" + language + "' strongs_number='" + strongsNumber + "'>"
                        + lemma + "</see>";


        }
        else if(node.nodeName() == "strongs_def")
        {
            return "<strongs_definition>";
        }
        else if(node.nodeName() == "kjv_def")
        {
            return "<kjv_definition>";
        }
        else if(node.nodeName() == "greek")
        {
            return node.toElement().attribute("unicode") + " ";
        }
        else if(node.nodeName() == "latin")
        {
            return "<latin>";
        }
    }
    return 0;
}

QString convertToXmlStringEnd(QDomNode node)
{
    if (node.isElement())
    {
        if(node.nodeName() == "strongs_def")
        {
            return "</strongs_definition>";
        }
        else if(node.nodeName() == "kjv_def")
        {
            return "</kjv_definition>";
        }
        else if(node.nodeName() == "latin")
        {
            return "</latin>";
        }
    }
    return 0;
}

void convertToXmlStringRecursively(QDomNode node, QString* result)
{
    for(int i=0; i<node.childNodes().size(); i++)
    {
        QDomNode childNode = node.childNodes().at(i);

        QString simpleResult = convertToXmlStringStart(childNode);
        if(simpleResult != 0)
            result->append(simpleResult);

        convertToXmlStringRecursively(childNode, result);

        simpleResult = convertToXmlStringEnd(childNode);
        if(simpleResult != 0)
            result->append(simpleResult);
    }
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

    if(!query.exec("create table strongs (strongs_number integer primary key, greek varchar(50), definition varchar(5000))"))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

    if(!query.exec("select strongs_number, hebrew from strongs_hebrew"))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

    while(query.next())
    {
        strongsNumberToHebrew.insert(query.value(0).toInt(), query.value(1).toString());
    }

    QDomDocument doc("mydocument");
    QFile file("strongsgreek.xml");
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

        strongsNumberToGreek.insert(strongsNumber, greek);
    }

    for(int i=0; i<domNodeList.size(); i++)
    {
        QDomNode entryNode = domNodeList.at(i);

        QDomNode strongsNumberNode = ((QDomElement&)entryNode).elementsByTagName("strongs").at(0);
        QString strongsNumberString = strongsNumberNode.toElement().firstChild().toText().data();
        int strongsNumber = strongsNumberString.toInt();

        entryNode.removeChild(strongsNumberNode);

        QDomNode greekNode = ((QDomElement&)entryNode).elementsByTagName("greek").at(0);
        QString greek = ((QDomElement&)greekNode).attribute("unicode");

        //        entryNode.removeChild(greekNode);

        QString* result = new QString();

        result->append("<?xml version='1.0' encoding='UTF-8'?>");
        result->append("<entry>");
        convertToXmlStringRecursively(entryNode, result);
        result->append("</entry>");


        query.prepare("insert into strongs values(?,?,?)");

        query.addBindValue(strongsNumber);
        query.addBindValue(greek);
        query.addBindValue(*result);

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
