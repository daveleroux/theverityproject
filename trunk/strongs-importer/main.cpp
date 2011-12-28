#include <QtCore/QCoreApplication>
#include <QDomDocument>
#include <QDomNodeList>
#include <QDomNode>
#include <QDebug>
#include <QFile>
#include <QtSql>
#include <QXmlQuery>
#include <QByteArray>


QMap<int, QString> strongsNumberToGreek;

struct Entry
{
    QString word;
    QString xml;

    Entry(QString word, QString xml)
    {
        this->word = word;
        this->xml = xml;
    }
    Entry()
    {
    }
};

QMap<int, Entry> hebrewMap;
QMap<int, Entry> greekMap;

int HEBREW_COUNT = 8674;

void assert(int l, int r)
{
    if ( l != r)
    {
        qDebug() << "assert failed: " << l << " : " << r;
        exit(1);
    }
}

void assert(QString l, QString r)
{
    if( l != r)
    {
        qDebug() << "assert failed: " << l << " : " << r;
        exit(1);
    }
}

void assert(bool b)
{
    if(!b)
    {
        qDebug() << "assert failed";
        exit(1);
    }
}


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

            QString lemma;
            if(language == "greek")
            {
                lemma = strongsNumberToGreek.value(strongsNumberInt);
                strongsNumberInt += HEBREW_COUNT;
            }
            else if(language == "hebrew")
            {
                lemma = hebrewMap.value(strongsNumberInt).word;
            }

            return "<strongsref language='" + language + "' strongs_number='" + QString().setNum(strongsNumberInt) + "' lemma='" + lemma +"'></strongsref>";
        }
        else if(node.nodeName() == "see")
        {
            QString language = node.toElement().attribute("language");
            language = language.toLower();
            QString strongsNumber = node.toElement().attribute("strongs");
            int strongsNumberInt = strongsNumber.toInt();


            QString lemma;
            if(language == "greek")
            {
                lemma = strongsNumberToGreek.value(strongsNumberInt);
                strongsNumberInt += HEBREW_COUNT;
            }
            else if(language == "hebrew")
            {
                lemma = hebrewMap.value(strongsNumberInt).word;
            }
            return "<see language='" + language + "' strongs_number='" + QString().setNum(strongsNumberInt) + "' lemma='" + lemma +"'></see>";


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
            return "<greek>" + node.toElement().attribute("unicode") + " </greek>";
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


QDomDocument getGreekDocument()
{
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
    return doc;
}

void constructGreekHash()
{
    QDomDocument doc = getGreekDocument();
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
}

void importGreek()
{
    QDomDocument doc = getGreekDocument();

    QDomNodeList domNodeList = doc.elementsByTagName("entry");

    for(int i=0; i<domNodeList.size(); i++)
    {
        QDomNode entryNode = domNodeList.at(i);

        QDomNode strongsNumberNode = ((QDomElement&)entryNode).elementsByTagName("strongs").at(0);
        entryNode.removeChild(strongsNumberNode);

        int strongsNumber = strongsNumberToGreek.keys().at(i);
        QString greek = strongsNumberToGreek.value(strongsNumber);

        QString* result = new QString();

        result->append("<?xml version='1.0' encoding='UTF-8'?>");
        result->append("<entry>");
        convertToXmlStringRecursively(entryNode, result);
        result->append("</entry>");

        Entry entry(greek, *result);
        greekMap.insert(strongsNumber, entry);

    }
}

void importHebrew()
{
    QDomDocument doc("mydocument");
    QFile file("StrongHebrew.1.1/StrongHebrewG.xml");
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

    for(int i=0; i<domNodeList.size(); i++)
    {
        QDomNode divNode = domNodeList.at(i);
        QString type = ((QDomElement&)divNode).attribute("type");
        if(type == "entry")
        {
            QDomNodeList foreignList = ((QDomElement&)divNode).elementsByTagName("foreign");
            if(foreignList.size() > 0)
            {
                assert(foreignList.size(), 1);
                QDomNode foreignNode = foreignList.at(0);
                assert(((QDomElement&)foreignNode).attribute("xml:lang"), "grc");
                QDomNodeList wList = ((QDomElement&)foreignNode).elementsByTagName("w");
                for(int j=0; j<wList.size(); j++)
                {
                    QDomElement wNode = wList.at(j).toElement();
                    QString ref = wNode.attribute("gloss");
                    assert(ref.startsWith("G:"));
                    int original = ref.mid(2).toInt();
                    QString greek = strongsNumberToGreek.value(original);

                    wNode.setAttribute("greek", greek);

                    int refInt = original + HEBREW_COUNT;
                    wNode.setAttribute("gloss",refInt);
                }
            }

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

//            if(i==1)
//            {
//                qDebug() << entryAsString;
//                exit(0);
//            }

            Entry entry(hebrew, entryAsString);
            hebrewMap.insert(number.toInt(), entry);
        }

    }

}

QString transform(QString stylesheet, QString input)
{
    QXmlQuery query(QXmlQuery::XSLT20);

    QByteArray byteArray(input.toUtf8());

    QBuffer buffer(&byteArray);
    buffer.open(QBuffer::ReadWrite);

    query.setFocus(&buffer);


    query.setQuery(QUrl(stylesheet));

    QString result;
    query.evaluateTo(&result);

    return result;
}

void insertIntoDb(int number, QString word, QString xml)
{
    QSqlQuery query;
    query.prepare("insert into strongs values(?,?,?)");

    query.addBindValue(number);
    query.addBindValue(word);
    query.addBindValue(xml);

    if(!query.exec())
    {
        qDebug() << "failed: "<< query.lastError()  << endl;
        exit(1);

    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


//    QFile testfile("test.txt");
//    if (!testfile.open(QIODevice::ReadOnly))
//        exit(1);

//    QByteArray byteArray = testfile.readAll();

//    QString testString = QString::fromUtf8(byteArray.data());

//    qDebug() << transform("hebrew-style.xsl", testString);
//    exit(0);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("verity.sqlite");
    if (!db.open())
    {
        qDebug() << "couldn't open db" << endl;
        return 1;
    }

    db.transaction();

    QSqlQuery query;
    query.exec("drop table strongs");

    if(!query.exec("create table strongs (strongs_number integer primary key, word varchar(50), definition varchar(5000))"))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

    constructGreekHash();
    qDebug() << "constructed greek hash";

    importHebrew();
    qDebug() << "done hebrew first pass";

    importGreek();
    qDebug() << "done greek first pass";


    foreach(int key, hebrewMap.keys())
    {
        Entry entry = hebrewMap.value(key);
        insertIntoDb(key, entry.word, transform("hebrew-style.xsl", entry.xml));
    }
    qDebug() << "done hebrew second pass";

    foreach(int key, greekMap.keys())
    {
        Entry entry = greekMap.value(key);
        insertIntoDb(key + HEBREW_COUNT, entry.word, transform("style.xsl", entry.xml));
    }
    qDebug() << "done greek second pass";

    query.exec("create index idx_strongs on strongs (strongs_number)");
    db.commit();
    db.close();

    return 0;
}
