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
    QString getHtml(int v=0);
public slots:
    void loadFinished(bool b);
};

#endif // MAINWINDOW_H
