#ifndef VBIBLEINTERFACE_H
#define VBIBLEINTERFACE_H

#include <QWidget>
#include "vlocationedit.h"
#include "bibletextbrowser.h"

class VBibleInterface : public QWidget
{
    Q_OBJECT
public:
    explicit VBibleInterface(QWidget *parent = 0);
    //TODO: make this private (I think only mainwindow::aftershown will be affected)
    BibleTextBrowser *bibleWebView;

signals:

public slots:

private:
    QVBoxLayout *vbox;
    VLocationEdit *locationEdit;
};

#endif // VBIBLEINTERFACE_H
