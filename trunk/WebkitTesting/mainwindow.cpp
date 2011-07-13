#include "mainwindow.h"
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QFile>
#include <QTextStream>
#include "clicklistener.h"
#include "scrolllistener.h"
#include <QMessageBox>
#include <QtSql>
#include <QXmlQuery>
#include <QWebElement>

QString MainWindow::getHtml(int normalisedChapter)
{

    QSqlQuery query;

    if(!query.exec("select text from net_bible where normalised_chapter=" + QString().setNum(normalisedChapter)))
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

    QString htmlFrame = "<html>"
                        "<head>"
                                "<link href=\"qrc:/style/bible.css\" rel=\"stylesheet\" type=\"text/css\"/>"
                        "</head>"
                        "<body>"
                        "</body>"
                        "</html>";


    webView->setHtml(htmlFrame);

    display(23);
}

void MainWindow::append(int normalisedChapter, QString html)
{
    normalisedChapters.append(normalisedChapter);

    QWebElement doc = webView->page()->mainFrame()->documentElement();
    QWebElement body = doc.firstChild().nextSibling();
    body.appendInside(html);


}

void MainWindow::append()
{
    int last = normalisedChapters.last();
    append(last+1, getHtml(last+1));
}

void MainWindow::display(int normalisedChapter)
{
    normalisedChapters.clear();
    append(normalisedChapter, getHtml(normalisedChapter));
}

void MainWindow::scrolled()
{
    int value =  webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);
    int height =  webView->page()->mainFrame()->scrollBarGeometry(Qt::Vertical).height();
    int max =  webView->page()->mainFrame()->scrollBarMaximum(Qt::Vertical);

    if(max - (value+height) < height)
    {
        append();
    }
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


    ScrollListener* scrollListener = new ScrollListener();
    connect(scrollListener, SIGNAL(scrolledSignal()), this, SLOT(scrolled()));

    webView->page()->mainFrame()->addToJavaScriptWindowObject("scrollListener", scrollListener);

    webView->page()->mainFrame()->evaluateJavaScript("window.onscroll = function () { "
        "scrollListener.scrolled(); "
        "}");
}
