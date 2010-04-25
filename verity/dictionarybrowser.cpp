#include "dictionarybrowser.h"

#include <QtSql>
#include <QXmlQuery>
#include <QByteArray>
#include <QDesktopServices>

DictionaryBrowser::DictionaryBrowser(QWidget* parent) : QTextBrowser(parent)
{

}


QVariant DictionaryBrowser::loadResource(int type, const QUrl& url)
{
    QString result;

    QString urlString = url.toString();

    if(type == QTextDocument::HtmlResource && urlString.startsWith("greek://"))
    {
        QString strongsNumber = urlString.replace("greek://","");

        QSqlQuery query;
        if(!query.exec("select greek, definition from strongs where strongs_number = " + strongsNumber))
        {
            qDebug() << "failed: " << query.lastError() << endl;
            exit(1);
        }


        if(query.next())
        {
            QString greek = query.value(0).toString();
            QString definition = query.value(1).toString();

            result.append("<h3>"+greek+"</h3>");

            result.append(convertDefinitionToHTML(definition));
        }

    }
    return result;
}

void DictionaryBrowser::display(TextInfo textInfo)
{
    QUrl url("greek://"+ QString().setNum(textInfo.strongsNumber));
    setSource(url);
}

QString DictionaryBrowser::convertDefinitionToHTML(QString input)
{
    QXmlQuery query(QXmlQuery::XSLT20);

    QByteArray byteArray(input.toUtf8());

    QBuffer buffer(&byteArray);
    buffer.open(QBuffer::ReadWrite);

     query.setFocus(&buffer);

     query.setQuery(QUrl("style.xsl"));

     QString result;
     query.evaluateTo(&result);

     return result;
}
