#ifndef VLOCATIONDROPDOWNS_H
#define VLOCATIONDROPDOWNS_H

#include <QWidget>
#include <QComboBox>
#include <QHBoxLayout>
#include <QToolButton>
#include "versereference.h"

class VLocationDropDowns : public QWidget
{
    Q_OBJECT
public:
    explicit VLocationDropDowns(QString activeText, QWidget *parent = 0);
    void setLocation(VerseReference reference);

private slots:
    void goClicked();

signals:
    void goSignal();

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