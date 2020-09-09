#include <QDebug>

#include <qwt_plot.h>
#include <qwt_scale_div.h>
#include "splotpanner.h"

SplotPanner::SplotPanner(QWidget *parent):
    QwtPlotPanner(parent)
{
    setOrientations(Qt::Horizontal);
}
void SplotPanner::setXRange(qreal min, qreal max)
{
    xBottomMin = min;
    xBottomMax = max;
}
void SplotPanner::moveCanvas( int dx, int dy )
{
    if ( dx == 0 && dy == 0 )
        return;

    QwtPlot *plot = this->plot();
    if ( plot == NULL )
        return;

    const bool doAutoReplot = plot->autoReplot();
    plot->setAutoReplot( false );

    const QwtScaleMap map = plot->canvasMap( QwtPlot::xBottom );

    const double p1 = map.transform( plot->axisScaleDiv( QwtPlot::xBottom ).lowerBound() );
    const double p2 = map.transform( plot->axisScaleDiv( QwtPlot::xBottom ).upperBound() );

    double d1, d2;

    d1 = map.invTransform( p1 - dx );
    d2 = map.invTransform( p2 - dx );

    if(!initialized)
    {
        initialized = true;

        d1Tmp = d1;
        d2Tmp = d2;
    }

    if(d1 < xBottomMin)
    {
        qreal diff = xBottomMin - d1Tmp;

        d1 = xBottomMin;
        d2 = d2Tmp + diff;
    }
    if(d2 > xBottomMax)
    {
        qreal diff = xBottomMax - d2Tmp;

        d2 = xBottomMax;
        d1 = d1Tmp + diff;
    }

    d1Tmp = d1;
    d2Tmp = d2;

    plot->setAxisScale( QwtPlot::xBottom, d1, d2 );

    plot->setAutoReplot( doAutoReplot );
    plot->replot();
}
