#ifndef LOCATIONLINEEDIT_H
#define LOCATIONLINEEDIT_H

#include <QLineEdit>
#include <QStringList>
#include <QHash>

class QListView;
class QStringListModel;
class QModelIndex;

class LocationLineEdit : public QLineEdit
{

    Q_OBJECT
public:
    LocationLineEdit(QStringList *words, QHash<QString, int> *hash, QVector<int> *bookChapterRange, int maxVisibleRows = 15, QWidget *parent = 0);

public slots:
    void setCompleter(const QString &text);
    void completeText(const QModelIndex &index);

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);

private:
    QStringList words;
    QListView *listView;
    QStringListModel *model;
    QStringList filteredModelFromText(const QString &text);
    int *maxVisibleRows;
    QHash<QString, int> hash;
    QVector<int> *bookChapterRange;
};

#endif // LOCATIONLINEEDIT_H
