#ifndef SEARCHBROWSER_H
#define SEARCHBROWSER_H

#include <QTextBrowser>
#include "versereferenceparser.h"

class SearchBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    SearchBrowser(QWidget* parent=0);
    void performSearch(QString);
    void setTextsAvaiable(QList<int>);

private:
    QList<int> textsAvailable;

//protected:
//    QVariant loadResource(int type, const QUrl& name);

signals:
    void goToResult(QList<int>, VerseReference);

public slots:
    void anchorClickedSlot(QUrl url);

};

#endif // SEARCHBROWSER_H
