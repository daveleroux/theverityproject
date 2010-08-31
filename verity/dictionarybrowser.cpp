#include "dictionarybrowser.h"
#include "globalvariables.h"

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

    if(type == QTextDocument::HtmlResource)
    {
        if(urlString.startsWith("greek://"))
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

                result.append(convertDefinitionToHTML("style.xsl", definition));
            }
        }
        else if(urlString.startsWith("hebrew://"))
        {
            QString strongsNumber = urlString.replace("hebrew://","");

            QSqlQuery query;
            if(!query.exec("select  definition from strongs_hebrew where strongs_number = " + strongsNumber))
            {
                qDebug() << "failed: " << query.lastError() << endl;
                exit(1);
            }


            if(query.next())
            {
                QString definition = query.value(0).toString();
                result.append(convertDefinitionToHTML("hebrew-style.xsl", definition));
            }

        }

    }
    return result;
}

void DictionaryBrowser::display(QString text, TextInfo textInfo)
{
    QString prefix;
    if(text == "tisch")
        prefix = "greek://";
    else if(text == "wlc")
        prefix = "hebrew://";

    QUrl url(prefix+ QString().setNum(textInfo.strongsNumber));
    setSource(url);
}

QString DictionaryBrowser::convertDefinitionToHTML(QString stylesheet, QString input)
{
    QXmlQuery query(QXmlQuery::XSLT20);

    QByteArray byteArray(input.toUtf8());

    QBuffer buffer(&byteArray);
    buffer.open(QBuffer::ReadWrite);

    query.setFocus(&buffer);

    query.setQuery(QUrl(DATA_PATH + "/" + stylesheet));

    QString result;
    query.evaluateTo(&result);

    return result;
}
