#include "vlocationedit.h"
#include <QDebug>
#include "versereferenceparser.h"
#include "biblereferenceevent.h"
#include <QMenu>
#include <QShortcut>

VLocationEdit::VLocationEdit(QVector<QString> texts, QWidget *parent) :
    QWidget(parent)
{
    QStringList availableBooks;
    availableBooks << "Genesis" << "Exodus" << "Leviticus" << "Numbers" << "Deuteronomy" << "Joshua" << "Judges" << "Ruth" << "1 Samuel" << "2 Samuel" << "1 Kings" << "2 Kings" << "1 Chronicles" << "2 Chronicles" << "Ezra" << "Nehemiah" << "Esther" << "Job" << "Psalms" << "Proverbs" << "Ecclesiastes" << "Song of Songs" << "Isaiah" << "Jeremiah" << "Lamentations" << "Ezekiel" << "Daniel" << "Hosea" << "Joel" << "Amos" << "Obadiah" << "Jonah" << "Micah" << "Nahum" << "Habakkuk" << "Zephaniah" << "Haggai" << "Zechariah" << "Malachi";
    availableBooks << "Matthew" << "Mark" << "Luke" << "John" << "Acts" << "Romans" << "1 Corinthians" << "2 Corinthians" << "Galatians" << "Ephesians" << "Philippians" << "Colossians" << "1 Thessalonians" << "2 Thessalonians" << "1 Timothy" << "2 Timothy" << "Titus" << "Philemon" << "Hebrews" << "James" << "1 Peter" << "2 Peter" << "1 John" << "2 John" << "3 John" << "Jude" << "Revelation";

//    QVector<int> *bookChapterRange = new QVector<int>;

    mainLayout = new QHBoxLayout(this);
    /*btnTextAndViewSelect.setText("Parallel View");
    btnTextAndViewSelect.setPopupMode(QToolButton::MenuButtonPopup);
    QMenu *menu = new QMenu();
    foreach (QString text, texts)
    {
        menu->addAction(text);
    }
    foreach (QAction *action, menu->actions())
    {
        action->setCheckable(true);
        action->setChecked(true);
    }
    btnTextAndViewSelect.setMenu(menu);
    mainLayout->addWidget(&btnTextAndViewSelect);*/

    stackedWidget = new QStackedWidget(this);

    //TODO: Get actual active text to pass to the LocationEditors (they need to get available books somehow)
    QString act = "default";

    locationDropDowns = new VLocationDropDowns(act, this);
    stackedWidget->addWidget(locationDropDowns);

    locationLineEdit = new VLocationLineEdit(act, this);
    stackedWidget->addWidget(locationLineEdit);

    btnSwitchView.setText("#");
    btnSwitchView.setToolTip("Switch input method");

    backButton = new QToolButton();
    backButton->setArrowType(Qt::LeftArrow);
    backButton->setEnabled(false);

    forwardButton = new QToolButton();
    forwardButton->setArrowType(Qt::RightArrow);
    forwardButton->setEnabled(false);

    mainLayout->addWidget(backButton);
    mainLayout->addWidget(forwardButton);

    mainLayout->addWidget(stackedWidget);
    mainLayout->addWidget(&btnSwitchView);

    this->setLayout(mainLayout);
    qDebug() << this->height();

    connect(&btnSwitchView, SIGNAL(clicked(bool)), this, SLOT(switchStackedWidget(bool)));
    connect(&btnTextAndViewSelect, SIGNAL(clicked(bool)), this, SLOT(switchParallelView()));

    connect(locationLineEdit, SIGNAL(returnPressed()), this, SLOT(go()));
    connect(locationDropDowns, SIGNAL(goSignal()), this, SLOT(go()));

    QShortcut *shortcut = new QShortcut(Qt::CTRL + Qt::Key_L, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(shortcutActivated()));
}

void VLocationEdit::switchStackedWidget(bool toggle)
{
    stackedWidget->setCurrentIndex((stackedWidget->currentIndex() * (-1)) + 1);
    stackedWidget->currentWidget()->setFocus(Qt::OtherFocusReason);
}

void VLocationEdit::switchParallelView()
{
    switch (btnTextAndViewSelect.text() == "Parallel View")
    {
    case true:
        btnTextAndViewSelect.setText("Single Text View");
        break;
    case false:
        btnTextAndViewSelect.setText("Parallel View");
        break;
    }
//    emit
}

void VLocationEdit::go()
{
    VerseReference verseReference = VerseReferenceParser::parse(stackedWidget->currentWidget()->property("destination").toString());
    locationLineEdit->setText(verseReference.stringRepresentation);
    locationLineEdit->hideOptions();

    locationDropDowns->setLocation(verseReference);
    stackedWidget->setCurrentWidget(locationDropDowns);

    (new BibleReferenceEvent(verseReference))->fire();
}

void VLocationEdit::shortcutActivated()
{
    stackedWidget->setCurrentWidget(locationLineEdit);
    locationLineEdit->selectAll();
    locationLineEdit->setFocus();
}
