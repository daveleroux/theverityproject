#include <QtCore/QCoreApplication>
#include "CLucene.h"
#include "CLucene/CLConfig.h"
#include "CLucene/analysis/Analyzers.h"
#include <QDebug>

using namespace lucene::index;
using namespace lucene::analysis;
using namespace lucene::util;
using namespace lucene::store;
using namespace lucene::document;
using namespace lucene::queryParser;
using namespace lucene::search;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);




    WhitespaceAnalyzer* whitespaceAnalyzer = new WhitespaceAnalyzer();
    IndexWriter* writer = new IndexWriter("luceneIndex", whitespaceAnalyzer, true);


    Document doc;
    doc.add(*new Field(_T("book"), _T("1"), Field::STORE_YES | Field::INDEX_UNTOKENIZED));
    doc.add(*new Field(_T("chapter"), _T("1"), Field::STORE_YES | Field::INDEX_UNTOKENIZED));
    doc.add(*new Field(_T("verse"), _T("1"), Field::STORE_YES | Field::INDEX_UNTOKENIZED));
    doc.add(*new Field(_T("contents"), _T("In the beginning, God created the heavens and the earth"), Field::STORE_YES | Field::INDEX_TOKENIZED));
    writer->addDocument( &doc );


    doc.clear();
    doc.add(*new Field(_T("book"), _T("1"), Field::STORE_YES | Field::INDEX_UNTOKENIZED));
    doc.add(*new Field(_T("chapter"), _T("1"), Field::STORE_YES | Field::INDEX_UNTOKENIZED));
    doc.add(*new Field(_T("verse"), _T("2"), Field::STORE_YES | Field::INDEX_UNTOKENIZED));
    doc.add(*new Field(_T("contents"), _T("The earth was without form and void, and darkness was over the face of the deep."
                                          " And the Spirit of God was hovering over the face of the waters."), Field::STORE_YES | Field::INDEX_TOKENIZED));
    writer->addDocument( &doc );


    writer->close();





    try
    {
        Query* query = QueryParser::parse(_T("hovering"), _T("contents"),whitespaceAnalyzer);
        IndexSearcher searcher("luceneIndex");
        Hits* hits = searcher.search(query);

        int max = hits->length() > 100? 100 : hits->length();

        qDebug() << max;

        for(int i=0; i<max; i++)
        {
            doc = hits->doc(i);
            qDebug() << QString::fromWCharArray(doc.get(_T("book"))) << " "
                    << QString::fromWCharArray(doc.get(_T("chapter"))) << ":"
                    << QString::fromWCharArray(doc.get(_T("verse")));
            qDebug() << QString::fromWCharArray(doc.get(_T("contents")));
        }
    }
    catch (CLuceneError e)
    {
        qDebug() << e.what();
    }


    return 0;
}
