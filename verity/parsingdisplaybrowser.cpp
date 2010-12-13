#include "parsingdisplaybrowser.h"
#include "parsingdecoder.h"
#include <QTextTable>
#include <QDebug>
#include <QtSql>

ParsingDisplayBrowser::ParsingDisplayBrowser(QWidget* parent) : QTextBrowser(parent)
{
}

void ParsingDisplayBrowser::display(TextAndTextInfo* textAndTextInfo)
{
    clear();

    QList<ParseAttribute> attributes = ParsingDecoder::parse(textAndTextInfo->textInfo.normalisedMorphTag);

    QTextCursor textCursor(document());

    QTextCharFormat boldFormat;
    boldFormat.setFontWeight(QFont::Bold);

    QTextCharFormat normalFormat;

    textCursor.insertHtml("<h3>Parsing</h3>");
    textCursor.insertText("\n");


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

    textCursor.insertText("Lemma:");
    textCursor.movePosition(QTextCursor::NextBlock);

    if(textAndTextInfo->textInfo.strongsLemma == textAndTextInfo->textInfo.fribergLemma)
        textCursor.insertText(textAndTextInfo->textInfo.strongsLemma, boldFormat);
    else
        textCursor.insertText(textAndTextInfo->textInfo.strongsLemma + "/" + textAndTextInfo->textInfo.fribergLemma, boldFormat);

}

QMimeData* ParsingDisplayBrowser::createMimeDataFromSelection() const
{
    QMimeData* mimeData = new QMimeData();
    mimeData->setText(textCursor().selectedText());
    return mimeData;
}

