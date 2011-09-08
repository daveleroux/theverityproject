#include "vtextselect.h"
#include <QDebug>

vTextSelect::vTextSelect(QVector<QString> availableTexts, QWidget *parent) :
    QWidget(parent)
{
    hbox = new QHBoxLayout();

    foreach (QString textName, availableTexts)
    {
        vectorBtnTextOption.append(new QPushButton(textName, this));
        vectorBtnTextOption.last()->setCheckable(true);
        hbox->addWidget(vectorBtnTextOption.last());
    }
    this->setLayout(hbox);
}
