#include <QtCore/QCoreApplication>
#include "CLucene.h"
#include "CLucene/CLConfig.h"
#include "CLucene/analysis/Analyzers.h"
#include <QDebug>
#include <QtSql>
#include "netanalyzer.h"


using namespace lucene::index;
using namespace lucene::analysis;
using namespace lucene::queryParser;
using namespace lucene::search;
using namespace lucene::util;
using namespace lucene::store;
using namespace lucene::document;

wchar_t* convert(QString input)
{
    wchar_t* arr = new wchar_t[input.length()+1];
    int size = input.toWCharArray(arr);
    arr[size] = '\0';
    return arr;        
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    QString INDEX_NAME = "netLuceneIndex";


    NetAnalyzer* netAnalyzer = new NetAnalyzer();
    Document doc;


//    try
//    {
//        Query* query = QueryParser::parse(convert(QString("rest years")), _T("contentsToSearch"),netAnalyzer);
//        IndexSearcher searcher(INDEX_NAME.toLatin1().data());
//        Hits* hits = searcher.search(query);

//        int max = hits->length() > 100? 100 : hits->length();

//        qDebug() << max;

//        for(int i=0; i<max; i++)
//        {
//            doc = hits->doc(i);
//            qDebug() << QString::fromWCharArray(doc.get(_T("book"))) << " "
//                    << QString::fromWCharArray(doc.get(_T("chapter"))) << ":"
//                    << QString::fromWCharArray(doc.get(_T("verse")));
//            qDebug() << QString::fromWCharArray(doc.get(_T("contentsToDisplay")));
//        }
//    }
//    catch (CLuceneError e)
//    {
//        qDebug() << e.what();
//    }
//    return 0;



        IndexWriter* writer = new IndexWriter(INDEX_NAME.toLatin1().data(), netAnalyzer, true);
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("verity.sqlite");
        if (!db.open())
        {
            qDebug() << "couldn't open db" << endl;
            return 1;
        }

        QSqlQuery query;
        query.exec("select book_number, chapter, verse, text from bibles where bibletext_id=1 and verse>0");

        int count = 0;
        while(query.next())
        {
            int book_number = query.value(0).toInt();
            int chapter = query.value(1).toInt();
            int verse = query.value(2).toInt();
            QString text = query.value(3).toString();

            QList<QString> regsToWhack;

            regsToWhack.append("<chapter>.*</chapter>");
            regsToWhack.append("<verse>.*</verse>");
            regsToWhack.append("<netNote.*</netNote>");
            regsToWhack.append("&.*;");
            regsToWhack.append("<paragraphTitle>.*</paragraphTitle>");
            regsToWhack.append("<speakerHeading>.*</speakerHeading>");

            foreach(QString regToWhack, regsToWhack)
            {
                QRegExp regEx(regToWhack);
                regEx.setMinimal(true);
                text.replace(regEx,"");
            }

            text.replace(QRegExp("<[^>]*>"),"");

            QString textToDisplay = text;

            text.replace(QRegExp(QString::fromUtf8("[,.“”!?–();‘’]")), " ");
            text = text.trimmed();

            doc.clear();
            doc.add(*new Field(_T("book"), convert(QString().setNum(book_number)), Field::STORE_YES | Field::INDEX_UNTOKENIZED));
            doc.add(*new Field(_T("chapter"), convert(QString().setNum(chapter)), Field::STORE_YES | Field::INDEX_UNTOKENIZED));
            doc.add(*new Field(_T("verse"), convert(QString().setNum(verse)), Field::STORE_YES | Field::INDEX_UNTOKENIZED));
            doc.add(*new Field(_T("contentsToSearch"), convert(text), Field::STORE_NO | Field::INDEX_TOKENIZED));
            doc.add(*new Field(_T("contentsToDisplay"), convert(textToDisplay), Field::STORE_YES | Field::INDEX_UNTOKENIZED));
            writer->addDocument( &doc );

            count++;
            if(count % 1000 == 0)
                   qDebug() << count;
        }

        writer->optimize();

        writer->close();
        db.close();
        return 0;
}
