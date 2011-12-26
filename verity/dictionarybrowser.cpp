#include "dictionarybrowser.h"
#include "globalvariables.h"
#include "strongsevent.h"
#include "parsingevent.h"
#include "eventmanager.h"
#include "biblequerier.h"
#include "basicwebhistoryitem.h"


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

    webHistory = new VWebHistory();

    connect(webHistory, SIGNAL(backwardAvailable(bool)), this, SIGNAL(backwardAvailable(bool)));
    connect(webHistory, SIGNAL(forwardAvailable(bool)), this, SIGNAL(forwardAvailable(bool)));

}

void DictionaryBrowser::javaScriptWindowObjectClearedSlot()
{
    page()->mainFrame()->addToJavaScriptWindowObject("javascriptClickListener", javascriptClickListener);
}

DictionaryBrowser::~DictionaryBrowser()
{
    delete javascriptClickListener;
    delete webHistory;
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

    BasicWebHistoryItem* item = new BasicWebHistoryItem(this, frameTop + transformToHtml(xml) + frameBottom);
    webHistory->display(item);
}

void DictionaryBrowser::backward()
{
    webHistory->backward();
}
void DictionaryBrowser::forward()
{
    webHistory->forward();
}

