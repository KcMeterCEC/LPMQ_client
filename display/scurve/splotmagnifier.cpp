#include <qwt_plot.h>

#include "splotmagnifier.h"

SplotMagnifier::SplotMagnifier(QWidget * parent):
    QwtPlotMagnifier(parent)
{
    setAxisEnabled(QwtPlot::yLeft, false);
}
void SplotMagnifier::setXRange(qreal min, qreal max)
{
    xBottomMin = min;
    xBottomMax = max;
}
void SplotMagnifier::rescale( double factor )
{
    QwtPlot* plt = plot();
    if ( plt == NULL )
        return;
    factor = qAbs( factor );
    if ( factor == 1.0 || factor == 0.0 )
        return;

    bool doReplot = false;

    const bool autoReplot = plt->autoReplot();
    plt->setAutoReplot( false );


    const QwtScaleMap scaleMap = plt->canvasMap( QwtPlot::xBottom );

    double v1 = scaleMap.s1();
    double v2 = scaleMap.s2();

    if ( scaleMap.transformation() )
    {
        // the coordinate system of the paint device is always linear

        v1 = scaleMap.transform( v1 ); // scaleMap.p1()
        v2 = scaleMap.transform( v2 ); // scaleMap.p2()
    }

    const double center = 0.5 * ( v1 + v2 );
    const double width_2 = 0.5 * ( v2 - v1 ) * factor;

    v1 = center - width_2;
    v2 = center + width_2;

    if ( scaleMap.transformation() )
    {
        v1 = scaleMap.invTransform( v1 );
        v2 = scaleMap.invTransform( v2 );
    }

    bool isAutoScale = false;
    if(v1 < xBottomMin)
    {
        v1 = xBottomMin;
        isAutoScale = true;
    }
    if(v2 > xBottomMax)
    {
        v2 = xBottomMax;
        isAutoScale = true;
    }

    plt->setAxisScale( QwtPlot::xBottom, v1, v2 );
    doReplot = true;

    if(isAutoScale)
    {
        plt->setAxisAutoScale(QwtPlot::xBottom, true);
    }

    plt->setAutoReplot( autoReplot );

    if ( doReplot )
        plt->replot();
}
