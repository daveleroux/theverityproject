#ifndef VTEXTSELECT_H
#define VTEXTSELECT_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>

class vTextSelect : public QWidget
{
    Q_OBJECT
public:
    explicit vTextSelect(QVector<QString> availableActiveTexts, QWidget *parent = 0);

signals:

public slots:

private:
    QHBoxLayout *hbox;
    QVector<QPushButton*> vectorBtnTextOption;
};

#endif // VTEXTSELECT_H
