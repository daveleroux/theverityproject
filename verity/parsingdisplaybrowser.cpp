#include "parsingdisplaybrowser.h"
#include "parsingdecoder.h"
#include <QTextTable>
#include <QDebug>
#include <QtSql>
#include "parsingevent.h"
#include "eventmanager.h"

ParsingDisplayBrowser::ParsingDisplayBrowser(QWidget* parent) : QTextBrowser(parent), Listener()
{
    EventManager::addListener(EventType::PARSING, this);
    zoomIn(2);
}

void ParsingDisplayBrowser::handleEvent(Event* event)
{
    ParsingEvent* parsingEvent = static_cast<ParsingEvent*>(event);
    display(parsingEvent->normalisedMorphTag);
}

void ParsingDisplayBrowser::display(QBitArray normalisedMorphTag)
{
    clear();

    QList<ParseAttribute> attributes = ParsingDecoder::parse(normalisedMorphTag);

    QTextCursor textCursor(document());

    QTextCharFormat boldFormat;
    boldFormat.setFontWeight(QFont::Bold);

    QTextCharFormat normalFormat;

//    textCursor.insertHtml("<h3>Parsing</h3>");
//    textCursor.insertText("\n");


    QTextTableFormat tableFormat;
    tableFormat.setCellPadding(1);
    tableFormat.setBorder(0);


    QTextTable* textTable = textCursor.insertTable(attributes.size() + 1, 2, tableFormat);


    if(attributes.size() > 0)
    {
        textTable->cellAt(0,1).setFormat(boldFormat);
    }

    for(int i=0; i<attributes.size(); i++)
    {
        textCursor.insertText(attributes.at(i).description + ":");
        textCursor.movePosition(QTextCursor::NextBlock);
        textCursor.insertText(attributes.at(i).value);
        textCursor.movePosition(QTextCursor::NextBlock);
    }

//    textCursor.insertText("Lemma:");
//    textCursor.movePosition(QTextCursor::NextBlock);

//    if(textInfo->strongsLemma == textInfo->fribergLemma)
//        textCursor.insertText(textInfo->strongsLemma, boldFormat);
//    else
//        textCursor.insertText(textInfo->strongsLemma + "/" + textInfo->fribergLemma, boldFormat);

}

QMimeData* ParsingDisplayBrowser::createMimeDataFromSelection() const
{
    QMimeData* mimeData = new QMimeData();
    mimeData->setText(textCursor().selectedText());
    return mimeData;
}

