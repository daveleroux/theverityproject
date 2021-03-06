#include <QDebug>
#include "vlocationdropdowns.h"
#include "versereferenceparser.h"
#include "biblequerier.h"

VLocationDropDowns::VLocationDropDowns(QString activeText, QWidget *parent) :
    QWidget(parent, Qt::Widget)
{
    //TODO: update the available books based on the active text
    //Ideally this should be in the form of some kind of struct or something that would contain the text's name (e.g. "Matthew" in NET but "<grk for matthew>" in tisch) and an id that is associated in the text (of the db) with that book
    //Consider implications of text specific names for the locationlineedit though...
    availableBooks << "Genesis" << "Exodus" << "Leviticus" << "Numbers" << "Deuteronomy" << "Joshua" << "Judges" << "Ruth" << "1 Samuel" << "2 Samuel" << "1 Kings" << "2 Kings" << "1 Chronicles" << "2 Chronicles" << "Ezra" << "Nehemiah" << "Esther" << "Job" << "Psalms" << "Proverbs" << "Ecclesiastes" << "Song of Songs" << "Isaiah" << "Jeremiah" << "Lamentations" << "Ezekiel" << "Daniel" << "Hosea" << "Joel" << "Amos" << "Obadiah" << "Jonah" << "Micah" << "Nahum" << "Habakkuk" << "Zephaniah" << "Haggai" << "Zechariah" << "Malachi";
    availableBooks << "Matthew" << "Mark" << "Luke" << "John" << "Acts" << "Romans" << "1 Corinthians" << "2 Corinthians" << "Galatians" << "Ephesians" << "Philippians" << "Colossians" << "1 Thessalonians" << "2 Thessalonians" << "1 Timothy" << "2 Timothy" << "Titus" << "Philemon" << "Hebrews" << "James" << "1 Peter" << "2 Peter" << "1 John" << "2 John" << "3 John" << "Jude" << "Revelation";

    mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);

    cmbBookSelector = new QComboBox(this);
    cmbBookSelector->addItems(availableBooks);
    cmbBookSelector->setMaxVisibleItems(39);
    mainLayout->addWidget(cmbBookSelector);

    cmbChapterSelector = new QComboBox(this);
    cmbChapterSelector->addItems(BibleQuerier::getChapterRange(1, VerseReferenceParser::parse("Genesis 1:1")));
    mainLayout->addWidget(cmbChapterSelector);

    cmbVerseSelector = new QComboBox(this);
    cmbVerseSelector->addItems(BibleQuerier::getVerseRange(1, VerseReferenceParser::parse("Genesis 1:1")));
    mainLayout->addWidget(cmbVerseSelector);

    btnGo = new QToolButton(this);
    btnGo->setText("GO");
    mainLayout->addWidget(btnGo);
    connect (btnGo, SIGNAL(clicked()), this, SLOT(goClicked()));
    connect (cmbBookSelector, SIGNAL(currentIndexChanged(QString)), this, SLOT(bookSelected(QString)));
    connect (cmbChapterSelector, SIGNAL(currentIndexChanged(QString)), this, SLOT(chapterSelected(QString)));

    this->setLayout(mainLayout);
}
VLocationDropDowns::~VLocationDropDowns()
{
    delete mainLayout;
    delete cmbBookSelector;
    delete cmbChapterSelector;
    delete cmbVerseSelector;
    delete btnGo;
}

void VLocationDropDowns::goClicked()
{
    setProperty("destination", cmbBookSelector->currentText() + " " + cmbChapterSelector->currentText() + ":" + cmbVerseSelector->currentText());
    emit goSignal();
}

void VLocationDropDowns::setLocation(VerseReference reference)
{
    cmbBookSelector->setCurrentIndex(cmbBookSelector->findText(VerseReferenceParser::booknameFromBookindex(reference.book)));
    if (cmbChapterSelector->count() >= reference.chapter)
    {
        cmbChapterSelector->setCurrentIndex(cmbChapterSelector->findText(QString::number(reference.chapter)));
    }
    else
    {
        cmbChapterSelector->setCurrentIndex(0);
    }

    if (cmbVerseSelector->count() >= reference.verse)
    {
        cmbVerseSelector->setCurrentIndex(cmbVerseSelector->findText(QString::number(reference.verse)));
    }
    else
    {
        cmbVerseSelector->setCurrentIndex(0);
    }
}

void VLocationDropDowns::bookSelected(QString text)
{
    cmbChapterSelector->clear();
    cmbChapterSelector->addItems(BibleQuerier::getChapterRange(1, VerseReferenceParser::parse(text + " 1:1")));
}
void VLocationDropDowns::chapterSelected(QString text)
{
    cmbVerseSelector->clear();
    cmbVerseSelector->addItems(BibleQuerier::getVerseRange(1, VerseReferenceParser::parse(cmbBookSelector->currentText()+ " "+text+":1")));
}
