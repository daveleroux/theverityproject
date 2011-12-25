#include "parsingdisplaybrowser.h"
#include "parsingdecoder.h"
#include <QTextTable>
#include <QDebug>
#include <QtSql>
#include "parsingevent.h"
#include "eventmanager.h"

ParsingDisplayBrowser::ParsingDisplayBrowser(QWidget* parent) : VWebView(parent)
{
    EventManager::addListener(EventType::PARSING, this);

    //should share this with bibletextbrowser?

    frameTop = "<html>"
               "<head>"
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

void ParsingDisplayBrowser::handleEvent(Event* event)
{
    if(event->getEventType() == EventType::PARSING)
    {
        ParsingEvent* parsingEvent = static_cast<ParsingEvent*>(event);
        display(parsingEvent->normalisedMorphTag);
    }
    else
    {
        VWebView::handleEvent(event);
    }

}

QSize ParsingDisplayBrowser::sizeHint() const
{
    return QSize(width(), 200);
}

void ParsingDisplayBrowser::display(QBitArray normalisedMorphTag)
{
//    clear();

    QList<ParseAttribute> attributes = ParsingDecoder::parse(normalisedMorphTag);

//    QTextCursor textCursor(document());

//    QTextCharFormat boldFormat;
//    boldFormat.setFontWeight(QFont::Bold);

//    QTextCharFormat normalFormat;

//    textCursor.insertHtml("<h3>Parsing</h3>");
//    textCursor.insertText("\n");


    QString table = "<table class=\"parsing\">";

//    QTextTableFormat tableFormat;
//    tableFormat.setCellPadding(1);
//    tableFormat.setBorder(0);


//    QTextTable* textTable = textCursor.insertTable(attributes.size() + 1, 2, tableFormat);


//    if(attributes.size() > 0)
//    {
//        textTable->cellAt(0,1).setFormat(boldFormat);
//    }

    for(int i=0; i<attributes.size(); i++)
    {
        table += "<tr>";

        table += "<td>" + attributes.at(i).description + ": </td>";

        if(i==0)
            table += "<td><b>" + attributes.at(i).value + "</b></td>";
        else
            table += "<td>" + attributes.at(i).value + "</td>";

        table += "</tr>";
    }

    table += "</table>";

//    textCursor.insertText("Lemma:");
//    textCursor.movePosition(QTextCursor::NextBlock);

//    if(textInfo->strongsLemma == textInfo->fribergLemma)
//        textCursor.insertText(textInfo->strongsLemma, boldFormat);
//    else
//        textCursor.insertText(textInfo->strongsLemma + "/" + textInfo->fribergLemma, boldFormat);

    setHtml(frameTop + table + frameBottom);

}

//QMimeData* ParsingDisplayBrowser::createMimeDataFromSelection() const
//{
//    QMimeData* mimeData = new QMimeData();
//    mimeData->setText(textCursor().selectedText());
//    return mimeData;
//}

