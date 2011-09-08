#include "vlocationedit.h"
#include <QDebug>

vLocationEdit::vLocationEdit(QWidget *parent) :
    QWidget(parent)
{
    QStringList availableBooks;
    availableBooks << "Genesis" << "Exodus" << "Leviticus" << "Numbers" << "Deuteronomy" << "Joshua" << "Judges" << "Ruth" << "1 Samuel" << "2 Samuel" << "1 Kings" << "2 Kings" << "1 Chronicles" << "2 Chronicles" << "Ezra" << "Nehemiah" << "Esther" << "Job" << "Psalms" << "Proverbs" << "Ecclesiastes" << "Song of Songs" << "Isaiah" << "Jeremiah" << "Lamentations" << "Ezekiel" << "Daniel" << "Hosea" << "Joel" << "Amos" << "Obadiah" << "Jonah" << "Micah" << "Nahum" << "Habakkuk" << "Zephaniah" << "Haggai" << "Zechariah" << "Malachi";
    availableBooks << "Matthew" << "Mark" << "Luke" << "John" << "Acts" << "Romans" << "1 Corinthians" << "2 Corinthians" << "Galatians" << "Ephesians" << "Philippians" << "Colossians" << "1 Thessalonians" << "2 Thessalonians" << "1 Timothy" << "2 Timothy" << "Titus" << "Philemon" << "Hebrews" << "James" << "1 Peter" << "2 Peter" << "1 John" << "2 John" << "3 John" << "Jude" << "Revelation";

    stackedWidget = new QStackedWidget();

    QString act = "default";

    locationDropDowns = new vLocationDropDowns(act, this);
    stackedWidget->addWidget(locationDropDowns);

    locationLineEdit = new vLocationLineEdit(act, this);
    locationLineEdit->setFixedHeight(locationDropDowns->height());
    stackedWidget->addWidget(locationLineEdit);

    btnSwitchView.setText("#");
    btnSwitchView.setShortcut(Qt::CTRL + Qt::Key_L);
    btnSwitchView.setToolTip("Switch input method (Ctrl+l)");
    mainLayout.addWidget(stackedWidget);
    mainLayout.addWidget(&btnSwitchView);

    this->setLayout(&mainLayout);

    connect(&btnSwitchView, SIGNAL(clicked(bool)), this, SLOT(switchView(bool)));
    connect(locationLineEdit, SIGNAL(returnPressed()), this, SLOT(go()));
}

void vLocationEdit::switchView(bool toggle)
{
    stackedWidget->setCurrentIndex((stackedWidget->currentIndex() * (-1)) + 1);
    stackedWidget->currentWidget()->setFocus(Qt::OtherFocusReason);
}

void vLocationEdit::go()
{

    qDebug() << "HAMBA: " << stackedWidget->currentWidget()->property("destination");
    locationLineEdit->setText(stackedWidget->currentWidget()->property("destination").toString());
    locationLineEdit->hideOptions();

//    emit somekindofsignaltogo(stackedWidget->currentWidget()->property("destination"));
}
