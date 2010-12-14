#include "paralleltextchapterdisplayer.h"
#include "biblequerier.h"
#include <QTextTable>
#include <QSet>
#include <QDebug>

ParallelTextChapterDisplayer::ParallelTextChapterDisplayer(QTextBrowser* textBrowser, int idLocation, int normalisedChapterLocation, QList<QString> texts) :
        ChapterDisplayer(textBrowser, idLocation, normalisedChapterLocation, texts)
{
}

ChapterRepresentation ParallelTextChapterDisplayer::constructChapterRepresentation(int normalisedChapter, int idLocation)
{
    QList<TextInfo> textInfos = BibleQuerier::readInChapterData(getPrimaryText(), normalisedChapter);
    QMap<BaseTextUnit, TextInfo> textUnits;

    QTextDocument document;
    QTextCursor textCursor(&document);
    textCursor.beginEditBlock();

//    QTextBlockFormat textBlockFormat;
//    if(hebrew)
//    {
//        textBlockFormat.setLayoutDirection(Qt::RightToLeft);
//        textBlockFormat.setRightMargin(10);
//    }
//    else
//    {
//        textBlockFormat.setAlignment(Qt::AlignLeft);
//    }

//    textCursor.setBlockFormat(textBlockFormat);

    QTextCharFormat superscriptFormat;
    superscriptFormat.setForeground(QBrush(Qt::red));
    superscriptFormat.setVerticalAlignment(QTextCharFormat::AlignSuperScript);

    QTextCharFormat defaultFormat;

    QTextCharFormat boldFormat;
    boldFormat.setFontWeight(QFont::Bold);

    QSet<int> verseSet;
    for(int i=0; i<textInfos.size(); i++)
    {
        verseSet.insert(textInfos.at(i).verse);
    }

    QVector<QTextLength> columnWidths;
    qreal percentage = 100/(qreal)texts.size();
    for(int i=0; i<texts.size(); i++)
        columnWidths.append(QTextLength(QTextLength::PercentageLength, percentage));
    QTextTableFormat tableFormat;
    tableFormat.setColumnWidthConstraints(columnWidths);

    QTextTable* table = textCursor.insertTable(verseSet.size(), texts.size(), tableFormat);

    int selectionStart = -1;
    int selectionEnd = -1;

    int currentVerse = 0;

    for(int i=0; i<textInfos.size(); i++)
    {
        TextInfo textInfo = textInfos.at(i);
        if(textInfo.numberInVerse == 1)
        {
            textCursor.setPosition(table->cellAt(currentVerse,0).firstCursorPosition().position());
            currentVerse++;
        }
        if(textInfo.chapter == 1 && textInfo.verse ==1 && textInfo.numberInVerse == 1)
        {
            textCursor.insertText(textInfo.book, boldFormat);
            textCursor.insertText(QString(QChar::ParagraphSeparator));
            textCursor.setCharFormat(defaultFormat);
        }
        if(textInfo.verse == 1 && textInfo.numberInVerse == 1)
        {
            textCursor.insertText("Chapter "+QString().setNum(textInfo.chapter), boldFormat);
            textCursor.insertText(QString(QChar::ParagraphSeparator));
            textCursor.setCharFormat(defaultFormat);
        }
        if(textInfo.numberInVerse == 1)
        {
            if(selectionStart != -1 && selectionEnd == -1)
                selectionEnd = textCursor.position();
            textCursor.insertText(QString().setNum(textInfo.verse), superscriptFormat);
            textCursor.setCharFormat(defaultFormat);
        }

        int start = textCursor.position();

        if(textInfo.id == idLocation)
        {
            selectionStart = start;
        }

        textCursor.insertText(textInfo.text);
        int end = textCursor.position();

        BaseTextUnit textUnit(start, end);
        textUnits.insert(textUnit, textInfo);

        textCursor.insertText(" ");
    }

    textCursor.endEditBlock();

    if(selectionStart != -1 && selectionEnd == -1)
        selectionEnd = textCursor.position() + 1;

    selectionEnd = selectionEnd - 2;
    return ChapterRepresentation(getPrimaryText(), normalisedChapter,QTextDocumentFragment(&document), textUnits, selectionStart, selectionEnd);
}

