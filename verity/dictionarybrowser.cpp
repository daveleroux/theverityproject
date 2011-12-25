#include "dictionarybrowser.h"
#include "globalvariables.h"
#include "strongsevent.h"
#include "parsingevent.h"
#include "eventmanager.h"
#include "biblequerier.h"

#include <QtSql>
#include <QXmlQuery>
#include <QByteArray>
#include <QDesktopServices>
#include <QTextBrowser>
#include <QDebug>
#include <QWebFrame>

DictionaryBrowser::DictionaryBrowser(QWidget* parent) : VWebView(parent)
{
    EventManager::addListener(EventType::STRONGS, this);

    javascriptClickListener = new JavascriptClickListener();
    page()->mainFrame()->addToJavaScriptWindowObject("javascriptClickListener", javascriptClickListener);
    connect(page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this , SLOT(javaScriptWindowObjectClearedSlot()));


    frameTop = "<html>"
               "<head>"
               "<style type=\"text/css\">\n";

    QFile tmp(DATA_PATH + "/bible.css"); //should get the css from a global resource (and get it each time) so when user changes settings changes everywhere
    if(!tmp.open(QIODevice::ReadOnly))
        exit(1);

    QByteArray byteArray = tmp.readAll();

    frameTop += QString::fromUtf8(byteArray.data());

    tmp.close();

    frameTop += "</style>"
                "</head>"
                "<body>"
                "<div>";

    frameBottom =  "</div>"
                   "</body>"
                   "</html>";

}

void DictionaryBrowser::javaScriptWindowObjectClearedSlot()
{
    page()->mainFrame()->addToJavaScriptWindowObject("javascriptClickListener", javascriptClickListener);
}

DictionaryBrowser::~DictionaryBrowser()
{
    delete javascriptClickListener;
}

void DictionaryBrowser::handleEvent(Event* event)
{
    if(event->getEventType() == EventType::STRONGS)
    {
        StrongsEvent* strongsEvent = static_cast<StrongsEvent*>(event);
        setNewContent(strongsEvent->strongsNum);
    }
    else
    {
        VWebView::handleEvent(event);
    }
}

QString DictionaryBrowser::transformToHtml(QString xml)
{
    xml.replace("<exegesis>", "<br/><exegesis>");
    xml.replace("<explanation>", "<br/><heading>Explanation</heading><explanation>");
    xml.replace("<translation>", "<br/><heading>Translation</heading><explanation>");
    xml.replace("<compare>", "<br/>Compare:<ul>");
    xml.replace("</compare>", "</ul>");
    xml.replace("<compareItem>", "<li>");
    xml.replace("</compareItem>", "</li>");

    xml.replace("<list>", "<ul>");
    xml.replace("</list>", "</ul>");
    xml.replace("<listItem>", "<li>");
    xml.replace("</listItem>", "</li>");

    xml.replace("<kjvDefinition>", "<br/><br/><heading>KJV Definition</heading><kjvDefinition>");
    xml.replace("<see>", "<see>See ");

    QMap<QString, QString> replaceMap;

    replaceMap.insert("entry", "entry");
    replaceMap.insert("heading", "heading");
    replaceMap.insert("hebrew", "hebrew");
    replaceMap.insert("morphology", "morphology");
    replaceMap.insert("exegesis", "exegesis");
    replaceMap.insert("explanation", "explanation");
    replaceMap.insert("translation", "translation");
    replaceMap.insert("greek", "greek");
    replaceMap.insert("b", "b");
    replaceMap.insert("i", "i");
    replaceMap.insert("kjvDefinition", "kjvDefinition");
    replaceMap.insert("see", "see");


    for(int i=0; i<replaceMap.keys().size(); i++)
    {
        QString key = replaceMap.keys().at(i);
        QString value = replaceMap.value(key);

        xml.replace("<"+key+">", "<span class=\""+value+"\">");
        xml.replace("</"+key+">", "</span>");
    }


    xml.replace(QRegExp("<link id=\"([0-9]*)\">([^<]*)</link>"), "<span class=\"link\" onclick=\"javascriptClickListener.strongsClicked(\\1)\">\\2</span>");

    return xml;
}


void DictionaryBrowser::setNewContent(int strongsNum)
{
    QString xml = BibleQuerier::getStrongsData(strongsNum);
    setHtml(frameTop + transformToHtml(xml) + frameBottom);
}

//QVariant DictionaryBrowser::loadResource(int type, const QUrl& url)
//{
//    QString result;

//    QString urlString = url.toString();

//    if(type == QTextDocument::HtmlResource)
//    {
//        if(urlString.startsWith("greek://"))
//        {
//            QString strongsNumber = urlString.replace("greek://","");

//            QSqlQuery query;
//            if(!query.exec("select greek, definition from strongs where strongs_number = " + strongsNumber))
//            {
//                qDebug() << "failed: " << query.lastError() << endl;
//                exit(1);
//            }

//            if(query.next())
//            {
//                QString greek = query.value(0).toString();
//                QString definition = query.value(1).toString();

//                result.append("<h3>"+greek+"</h3>");

//                result.append(convertDefinitionToHTML("style.xsl", definition));
//            }
//        }
//        else if(urlString.startsWith("hebrew://"))
//        {
//            QString strongsNumber = urlString.replace("hebrew://","");

//            QSqlQuery query;
//            if(!query.exec("select  definition from strongs_hebrew where strongs_number = " + strongsNumber))
//            {
//                qDebug() << "failed: " << query.lastError() << endl;
//                exit(1);
//            }


//            if(query.next())
//            {
//                QString definition = query.value(0).toString();
//                result.append(convertDefinitionToHTML("hebrew-style.xsl", definition));
//            }

//        }

//    }
//    return result;
//}

//void DictionaryBrowser::display(TextInfo* textInfo)
//{
//    QString prefix;
//    if(textInfo->bibleText == "tisch")
//        prefix = "greek://";
//    else if(textInfo->bibleText == "wlc")
//        prefix = "hebrew://";

//    QUrl url(prefix+ QString().setNum(textInfo->strongsNumber));
//    setSource(url);
//}

//QString DictionaryBrowser::convertDefinitionToHTML(QString stylesheet, QString input)
//{
//    QXmlQuery query(QXmlQuery::XSLT20);

//    QByteArray byteArray(input.toUtf8());

//    QBuffer buffer(&byteArray);
//    buffer.open(QBuffer::ReadWrite);

//    query.setFocus(&buffer);

//    query.setQuery(QUrl(DATA_PATH + "/" + stylesheet));

//    QString result;
//    query.evaluateTo(&result);

//    return result;
//}
