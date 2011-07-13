#include "mainwindow.h"
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QFile>
#include <QTextStream>
#include "clicklistener.h"
#include <QMessageBox>
#include <QtSql>
#include <QXmlQuery>

QString MainWindow::getHtml()
{

    QSqlQuery query;

    if(!query.exec("select text from net_bible where normalised_chapter=973"))
    {
        qDebug() << "failed: " << query.lastError() << endl;
        exit(1);
    }

    QString wholeChapter = "<normalisedChapter>";
    while(query.next())
    {
        QString text = query.value(0).toString();
        wholeChapter = wholeChapter + text;
    }
    wholeChapter = wholeChapter + "</normalisedChapter>";

    //    qDebug() << wholeChapter;


    QXmlQuery xmlQuery(QXmlQuery::XSLT20);

    QByteArray byteArray(wholeChapter.toUtf8());

    QBuffer buffer(&byteArray);
    buffer.open(QBuffer::ReadWrite);

    xmlQuery.setFocus(&buffer);

    xmlQuery.setQuery(QUrl("qrc:/xsl/bible.xsl"));

    QString result;
    xmlQuery.evaluateTo(&result);

    return result;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("verity.sqlite");
    if (!db.open())
    {
        qDebug() << "couldn't open db" << endl;
        exit(1);
    }




    webView = new QWebView();
    setCentralWidget(webView);


    connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));

    //webView->settings()->setAttribute(QWebSettings::JavascriptEnabled,true);

    //    QString html = "<html><head></head><body><h1>hello world</h1></body></html>";
    //    for(int i=1; i<= 32; i++ )
    //    {
    //        qDebug() << i;
    //        getHtml(i);
    //    }
    QString html = getHtml();
    qDebug() << html;
    webView->setHtml(html, QUrl::fromLocalFile(QDir::currentPath()));

    //    webView->load(QUrl("http://qt.nokia.com/"));
    //         webView->show();
}

void MainWindow::wordClicked(int id)
{
    QMessageBox msgBox;
    msgBox.setSizeGripEnabled(true);

    msgBox.setText(QString().setNum(id) );
    msgBox.exec();
}

MainWindow::~MainWindow()
{

}

void MainWindow::loadFinished(bool b)
{
    ClickListener* clickListener = new ClickListener(this);
    webView->page()->mainFrame()->addToJavaScriptWindowObject("clickListener", clickListener);
}
