#ifndef SPLOTCURVE_H
#define SPLOTCURVE_H

#include <qwt_plot_curve.h>

class SplotCurve : public QwtPlotCurve
{
public:
    explicit SplotCurve(const QString &title);
};

#endif // SPLOTCURVE_H
