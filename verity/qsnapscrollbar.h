#ifndef QSNAPSCROLLBAR_H
#define QSNAPSCROLLBAR_H

#include "markedscrollbar.h"
#include <QList>

class QSnapScrollBar : public MarkedScrollBar
{
    Q_OBJECT

public:
//    QSnapScrollBar();
    QSnapScrollBar(QWidget *parent);

protected:
    void mouseMoveEvent ( QMouseEvent * event );
    int pixelPosToRangeValue(int pos) const;
    QList<int> snapPoint;

    //void dragMoveEvent(QDragMoveEvent event);
    //void sliderChange(SliderChange change);

public slots:
    void defineSnapPoints(QList<int>);
//    void blah(int value);
};

#endif // QSNAPSCROLLBAR_H
