#include "basicwebhistoryitem.h"
#include <QWebFrame>

BasicWebHistoryItem::BasicWebHistoryItem(VWebView* webView, QString html) : VWebHistoryItem(webView)
{
    this->html = html;
}

BasicWebHistoryItem::~BasicWebHistoryItem()
{
}

void BasicWebHistoryItem::saveState()
{
    scrollValue = webView->page()->mainFrame()->scrollBarValue(Qt::Vertical);
}

void BasicWebHistoryItem::restore()
{
    webView->setHtml(html);
    webView->page()->mainFrame()->setScrollBarValue(Qt::Vertical, scrollValue);
}

void BasicWebHistoryItem::display()
{
    webView->setHtml(html);
}

