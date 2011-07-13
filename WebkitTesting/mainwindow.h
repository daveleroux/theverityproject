#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWebKit/QWebView>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QWebView* webView;
    explicit MainWindow(QWidget *parent = 0);
    void wordClicked(int id);
    ~MainWindow();
    QString getHtml(int normalisedChapter);
    void display(int normalisedChapter);
    void append(int normalisedChapter, QString html);
    void append();
    QList<int> normalisedChapters;

public slots:
    void loadFinished(bool b);
    void scrolled();
};

#endif // MAINWINDOW_H
