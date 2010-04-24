#include "dictionarybrowser.h"

#include <QtSql>
#include <QXmlQuery>
#include <QByteArray>

DictionaryBrowser::DictionaryBrowser(QWidget* parent) : QTextBrowser(parent)
{
    setOpenLinks(false);
    connect(this, SIGNAL(anchorClicked(QUrl)), this, SLOT(display(QUrl)));
}

void DictionaryBrowser::display(TextInfo textInfo)
{
    display(QUrl("GREEK:" + QString().setNum(textInfo.strongsNumber)));
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

void DictionaryBrowser::display(QUrl url)
{
    QString strongsNumber = url.toString();
    if(strongsNumber.startsWith("GREEK:"))
    {
        strongsNumber.replace("GREEK:","");

        clear();
        QTextCursor textCursor(document());

        QTextCharFormat boldFormat;
        boldFormat.setFontWeight(QFont::Bold);

        QTextCharFormat normalFormat;


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

            textCursor.insertHtml("<h3>"+greek+"</h3>");
            textCursor.insertText("\n");

            textCursor.insertHtml(convertDefinitionToHTML(definition));
        }

    }

}
