#include "vbibleinterface.h"
#include <QDebug>

VBibleInterface::VBibleInterface(QWidget *parent) :
    QWidget(parent)
{
    vbox = new QVBoxLayout(this);

    locationEdit = new VLocationEdit(this);
    vbox->addWidget(locationEdit);
    //this should be dynamic based on a variable passed to the constructor
    QVector<QString> texts;
    texts.append("net [fix this code!]");
    texts.append("tisch [fix this code!]");
    texts.append("wlc [fix this code!]");
    qDebug() << "vBibleInterface - 1";
    textSelect = new VTextSelect(texts);
    qDebug() << "vBibleInterface - 1.5";
    vbox->addWidget(textSelect);
    qDebug() << "vBibleInterface - 2";
    bibleWebView = new BibleTextBrowser(this);
    vbox->addWidget(bibleWebView, 1);

//    vbox->setStretch(0,0);//makes bibleWebview fill up the space
//    vbox->setStretch(1,1);//makes bibleWebview fill up the space


    this->setLayout(vbox);
    qDebug() << "vBibleInterface - success";
}
