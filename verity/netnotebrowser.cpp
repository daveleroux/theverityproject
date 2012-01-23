#include "netnotebrowser.h"
#include "eventmanager.h"
#include "netnoteevent.h"
#include "biblequerier.h"
#include <QFile>

NetNoteBrowser::NetNoteBrowser(QWidget *parent) : VWebView(parent)
{
    EventManager::addListener(EventType::NET_NOTE, this);

    //should share this with bibletextbrowser?

    frameTop = "<html>"
               "<head>"
               "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
               "<style type=\"text/css\">\n";

    QFile tmp(DATA_PATH + "/bible.css");
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

//QSizePolicy NetNoteBrowser::sizePolicy()
//{
//    return QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
//}

QSize NetNoteBrowser::sizeHint() const
{
    return QSize(width(), 200);
}

QString NetNoteBrowser::transformToHtml(QString xml)
{
    QMap<QString, QString> replaceMap;

    replaceMap.insert("smallCaps", "smallCaps");
    replaceMap.insert("translatorsNote", "translatorsNote");
    replaceMap.insert("studyNote", "studyNote");
    replaceMap.insert("textCriticalNote", "textCriticalNote");
    replaceMap.insert("mapNote", "mapNote");
    replaceMap.insert("b", "b");
    replaceMap.insert("i", "i");
    replaceMap.insert("sup", "sup");
    replaceMap.insert("hebrew", "hebrew");
    replaceMap.insert("greek", "greek");
    replaceMap.insert("note", "note");
    replaceMap.insert("heading", "heading");


    for(int i=0; i<replaceMap.keys().size(); i++)
    {
        QString key = replaceMap.keys().at(i);
        QString value = replaceMap.value(key);

        xml.replace("<"+key+">", "<span class=\""+value+"\">");
        xml.replace("</"+key+">", "</span>");
    }

    //    xml.replace(QRegExp("<word bibleTextId=\"([0-9]*)\" wordId=\"([0-9]*)\">([^<]*)</word>"), "<span onclick=\"javascriptClickListener.wordClicked(\\1,\\2)\">\\3</span>");
    //    xml.replace(QRegExp("<netNote id=\"([0-9]*)\">([^<]*)</netNote>"), "<span class=\"netNote\" onclick=\"javascriptClickListener.netNoteClicked(\\1)\">\\2</span>");

    return frameTop + xml + frameBottom;
}


void NetNoteBrowser::handleEvent(Event* event)
{
    if(event->getEventType() == EventType::NET_NOTE)
    {
        NetNoteEvent* netNoteEvent = static_cast<NetNoteEvent*>(event);
        QString xml = BibleQuerier::getNetNote(netNoteEvent->id);
        setHtml(transformToHtml(xml));
        parentWidget()->raise();
    }
    else
    {
        VWebView::handleEvent(event);
    }
}
