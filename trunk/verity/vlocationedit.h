#ifndef VLOCATIONEDIT_H
#define VLOCATIONEDIT_H

#include <QWidget>
#include <QPushButton>
#include <QStackedWidget>
#include "vlocationlineedit.h"
#include "vlocationdropdowns.h"

class VLocationEdit : public QWidget
{
    Q_OBJECT
public:
    explicit VLocationEdit(QVector<QString> texts, QWidget *parent = 0);

private:
    QToolButton btnTextAndViewSelect;
    QHBoxLayout* mainLayout;
    QStackedWidget *stackedWidget;

    VLocationDropDowns *locationDropDowns;
    VLocationLineEdit *locationLineEdit;

    QPushButton btnSwitchView;

public slots:
    void shortcutActivated();
    void switchStackedWidget(bool toggle);
    void switchParallelView();
    void go();
};

#endif // VLOCATIONEDIT_H
