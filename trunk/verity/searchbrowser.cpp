#include "searchbrowser.h"
#include "biblequerier.h"
#include "versereferenceparser.h"

SearchBrowser::SearchBrowser(QWidget* parent) : QTextBrowser(parent)
{
    zoomIn(2);
    setOpenLinks(false);
    connect(this, SIGNAL(anchorClicked(QUrl)), this, SLOT(anchorClickedSlot(QUrl)));
}

void SearchBrowser::performSearch(QString searchTerms)
{
    QString print = "<html>";
    QStringList results = BibleQuerier::search(searchTerms);

    print.append("<ul>");
    for (int i = 0; i < results.count(); i++)
    {
        print.append("<li>" + results.at(i) + "</li>");
    }
    print.append("</ul>");
    print.append("</html>");

    this->setHtml(print);
}


void SearchBrowser::anchorClickedSlot(QUrl url)
{
//    QList<QString> tmp;
    QString urlString = url.toString().mid(9);
    QStringList refPieces = urlString.split(".");
    int book = ((QString)(refPieces.at(0))).toInt();
    int chapter = ((QString)(refPieces.at(1))).toInt();
    int verse = ((QString)(refPieces.at(2))).toInt();
    VerseReference verseReference(book, chapter, verse, VerseReferenceParser::calculateStringRepresentation(book, chapter, verse) );
//    VerseReference verseReference = VerseReferenceParser::parse(urlString.mid(9));

    emit goToResult(this->textsAvailable, verseReference);
    //figure out how to cancel loading of resource
    //    return urlString.mid(9);
//    return this->toHtml();
}

void SearchBrowser::setTextsAvaiable(QList<QString> txts)
{
    this->textsAvailable = txts;
}
