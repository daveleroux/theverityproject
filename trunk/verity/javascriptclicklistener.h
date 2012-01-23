#ifndef JAVASCRIPTCLICKLISTENER_H
#define JAVASCRIPTCLICKLISTENER_H

#include <QObject>

class JavascriptClickListener : public QObject
{
    Q_OBJECT
public:
    JavascriptClickListener();
public slots:
    void wordClicked(int bibletextId, int wordId);
    void netNoteClicked(int id);
    void strongsClicked(int id);
    void bibleReferenceClicked(int book, int chapter, int verse);
};

#endif // JAVASCRIPTCLICKLISTENER_H
