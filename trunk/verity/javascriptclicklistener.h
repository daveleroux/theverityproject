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
};

#endif // JAVASCRIPTCLICKLISTENER_H