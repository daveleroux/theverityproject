#include <QDebug>
#include "vlocationdropdowns.h"

vLocationDropDowns::vLocationDropDowns(QString activeText, QWidget *parent) :
    QWidget(parent)
{
    availableBooks << "Genesis" << "Exodus" << "Leviticus" << "Numbers" << "Deuteronomy" << "Joshua" << "Judges" << "Ruth" << "1 Samuel" << "2 Samuel" << "1 Kings" << "2 Kings" << "1 Chronicles" << "2 Chronicles" << "Ezra" << "Nehemiah" << "Esther" << "Job" << "Psalms" << "Proverbs" << "Ecclesiastes" << "Song of Songs" << "Isaiah" << "Jeremiah" << "Lamentations" << "Ezekiel" << "Daniel" << "Hosea" << "Joel" << "Amos" << "Obadiah" << "Jonah" << "Micah" << "Nahum" << "Habakkuk" << "Zephaniah" << "Haggai" << "Zechariah" << "Malachi";
    availableBooks << "Matthew" << "Mark" << "Luke" << "John" << "Acts" << "Romans" << "1 Corinthians" << "2 Corinthians" << "Galatians" << "Ephesians" << "Philippians" << "Colossians" << "1 Thessalonians" << "2 Thessalonians" << "1 Timothy" << "2 Timothy" << "Titus" << "Philemon" << "Hebrews" << "James" << "1 Peter" << "2 Peter" << "1 John" << "2 John" << "3 John" << "Jude" << "Revelation";

    //TODO: Get actual chapter range
    bookChapterRange = new QVector<int>;
    bookChapterRange->append(1);
    bookChapterRange->append(2);
    bookChapterRange->append(3);
    bookChapterRange->append(4);
    bookChapterRange->append(5);// BibleQuerier::getChaptersAvailable(activeText, &availableBooks.value(0));

    mainLayout = new QHBoxLayout(this);

    cmbBookSelector = new QComboBox(this);
    cmbBookSelector->addItems(availableBooks);
    mainLayout->addWidget(cmbBookSelector);

//    cmbChapterSelector.addItems((QStringList)bookChapterRange.toList());
    cmbChapterSelector = new QComboBox(this);
    cmbChapterSelector->addItem("1");
    mainLayout->addWidget(cmbChapterSelector);

    cmbVerseSelector = new QComboBox(this);
    cmbVerseSelector->addItem("1");
    mainLayout->addWidget(cmbVerseSelector);

    btnGo = new QToolButton();
    btnGo->setText("GO");
    mainLayout->addWidget(btnGo);

    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);
}
