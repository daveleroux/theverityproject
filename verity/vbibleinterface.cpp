#include "vbibleinterface.h"
#include <QDebug>

VBibleInterface::VBibleInterface(QWidget *parent) :
    QWidget(parent)
{
    vbox = new QVBoxLayout(this);

    vbox->setContentsMargins(7,0,0,0);
    vbox->setSpacing(0);


    //this should be dynamic based on a variable passed to the constructor
    QVector<QString> texts;
    texts.append("net [fix this code!]");
    texts.append("tisch [fix this code!]");
    texts.append("wlc [fix this code!]");

    locationEdit = new VLocationEdit(texts, this);
    vbox->addWidget(locationEdit);
    bibleWebView = new BibleTextBrowser(this);
    vbox->addWidget(bibleWebView, 1);

    connect(bibleWebView, SIGNAL(backwardAvailable(bool)), locationEdit->backButton, SLOT(setEnabled(bool)));
    connect(bibleWebView, SIGNAL(forwardAvailable(bool)), locationEdit->forwardButton, SLOT(setEnabled(bool)));
    connect(locationEdit->backButton, SIGNAL(clicked()), bibleWebView, SLOT(backward()));
    connect(locationEdit->forwardButton, SIGNAL(clicked()), bibleWebView, SLOT(forward()));

    this->setLayout(vbox);
}
