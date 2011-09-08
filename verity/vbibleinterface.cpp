#include "vbibleinterface.h"
#include <QDebug>

vBibleInterface::vBibleInterface(QWidget *parent) :
    QWidget(parent)
{
    vbox = new QVBoxLayout();

    locationEdit = new vLocationEdit(this);
    vbox->addWidget(locationEdit);
    bibleWebView = new BibleTextBrowser();
    vbox->addWidget(bibleWebView);

    this->setLayout(vbox);
    qDebug() << "vBibleInterface - success";
}
