#ifndef VLOCATIONDROPDOWNS_H
#define VLOCATIONDROPDOWNS_H

#include <QWidget>
#include <QComboBox>
#include <QHBoxLayout>

class vLocationDropDowns : public QWidget
{
    Q_OBJECT
public:
    explicit vLocationDropDowns(QString activeText, QWidget *parent = 0);

private:
    QStringList availableBooks;
    QVector<int> *bookChapterRange;

    QHBoxLayout *mainLayout;
    QComboBox *cmbBookSelector;
    QComboBox *cmbChapterSelector;
    QComboBox *cmbVerseSelector;

};

#endif // VLOCATIONDROPDOWNS_H
