#include "vwebview.h"
#include <QApplication>
#include <QWheelEvent>
#include <QDebug>
#include <QSettings>
#include "eventmanager.h"

int VWebView::staticIdentifier = 1;
QString VWebView::VWEBVIEW_SETTING_GROUP = "VWebViewSettingGroup";

VWebView::VWebView(QWidget *parent) :
    QWebView(parent), Listener()
{
    EventManager::addListener(EventType::CLOSING, this);

    thisId = staticIdentifier++;

    QSettings settings(PROGRAM_NAME, PROGRAM_NAME);
    settings.beginGroup(VWEBVIEW_SETTING_GROUP);
    setTextSizeMultiplier(settings.value(QString().setNum(thisId), 1).toReal());
    settings.endGroup();
}

void VWebView::handleEvent(Event* event)
{
    QSettings settings(PROGRAM_NAME, PROGRAM_NAME);

    settings.beginGroup(VWEBVIEW_SETTING_GROUP);
    settings.setValue(QString().setNum(thisId), textSizeMultiplier());
    settings.endGroup();
}

void VWebView::wheelEvent(QWheelEvent *event)
{
    if((QApplication::keyboardModifiers() & Qt::ControlModifier) > 0)
    {        
        float zoom = event->delta() / qAbs(event->delta());

        setTextSizeMultiplier(textSizeMultiplier() + 0.05f*zoom);
        event->accept();
        zoomed();
    }
    else
    {
        QWebView::wheelEvent(event);
    }
}

void VWebView::zoomed()
{
}

VWebView::~VWebView()
{
}

void VWebView::dropEvent (QDropEvent * ev) //prevents the drop of an html file that then loads
{
}


