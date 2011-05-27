#include "locationlineedit.h"
#include <QKeyEvent>
#include <QtGui/QListView>
#include <QtGui/QStringListModel>
#include <QDebug>

LocationLineEdit::LocationLineEdit(QStringList *words, QHash<QString, int> *hash, QVector<int> *bookChapterRange, int maxVisibleRows, QWidget *parent)
    : QLineEdit(parent), words(**&words), hash(**&hash)
{
    listView = new QListView(this);
    model = new QStringListModel(this);
    listView->setWindowFlags(Qt::ToolTip);

    connect(this, SIGNAL(textChanged(const QString &)), this, SLOT(setCompleter(const QString &)));
    connect(listView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(completeText(const QModelIndex &)));

    this->bookChapterRange = new QVector<int>;
    this->bookChapterRange = bookChapterRange;
    this->maxVisibleRows = &maxVisibleRows;

    listView->setModel(model);
}

void LocationLineEdit::focusOutEvent(QFocusEvent *e)
{
    listView->hide();
    QLineEdit::focusOutEvent(e);
}
void LocationLineEdit::keyPressEvent(QKeyEvent *e)
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

void LocationLineEdit::setCompleter(const QString &text)
{
    if (text.isEmpty())
    {
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

    QRegExp rx = QRegExp("^[(\\d\\ )(\\d)]?[a-zA-Z]*\\b");
    rx.exactMatch(text);
    /*
        IF the old model had only 1 row
        OR the text matched a tag
        OR there was a valid selectedIndex
     */

    if (!words.contains(rx.cap(0)) &&               //only if the starting text is not a recognised book already
        text.length() -1 == rx.cap(0).length() &&   //don't keep trying to complete when there's stuff after the book
        text.indexOf(QRegExp("[\\d\\s]$")) &&       //the last character must be a digit or whitespace
        listView->model()->rowCount() > 0)          //and there must be at least one possibility for book
    {
        if (listView->currentIndex().isValid())
        {
            setText(listView->currentIndex().data().toString() + " " + text.right(1));
        }
        else if (text.length() > 2) //assume that the user wants the top book in the list
        {
            setText(model->stringList().at(0) + " " + text.right(1));
        }
        else
        {
            //there is stuff there but no valid index
            model->setStringList(filteredModelFromText(text));
        }
    }
    else
    {
        model->setStringList(filteredModelFromText(text));
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
    listView->show();
}

//Basically just a slot to connect to the listView's click event
void LocationLineEdit::completeText(const QModelIndex &index)
{
    QString text = index.data().toString();
    setText(text);
    listView->hide();
}

QStringList LocationLineEdit::filteredModelFromText(const QString &text)
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
    if (words.contains(rx.cap(0)))
    {
        QString book = rx.cap(0);
        QString textWithoutBook = text.mid(book.length());
        rx = QRegExp("^\\ ?(\\d{1,3})\\D(\\d{1,3})$");
        rx.exactMatch(textWithoutBook);
        if (rx.cap(1).length() > 0 && rx.cap(2) > 0)
        {
            if (listView->isEnabled())
                listView->setEnabled(false);

            newFilteredModel << " > " + book + " " + rx.cap(1) + ":" + rx.cap(2);
        }
        else
        {
            if (listView->isEnabled())
                listView->setEnabled(false);

            int index = words.indexOf(book);
            if (index < bookChapterRange->count())
                newFilteredModel << book + " 1-" + QVariant(bookChapterRange->at(index)).toString();
        }
    }
    else
    {
        if (hash.contains(text))
        {
            newFilteredModel << words.at(hash[text] -1);
        }

        /*remove space between number and title of book for more promiscuous matching*/
        QString userTextToCompare = text.toLower().replace(QRegExp("^(\\d)\\ (.*)$"), "\\1\\2");
        foreach(QString word, words)
        {
            QString comparisonWord = word.toLower().replace(QRegExp("^(\\d)\\ (.*)$"), "\\1\\2");
            if (comparisonWord.startsWith(userTextToCompare) && !newFilteredModel.contains(word))
            {
                newFilteredModel << word;
            }
        }
        foreach(QString word, words)
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
