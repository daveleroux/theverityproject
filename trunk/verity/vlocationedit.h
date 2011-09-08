#ifndef VLOCATIONEDIT_H
#define VLOCATIONEDIT_H

#include <QWidget>
#include <QPushButton>
#include <QStackedWidget>
#include "vlocationlineedit.h"
#include "vlocationdropdowns.h"

class vLocationEdit : public QWidget
{
    Q_OBJECT
public:
    explicit vLocationEdit(QWidget *parent = 0);

private:
    QHBoxLayout mainLayout;
    QStackedWidget *stackedWidget;

    vLocationDropDowns *locationDropDowns;
    vLocationLineEdit *locationLineEdit;

    QPushButton btnSwitchView;

public slots:
    void switchView(bool toggle);
    void go();
};

#endif // VLOCATIONEDIT_H
