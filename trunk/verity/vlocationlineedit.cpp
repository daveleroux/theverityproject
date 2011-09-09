#include "vlocationlineedit.h"
#include <QKeyEvent>
#include <QtGui/QListView>
#include <QtGui/QStringListModel>
#include <QDebug>

vLocationLineEdit::vLocationLineEdit(QString activeText, QWidget *parent)
    : QLineEdit(parent)
{
    listView = new QListView(this);
    listView->setWindowFlags(Qt::ToolTip);

    connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(setCompleter(const QString &)));
    connect(listView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(completeText(const QModelIndex &)));

    words = new QStringList();
    *words << "Genesis" << "Exodus" << "Leviticus" << "Numbers" << "Deuteronomy" << "Joshua" << "Judges" << "Ruth" << "1 Samuel" << "2 Samuel" << "1 Kings" << "2 Kings" << "1 Chronicles" << "2 Chronicles" << "Ezra" << "Nehemiah" << "Esther" << "Job" << "Psalms" << "Proverbs" << "Ecclesiastes" << "Song of Songs" << "Isaiah" << "Jeremiah" << "Lamentations" << "Ezekiel" << "Daniel" << "Hosea" << "Joel" << "Amos" << "Obadiah" << "Jonah" << "Micah" << "Nahum" << "Habakkuk" << "Zephaniah" << "Haggai" << "Zechariah" << "Malachi";
    *words << "Matthew" << "Mark" << "Luke" << "John" << "Acts" << "Romans" << "1 Corinthians" << "2 Corinthians" << "Galatians" << "Ephesians" << "Philippians" << "Colossians" << "1 Thessalonians" << "2 Thessalonians" << "1 Timothy" << "2 Timothy" << "Titus" << "Philemon" << "Hebrews" << "James" << "1 Peter" << "2 Peter" << "1 John" << "2 John" << "3 John" << "Jude" << "Revelation";

    hash = getBookHash();

    this->maxVisibleRows = 15;

    model = new QStringListModel(this);
    listView->setModel(model);
}

void vLocationLineEdit::focusOutEvent(QFocusEvent *e)
{
    listView->hide();
    QLineEdit::focusOutEvent(e);
}
void vLocationLineEdit::keyPressEvent(QKeyEvent *e)
{
    int key = e->key();
    if (!listView->isHidden())
    {
        int count = listView->model()->rowCount();
        QModelIndex currentIndex = listView->currentIndex();

        if (key == Qt::Key_Down || key == Qt::Key_Up)
        {
            int row = currentIndex.row();
            switch(key) {
            case Qt::Key_Down:
                if (++row >= count)
                    row = 0;
                break;
            case Qt::Key_Up:
                if (--row < 0)
                    row = count - 1;
                break;
            }

            if (listView->isEnabled())
            {
                QModelIndex index = listView->model()->index(row, 0);
                listView->setCurrentIndex(index);
            }
        }
        else if ((Qt::Key_Enter == key || Qt::Key_Return == key || Qt::Key_Space == key) && listView->isEnabled())
        {
            if (currentIndex.isValid())
            {
                QString text = currentIndex.data().toString();
                setText(text + " ");
                listView->hide();
                setCompleter(this->text());
            }
            else if (this->text().length() > 1)
            {
                QString text = model->stringList().at(0);
                setText(text + " ");
                listView->hide();
                setCompleter(this->text());
                //to go at an instance such as this just add: QLineEdit::keyPressEvent(e);
            }
            else
            {
                QLineEdit::keyPressEvent(e);
            }
        }
        else if (Qt::Key_Escape == key)
        {
            listView->hide();
        }
        else
        {
            listView->hide();
            QLineEdit::keyPressEvent(e);
        }
    }
    else
    {
        if (key == Qt::Key_Down || key == Qt::Key_Up)
        {
            setCompleter(this->text());

            if (!listView->isHidden())
            {
                int row;
                switch(key) {
                case Qt::Key_Down:
                    row = 0;
                    break;
                case Qt::Key_Up:
                    row = listView->model()->rowCount() - 1;
                    break;
                }
                if (listView->isEnabled())
                {
                    QModelIndex index = listView->model()->index(row, 0);
                    listView->setCurrentIndex(index);
                }
            }
        }
        else
        {
            QLineEdit::keyPressEvent(e);
        }
    }
}

void vLocationLineEdit::setCompleter(const QString &text)
{
    if (text.isEmpty())
    {
        this->setProperty("destination", "");
        listView->hide();
        return;
    }
    /*
      This is there in the original but it seems to be bad for performance
      (keeping listview hidden unnecessarily - havn't thought about it properly though)
     */
//    if ((text.length() > 1) && (!listView->isHidden()))
//    {
//        return;
//    }

    QRegExp rx = QRegExp("^((\\d\\ )|(\\d))?[a-zA-Z]*\\b");
    rx.exactMatch(text);

    /*
        IF the old model had only 1 row
        OR the text matched a tag
        OR there was a valid selectedIndex
     */

    bool doSuggestions = true;
    if (!words->contains(rx.cap(0)) &&               //only if the starting text is not a recognised book already
        text.length() -1 == rx.cap(0).length() &&   //don't keep trying to complete when there's stuff after the book
        text.indexOf(QRegExp("[\\d\\s]$")) &&       //the last character must be a digit or whitespace
        listView->model()->rowCount() > 0)          //and there must be at least one possibility for book
    {
        if (listView->currentIndex().isValid())
        {
            setText(listView->currentIndex().data().toString() + " " + text.right(1));
            doSuggestions = false;
        }
        else if (text.length() > 2) //assume that the user wants the top book in the list
        {
            setText(model->stringList().at(0) + " " + text.right(1));
            doSuggestions = false;
        }
        //there is stuff there but no valid index so just doSuggestions
    }
    else if (text.length() > rx.cap(0).length() && !words->contains(rx.cap(0)))
    {
        QStringList possiblyPasted = filteredModelFromText(rx.cap(0));
        if (possiblyPasted.count() == 1)
        {
            //so we set the text to have the abbreviation fixed and the
            setText(possiblyPasted.at(0) + " " + text.mid(rx.cap(0).length()).trimmed());
            doSuggestions = false;
        }
    }

    if (doSuggestions)
    {
        model->setStringList(filteredModelFromText(text));
        if (model->rowCount() > 0)
            setProperty("destination", model->stringList().at(0));
    }

    if (model->rowCount() == 0)
    {
        return;
    }

    int maxVisibleRows = 10;
    // Position the text edit
    QPoint p(0, height());
    int x = mapToGlobal(p).x();
    int y = mapToGlobal(p).y() + 1;
    listView->move(x, y);
    listView->setMinimumWidth(width());
    listView->setMaximumWidth(width());
    if (model->rowCount() > maxVisibleRows)
    {
        listView->setFixedHeight(maxVisibleRows * (listView->fontMetrics().height() + 2) + 2);
    }
    else
    {
        listView->setFixedHeight(model->rowCount() * (listView->fontMetrics().height() + 2) + 2);
    }
    setProperty("destination", model->stringList().at(0));
    listView->show();
}

//Basically just a slot to connect to the listView's click event
void vLocationLineEdit::completeText(const QModelIndex &index)
{
    QString text = index.data().toString();
    setText(text);
    listView->hide();
}

QStringList vLocationLineEdit::filteredModelFromText(const QString &text)
{
    QStringList newFilteredModel;

    /*
      If statement as follows:
      If the text begins with a recognised book then
        If no valid chapter is selected show chapter range (if available)
        Else show destination
      If the text can be parsed as a book in some way then show options
     */
    QRegExp rx = QRegExp("^(\\d\\ )?[a-zA-Z]*\\b");
    rx.exactMatch(text);
    if (words->contains(rx.cap(0)))
    {
        if (listView->isEnabled())
            listView->setEnabled(false);

        QString book = rx.cap(0);
        QString textWithoutBook = text.mid(book.length());
        rx = QRegExp("^\\ (\\d+)\\D{0,3}(\\d+)?$");//at most 3 characters between chapter and verse "\\D{0,3}"
        rx.exactMatch(textWithoutBook);
        if (rx.cap(1).length() > 0 && rx.cap(2) > 0)
        {
            newFilteredModel << book + " " + rx.cap(1) + ":" + rx.cap(2);
        }
        else if (rx.cap(1).length() > 0)
        {
            newFilteredModel << book + " " + rx.cap(1) + ":1";
        }
        else
        {
            newFilteredModel << book + " 1:1";
        }
    }
    else
    {
        if (hash.contains(text))
        {
            newFilteredModel << words->at(hash[text] -1);
        }

        /*remove space between number and title of book for more promiscuous matching*/
        QString userTextToCompare = text.toLower().replace(QRegExp("^(\\d)\\ (.*)$"), "\\1\\2");
        foreach(QString word, *words)
        {
            QString comparisonWord = word.toLower().replace(QRegExp("^(\\d)\\ (.*)$"), "\\1\\2");
            if (comparisonWord.startsWith(userTextToCompare) && !newFilteredModel.contains(word))
            {
                newFilteredModel << word;
            }
        }
        foreach(QString word, *words)
        {
            QString comparisonWord = word.toLower().replace(QRegExp("^(\\d)\\ (.*)$"), "\\1\\2");
            if (comparisonWord.contains(userTextToCompare) && !newFilteredModel.contains(word))
            {
                newFilteredModel << word;
            }
        }

        if (newFilteredModel.count() > 0)
        {
            if (!listView->isEnabled())
                listView->setEnabled(true);
        }
    }
    return newFilteredModel;
}

void vLocationLineEdit::hideOptions()
{
    listView->hide();
}

QHash<QString, int> vLocationLineEdit::getBookHash()
{
    QHash<QString, int> hash;

    hash.insert("genesis", 1   );
    hash.insert("gen", 1   );
    hash.insert("exodus", 2    );
    hash.insert("ex", 2    );
    hash.insert("leviticus",3    );
    hash.insert("lev",3    );
    hash.insert("numbers",4    );
    hash.insert("num",4    );
    hash.insert("deuteronomy",5    );
    hash.insert("deut",5    );
    hash.insert("joshua",6    );
    hash.insert("josh",6    );
    hash.insert("judges",7    );
    hash.insert("jud",7    );
    hash.insert("ruth",8    );
    hash.insert("rut",8    );
    hash.insert("1 samuel",9    );
    hash.insert("1sam",9    );
    hash.insert("2 samuel",10    );
    hash.insert("2sam",10    );
    hash.insert("1 kings",11    );
    hash.insert("1ki",11    );
    hash.insert("2 kings",12    );
    hash.insert("2ki",12    );
    hash.insert("1 chronicles",13    );
    hash.insert("1ch",13    );
    hash.insert("2 chronicles",14    );
    hash.insert("2ch",14    );
    hash.insert("ezra",15    );
    hash.insert("ez",15    );
    hash.insert("nehemiah",16    );
    hash.insert("neh",16    );
    hash.insert("esther",17    );
    hash.insert("est",17    );
    hash.insert("job",18    );
    hash.insert("psalms",19    );
    hash.insert("ps",19    );
    hash.insert("proverbs",20    );
    hash.insert("pr",20    );
    hash.insert("ecclesiastes",21    );
    hash.insert("ecc",21    );
    hash.insert("song of songs",22    );
    hash.insert("ss",22    );
    hash.insert("isaiah",23    );
    hash.insert("is",23    );
    hash.insert("jeremiah",24    );
    hash.insert("jer",24    );
    hash.insert("lamentations",25    );
    hash.insert("lam",25    );
    hash.insert("ezekiel",26    );
    hash.insert("ez",26    );
    hash.insert("daniel",27    );
    hash.insert("dan",27    );
    hash.insert("hosea",28    );
    hash.insert("hos",28    );
    hash.insert("joel",29    );
    hash.insert("amos",30    );
    hash.insert("obadiah",31    );
    hash.insert("ob",31    );
    hash.insert("jonah",32    );
    hash.insert("micah",33    );
    hash.insert("mic",33    );
    hash.insert("nahum",34    );
    hash.insert("nah",34    );
    hash.insert("habakkuk",35    );
    hash.insert("hab",35    );
    hash.insert("zephaniah",36    );
    hash.insert("zep",36    );
    hash.insert("haggai",37    );
    hash.insert("hag",37    );
    hash.insert("zechariah",38    );
    hash.insert("zec",38    );
    hash.insert("malachi",39    );
    hash.insert("mal",39    );


    hash.insert( "matthew" ,  40 );
    hash.insert( "matt" ,  40 );
    hash.insert( "mat" ,  40 );
    hash.insert( "mt" ,  40 );
    hash.insert( "mark" ,  41 );
    hash.insert( "mrk" ,  41 );
    hash.insert( "mk" ,  41 );
    hash.insert( "mr" ,  41 );
    hash.insert( "luke" ,  42 );
    hash.insert( "luk" ,  42 );
    hash.insert( "lk" ,  42 );
    hash.insert( "john" ,  43 );
    hash.insert( "joh" ,  43 );
    hash.insert( "jn" ,  43 );
    hash.insert( "jhn" ,  43 );
    hash.insert( "acts" ,  44 );
    hash.insert( "ac" ,  44 );
    hash.insert( "romans" ,  45 );
    hash.insert( "rom" ,  45 );
    hash.insert( "ro" ,  45 );
    hash.insert( "rm" ,  45 );
    hash.insert( "1 corinthians" ,  46 );
    hash.insert( "1 cor" ,  46 );
    hash.insert( "1 co" ,  46 );
    hash.insert( "i co" ,  46 );
    hash.insert( "1co" ,  46 );
    hash.insert( "i cor" ,  46 );
    hash.insert( "1cor" ,  46 );
    hash.insert( "i corinthians" ,  46 );
    hash.insert( "1corinthians" ,  46 );
    hash.insert( "1st corinthians" ,  46 );
    hash.insert( "first corinthians" ,  46 );
    hash.insert( "2 corinthians" ,  47 );
    hash.insert( "2 cor" ,  47 );
    hash.insert( "2 co" ,  47 );
    hash.insert( "ii co" ,  47 );
    hash.insert( "2co" ,  47 );
    hash.insert( "ii cor" ,  47 );
    hash.insert( "2cor" ,  47 );
    hash.insert( "ii corinthians" ,  47 );
    hash.insert( "2corinthians" ,  47 );
    hash.insert( "2nd corinthians" ,  47 );
    hash.insert( "second corinthians" ,  47 );
    hash.insert( "galatians" ,  48 );
    hash.insert( "gal" ,  48 );
    hash.insert( "ga" ,  48 );
    hash.insert( "ephesians" ,  49 );
    hash.insert( "ephes" ,  49 );
    hash.insert( "eph" ,  49 );
    hash.insert( "philippians" ,  50 );
    hash.insert( "phil" ,  50 );
    hash.insert( "php" ,  50 );
    hash.insert( "colossians" ,  51 );
    hash.insert( "col" ,  51 );
    hash.insert( "1 thessalonians" ,  52 );
    hash.insert( "1 thess" ,  52 );
    hash.insert( "1 thes" ,  52 );
    hash.insert( "1 th" ,  52 );
    hash.insert( "i th" ,  52 );
    hash.insert( "1th" ,  52 );
    hash.insert( "i thes" ,  52 );
    hash.insert( "1thes" ,  52 );
    hash.insert( "i thess" ,  52 );
    hash.insert( "1thess" ,  52 );
    hash.insert( "i thessalonians" ,  52 );
    hash.insert( "1thessalonians" ,  52 );
    hash.insert( "1st thessalonians" ,  52 );
    hash.insert( "first thessalonians" ,  52 );
    hash.insert( "2 thessalonians" ,  53 );
    hash.insert( "2 thess" ,  53 );
    hash.insert( "2 th" ,  53 );
    hash.insert( "ii th" ,  53 );
    hash.insert( "2th" ,  53 );
    hash.insert( "ii thes" ,  53 );
    hash.insert( "2thes" ,  53 );
    hash.insert( "ii thess" ,  53 );
    hash.insert( "2thess" ,  53 );
    hash.insert( "ii thessalonians" ,  53 );
    hash.insert( "2thessalonians" ,  53 );
    hash.insert( "2nd thessalonians" ,  53 );
    hash.insert( "second thessalonians" ,  53 );
    hash.insert( "1 timothy" ,  54 );
    hash.insert( "1 tim" ,  54 );
    hash.insert( "1 ti" ,  54 );
    hash.insert( "i ti" ,  54 );
    hash.insert( "1ti" ,  54 );
    hash.insert( "i tim" ,  54 );
    hash.insert( "1tim" ,  54 );
    hash.insert( "i timothy" ,  54 );
    hash.insert( "1timothy" ,  54 );
    hash.insert( "1st timothy" ,  54 );
    hash.insert( "first timothy" ,  54 );
    hash.insert( "2 timothy" ,  55 );
    hash.insert( "2 tim" ,  55 );
    hash.insert( "2 ti" ,  55 );
    hash.insert( "ii ti" ,  55 );
    hash.insert( "2ti" ,  55 );
    hash.insert( "ii tim" ,  55 );
    hash.insert( "2tim" ,  55 );
    hash.insert( "ii timothy" ,  55 );
    hash.insert( "2timothy" ,  55 );
    hash.insert( "2nd timothy" ,  55 );
    hash.insert( "second timothy" ,  55 );
    hash.insert( "titus" ,  56 );
    hash.insert( "tit" ,  56 );
    hash.insert( "philemon" ,  57 );
    hash.insert( "philem" ,  57 );
    hash.insert( "phm" ,  57 );
    hash.insert( "hebrews" ,  58 );
    hash.insert( "heb" ,  58 );
    hash.insert( "james" ,  59 );
    hash.insert( "jas" ,  59 );
    hash.insert( "jm" ,  59 );
    hash.insert( "1 peter" ,  60 );
    hash.insert( "1 pet" ,  60 );
    hash.insert( "1 pe" ,  60 );
    hash.insert( "i pe" ,  60 );
    hash.insert( "1pe" ,  60 );
    hash.insert( "i pet" ,  60 );
    hash.insert( "1pet" ,  60 );
    hash.insert( "i pt" ,  60 );
    hash.insert( "1 pt" ,  60 );
    hash.insert( "1pt" ,  60 );
    hash.insert( "i peter" ,  60 );
    hash.insert( "1peter" ,  60 );
    hash.insert( "1st peter" ,  60 );
    hash.insert( "first peter" ,  60 );
    hash.insert( "2 peter" ,  61 );
    hash.insert( "2 pet" ,  61 );
    hash.insert( "2 pe" ,  61 );
    hash.insert( "ii pe" ,  61 );
    hash.insert( "2pe" ,  61 );
    hash.insert( "ii pet" ,  61 );
    hash.insert( "2pet" ,  61 );
    hash.insert( "ii pt" ,  61 );
    hash.insert( "2 pt" ,  61 );
    hash.insert( "2pt" ,  61 );
    hash.insert( "ii peter" ,  61 );
    hash.insert( "2peter" ,  61 );
    hash.insert( "2nd peter" ,  61 );
    hash.insert( "second peter" ,  61 );
    hash.insert( "1 john" ,  62 );
    hash.insert( "1 jn" ,  62 );
    hash.insert( "i jn" ,  62 );
    hash.insert( "1jn" ,  62 );
    hash.insert( "i jo" ,  62 );
    hash.insert( "1jo" ,  62 );
    hash.insert( "i joh" ,  62 );
    hash.insert( "1joh" ,  62 );
    hash.insert( "i jhn" ,  62 );
    hash.insert( "1 jhn" ,  62 );
    hash.insert( "1jhn" ,  62 );
    hash.insert( "i john" ,  62 );
    hash.insert( "1john" ,  62 );
    hash.insert( "1st john" ,  62 );
    hash.insert( "first john" ,  62 );
    hash.insert( "2 john" ,  63 );
    hash.insert( "2 jn" ,  63 );
    hash.insert( "ii jn" ,  63 );
    hash.insert( "2jn" ,  63 );
    hash.insert( "ii jo" ,  63 );
    hash.insert( "2jo" ,  63 );
    hash.insert( "ii joh" ,  63 );
    hash.insert( "2joh" ,  63 );
    hash.insert( "ii jhn" ,  63 );
    hash.insert( "2 jhn" ,  63 );
    hash.insert( "2jhn" ,  63 );
    hash.insert( "ii john" ,  63 );
    hash.insert( "2john" ,  63 );
    hash.insert( "2nd john" ,  63 );
    hash.insert( "second john" ,  63 );
    hash.insert( "3 john" ,  64 );
    hash.insert( "3 jn" ,  64 );
    hash.insert( "iii jn" ,  64 );
    hash.insert( "3jn" ,  64 );
    hash.insert( "iii jo" ,  64 );
    hash.insert( "3jo" ,  64 );
    hash.insert( "iii joh" ,  64 );
    hash.insert( "3joh" ,  64 );
    hash.insert( "iii jhn" ,  64 );
    hash.insert( "3 jhn" ,  64 );
    hash.insert( "3jhn" ,  64 );
    hash.insert( "iii john" ,  64 );
    hash.insert( "3john" ,  64 );
    hash.insert( "3rd john" ,  64 );
    hash.insert( "third john" ,  64 );
    hash.insert( "jude" ,  65 );
    hash.insert( "jud" ,  65 );
    hash.insert( "revelation" ,  66);
    hash.insert( "rev" ,  66 );
    hash.insert( "re" ,  66 );

    return hash;
}
