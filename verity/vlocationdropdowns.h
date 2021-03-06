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
    VLocationDropDowns(QString activeText, QWidget *parent = 0);
    ~VLocationDropDowns();
    void setLocation(VerseReference reference);

private slots:
    void goClicked();
    void bookSelected(QString text);
    void chapterSelected(QString text);

signals:
    void goSignal();

private:
    QStringList availableBooks;

    QHBoxLayout *mainLayout;
    QComboBox *cmbBookSelector;
    QComboBox *cmbChapterSelector;
    QComboBox *cmbVerseSelector;
    QToolButton *btnGo;

};

#endif // VLOCATIONDROPDOWNS_H
