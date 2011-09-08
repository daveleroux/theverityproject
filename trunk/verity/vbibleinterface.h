#ifndef VBIBLEINTERFACE_H
#define VBIBLEINTERFACE_H

#include <QWidget>
#include "vlocationedit.h"
#include "bibletextbrowser.h"

class vBibleInterface : public QWidget
{
    Q_OBJECT
public:
    explicit vBibleInterface(QWidget *parent = 0);
    BibleTextBrowser *bibleWebView;

signals:

public slots:

private:
    QVBoxLayout *vbox;
    vLocationEdit *locationEdit;
};

#endif // VBIBLEINTERFACE_H
