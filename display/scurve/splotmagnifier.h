#ifndef SPLOTMAGNIFIER_H
#define SPLOTMAGNIFIER_H

#include <qwt_plot_magnifier.h>

class SplotMagnifier : public QwtPlotMagnifier
{
public:
    SplotMagnifier(QWidget * parent = nullptr);

    void setXRange(qreal min, qreal max);
protected:
    void rescale( double factor ) override;
private:
    qreal xBottomMin = 0;
    qreal xBottomMax = 0;
};

#endif // SPLOTMAGNIFIER_H
