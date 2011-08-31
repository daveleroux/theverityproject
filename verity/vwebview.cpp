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
        int numDegrees = event->delta() / 8;
        int numSteps = numDegrees / 15;
        float zoom = numSteps/10.0f;
        setTextSizeMultiplier(textSizeMultiplier() + zoom);
        event->accept();
    }
    else
    {
        QWebView::wheelEvent(event);
    }
}

VWebView::~VWebView()
{
}

