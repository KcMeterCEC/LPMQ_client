#include "splotpanner.h"

SplotPanner::SplotPanner(QWidget *parent):
    QwtPlotPanner(parent)
{
    setOrientations(Qt::Horizontal);
}
