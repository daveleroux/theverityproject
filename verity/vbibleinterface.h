#ifndef VBIBLEINTERFACE_H
#define VBIBLEINTERFACE_H

#include <QWidget>
#include "vlocationedit.h"
#include "bibletextbrowser.h"
#include "vtextselect.h"

class vBibleInterface : public QWidget
{
    Q_OBJECT
public:
    explicit vBibleInterface(QWidget *parent = 0);
    //TODO: make this private (I think only mainwindow::aftershown will be affected)
    BibleTextBrowser *bibleWebView;

signals:

public slots:

private:
    QVBoxLayout *vbox;
    vLocationEdit *locationEdit;
    vTextSelect *textSelect;
};

#endif // VBIBLEINTERFACE_H
