#include "paralleltextchapterdisplayer.h"
#include "biblequerier.h"
#include <QTextTable>
#include <QSet>
#include <QDebug>
#include "parallelchapterrepresentation.h"
#include "parallelgridconstructor.h"

ParallelTextChapterDisplayer::ParallelTextChapterDisplayer(QTextBrowser* textBrowser, QList<QString> texts) :
        ChapterDisplayer(textBrowser, texts)
{
}

QSet<int> ParallelTextChapterDisplayer::extractParallelIds(QList<TextInfo> textInfos)
{
    QSet<int> result;
    for(int i=0; i<textInfos.size(); i++)
    {
        TextInfo textInfo = textInfos.at(i);
        result.insert(textInfo.parallel);
    }
    return result;
}

ChapterRepresentation* ParallelTextChapterDisplayer::constructChapterRepresentation(int normalisedChapter, int idLocation)
{
    bool prepending = false;
    bool appending = false;

    if(chapters.size() > 0)
    {
        int firstNormalisedChapter = getFirstNormChapter();
        if(normalisedChapter < firstNormalisedChapter)
        {
            prepending = true;
        }
        else
        {
            appending = true;
        }
    }


    QList<TextInfo> primaryTextTextInfos = BibleQuerier::readInChapterData(getPrimaryText(), normalisedChapter);

    QList<VerseNode*> chainHeads;
    chainHeads.append(ParallelGridConstructor::constructChain(primaryTextTextInfos));


    QSet<int> parallelIds = extractParallelIds(primaryTextTextInfos);

    QMap<QString, int> firstIdsMap;
    QMap<QString, int> lastIdsMap;

    for(int i=1; i<texts.size(); i++)
    {
        int idToInclude = -1;
        if(appending)
        {
            ParallelChapterRepresentation* parallelChRep = (ParallelChapterRepresentation*)chapters.value(getLastNormChapter());
            idToInclude = parallelChRep->getLastIdForText(texts.at(i));
            if(idToInclude != -1)
                idToInclude++;
        }
        else if(prepending)
        {
            ParallelChapterRepresentation* parallelChRep = (ParallelChapterRepresentation*)chapters.value(getFirstNormChapter());
            idToInclude = parallelChRep->getFirstIdForText(texts.at(i));
            if(idToInclude != -1)
                idToInclude--;
        }

        QList<TextInfo> textInfos = BibleQuerier::readInChapterDataForParallel(texts.at(i), parallelIds, idToInclude);

        if(textInfos.size() > 0)
        {
            firstIdsMap.insert(texts.at(i), textInfos.at(0).id);
            lastIdsMap.insert(texts.at(i), textInfos.at(textInfos.size()-1).id);
        }
        else
        {
            firstIdsMap.insert(texts.at(i), -1);
            lastIdsMap.insert(texts.at(i), -1);
        }
        chainHeads.append(ParallelGridConstructor::constructChain(textInfos));

        parallelIds = parallelIds.unite(extractParallelIds(textInfos));
    }

    VerseNode* grid = ParallelGridConstructor::constructGrid(chainHeads);

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

    QFont defaultFont = document.defaultFont();
    QFont smallFont = defaultFont;
    smallFont.setPointSize(6);


    QTextCharFormat superscriptFormat;
    superscriptFormat.setForeground(QBrush(Qt::red));
    superscriptFormat.setVerticalAlignment(QTextCharFormat::AlignSuperScript);

    QTextCharFormat defaultFormat;

    QTextCharFormat boldFormat;
    boldFormat.setFontWeight(QFont::Bold);


    QVector<QTextLength> columnWidths;
    qreal percentage = 100/(qreal)texts.size();
    for(int i=0; i<texts.size(); i++)
        columnWidths.append(QTextLength(QTextLength::PercentageLength, percentage));
    QTextTableFormat tableFormat;
    tableFormat.setColumnWidthConstraints(columnWidths);

    int rows = 0;

    VerseNode* node = grid->down;
    while(node != 0)
    {
        rows++;
        node = node->down;
    }

    QTextTable* table = textCursor.insertTable(rows, texts.size(), tableFormat);

    int selectionStart = -1;
    int selectionEnd = -1;

    int currentRow = 0;

    VerseNode* headingDownNode = grid->down;
    while(headingDownNode != 0)
    {
        int currentColumn = 0;
        VerseNode* cell = headingDownNode;

        while(cell != 0)
        {
            if(selectionStart != -1 && selectionEnd == -1)
                selectionEnd = textCursor.position()+1;

            textCursor.setPosition(table->cellAt(currentRow,currentColumn).firstCursorPosition().position());

            for(int i=0; i<cell->textInfos.size(); i++)
            {
                TextInfo textInfo = cell->textInfos.at(i);

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

                    textCursor.insertText(QString().setNum(textInfo.verse), superscriptFormat);

                    textCursor.setCharFormat(defaultFormat);
                }

                int start = textCursor.position();

                if(textInfo.id == idLocation && currentColumn == 0)
                {
                    selectionStart = start;
                }

                textCursor.insertText(textInfo.text);
                int end = textCursor.position();

                BaseTextUnit textUnit(start, end);
                textUnits.insert(textUnit, textInfo);

                textCursor.insertText(" ");
            }
            currentColumn++;
            cell = cell->right;
        }

        currentRow++;
        headingDownNode = headingDownNode->down;
    }

    textCursor.endEditBlock();

    if(selectionStart != -1 && selectionEnd == -1)
        selectionEnd = textCursor.position() + 1;

    selectionEnd = selectionEnd - 2;

    return new ParallelChapterRepresentation(getPrimaryText(),
                                             normalisedChapter,
                                             QTextDocumentFragment(&document),
                                             textUnits,
                                             selectionStart,
                                             selectionEnd,
                                             firstIdsMap,
                                             lastIdsMap);
}

