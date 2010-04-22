#include "qsnapscrollbar.h"
#include <QDebug>
#include <QtGui/QWidget>
#include <QStyleOptionSlider>
#include <QMouseEvent>
#include <QStyle>

QSnapScrollBar::QSnapScrollBar(QWidget* parent)
 : MarkedScrollBar(parent)
{
}

void QSnapScrollBar::mouseMoveEvent(QMouseEvent* e)
{
    QPoint click = e->pos();
    int newPosition = pixelPosToRangeValue(click.y());

    //make the snap range a quarter of the length of the slider
    QStyleOptionSlider opt;
    this->initStyleOption(&opt);
    int sliderHeight = this->pageStep();


    int maxSnapDistance = sliderHeight / 3;
    for (int i = 0; i < snapPoint.size(); i++)
    {
        int sp = snapPoint.at(i);
        if (newPosition != 0 && newPosition !=maximum())
        {
            /*we break out of the loop because otherwise, in theory, it could just snap itself to the end...
              testing for a snap to the beginning of a chapter first is also more useful
                - it means that the bar will prefer to snap to the start of a chapter
              */
            if (abs(newPosition - sp) < maxSnapDistance) //snap to beginning of chapter
            {
                newPosition = sp;
                break;
            }
            else if (abs(newPosition - (sp - sliderHeight)) < maxSnapDistance) //snap to end of chapter
            {
                newPosition = sp - sliderHeight;
                break;
            }
        }
    }
    setSliderPosition(newPosition);
}


int QSnapScrollBar::pixelPosToRangeValue(int pos) const
{
    QStyleOptionSlider opt;
    this->initStyleOption(&opt);

    QRect gr = this->style()->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarGroove, this);
    QRect sr = this->style()->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarSlider, this);
    int sliderMin, sliderMax, sliderLength;
    sliderLength = sr.height();
    sliderMin = gr.y();
    sliderMax = gr.bottom() - sliderLength + 1;

    return  QStyle::sliderValueFromPosition(minimum(), maximum(), pos - sliderMin - sr.height()/2,
                                            sliderMax - sliderMin, opt.upsideDown);
}

void QSnapScrollBar::defineSnapPoints(QList<int> newSnapPoint)
{
    this->snapPoint = newSnapPoint;
}
