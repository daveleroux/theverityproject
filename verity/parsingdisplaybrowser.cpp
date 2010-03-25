#include "parsingdisplaybrowser.h"
#include "parsingdecoder.h"
#include <QTextTable>
#include <QDebug>
#include <QtSql>

ParsingDisplayBrowser::ParsingDisplayBrowser()
{
}

ParsingDisplayBrowser::ParsingDisplayBrowser(QWidget* parent) : QTextBrowser(parent)
{
}

void ParsingDisplayBrowser::display(TextInfo textInfo, bool showDefinition)
{
    clear();

    QList<ParseAttribute> attributes = ParsingDecoder::parse(textInfo.normalisedMorphTag);

    QTextCursor textCursor(document());

    QTextCharFormat boldFormat;
    boldFormat.setFontWeight(QFont::Bold);

    QTextCharFormat normalFormat;

    textCursor.insertText("Parsing\n", boldFormat);

    QTextTableFormat tableFormat;
    tableFormat.setCellPadding(1);
    tableFormat.setBorder(0);


    QTextTable* textTable = textCursor.insertTable(attributes.size(), 2, tableFormat);


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

    if(showDefinition && attributes.size() > 0)
    {
        textCursor.insertText("\n\n", boldFormat);

        if(textInfo.strongsLemma == textInfo.fribergLemma)
            textCursor.insertText(textInfo.strongsLemma + "\n\n", boldFormat);
        else
            textCursor.insertText(textInfo.strongsLemma + "/" + textInfo.fribergLemma + "\n\n", boldFormat);

        QSqlQuery query;
        if(!query.exec("select strongs_def, kjv_def, strongs_derivation, strongs_other from strongs where strongs_number = " + QString().setNum(textInfo.strongsNumber)))
         {
             qDebug() << "failed: " << query.lastError() << endl;
             exit(1);
         }

        if(query.next())
        {
                QString strongsDef = query.value(0).toString();
                QString kjvDef = query.value(1).toString();
                QString strongsDerivation = query.value(2).toString();
                QString strongsOther = query.value(3).toString();

                textCursor.insertText("Strong's Definition\n", boldFormat);
                textCursor.insertText(strongsDef.trimmed() + "\n\n", normalFormat);

                textCursor.insertText("KJV Definition\n", boldFormat);
                textCursor.insertText(kjvDef.trimmed() + "\n\n", normalFormat);

                textCursor.insertText("Strong's Derivation\n", boldFormat);
                textCursor.insertText(strongsDerivation.trimmed() + "\n\n", normalFormat);


                textCursor.insertText("Strong's Other\n", boldFormat);
                textCursor.insertText(strongsOther.trimmed() + "\n\n", normalFormat);

        }
    }
}
