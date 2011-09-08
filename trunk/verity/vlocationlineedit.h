#ifndef LOCATIONLINEEDIT_H
#define LOCATIONLINEEDIT_H

#include <QLineEdit>
#include <QListView>
#include <QStringListModel>
#include <QStringList>
#include <QHash>

class QListView;
class QStringListModel;
class QModelIndex;

class vLocationLineEdit : public QLineEdit
{

    Q_OBJECT
public:
    vLocationLineEdit(QString activeText, QWidget *parent = 0);
    void hideOptions();

public slots:
    void setCompleter(const QString &text);
    void completeText(const QModelIndex &index);

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);

private:
    QStringList *words;
    QListView *listView;
    QStringListModel *model;
    QStringList filteredModelFromText(const QString &text);
    int maxVisibleRows;
    QHash<QString, int> hash;
    QHash<QString, int> getBookHash();
};

#endif // LOCATIONLINEEDIT_H
