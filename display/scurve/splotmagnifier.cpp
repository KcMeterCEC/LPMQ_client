#include <qwt_plot.h>

#include "splotmagnifier.h"

SplotMagnifier::SplotMagnifier(QWidget * parent):
    QwtPlotMagnifier(parent)
{
    setAxisEnabled(QwtPlot::yLeft, false);
}
