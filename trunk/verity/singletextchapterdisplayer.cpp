#include "singletextchapterdisplayer.h"
#include "biblequerier.h"

SingleTextChapterDisplayer::SingleTextChapterDisplayer(QTextBrowser* textBrowser, QList<QString> texts, QMap<QString, QString> fontFamilies) :
        ChapterDisplayer(textBrowser, texts, fontFamilies)
{
}

QString SingleTextChapterDisplayer::getText()
{
    return getPrimaryText();
}


ChapterRepresentation* SingleTextChapterDisplayer::constructChapterRepresentation(int normalisedChapter, int idLocation)
{
    bool hebrew = false;
    if(getText() == "wlc")
        hebrew = true;


    QList<TextInfo> textInfos = BibleQuerier::readInChapterData(getText(), normalisedChapter);
    QMap<BaseTextUnit, TextInfo> textUnits;

    QTextDocument document;
    QTextCursor textCursor(&document);
    textCursor.beginEditBlock();

    QTextBlockFormat textBlockFormat;
    if(hebrew)
    {
        textBlockFormat.setLayoutDirection(Qt::RightToLeft);
        textBlockFormat.setRightMargin(10);
    }
    else
    {
        textBlockFormat.setAlignment(Qt::AlignLeft);
    }

    textCursor.setBlockFormat(textBlockFormat);

    QTextCharFormat superscriptFormat = getSuperscriptFormat();

    QTextCharFormat defaultFormat = getDefaultFormat(getText());

    QTextCharFormat boldFormat = getBoldFormat();

    textCursor.insertBlock();

    int selectionStart = -1;
    int selectionEnd = -1;


    //    textCursor.setCharFormat(defaultFormat);
    for(int i=0; i<textInfos.size(); i++)
    {
        TextInfo textInfo = textInfos.at(i);

        if(textInfo.paragraph)
        {
            textCursor.insertText(QString(QChar::ParagraphSeparator));
            textCursor.insertText(QString(QChar::ParagraphSeparator));
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

    textCursor.insertText(QString(QChar::ParagraphSeparator));

    textCursor.endEditBlock();

    if(selectionStart != -1 && selectionEnd == -1)
        selectionEnd = textCursor.position()-2;

    return new ChapterRepresentation(getText(), normalisedChapter,QTextDocumentFragment(&document), textUnits, selectionStart, selectionEnd);

}
