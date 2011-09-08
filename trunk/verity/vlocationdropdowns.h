#ifndef VLOCATIONDROPDOWNS_H
#define VLOCATIONDROPDOWNS_H

#include <QWidget>
#include <QComboBox>
#include <QHBoxLayout>
#include <QToolButton>

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
    QToolButton *btnGo;

};

#endif // VLOCATIONDROPDOWNS_H
