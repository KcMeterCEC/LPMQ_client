#ifndef SPLOTMAGNIFIER_H
#define SPLOTMAGNIFIER_H

#include <qwt_plot_magnifier.h>

class SplotMagnifier : public QwtPlotMagnifier
{
public:
    SplotMagnifier(QWidget * parent = nullptr);
};

#endif // SPLOTMAGNIFIER_H
