#ifndef SPLOTPANNER_H
#define SPLOTPANNER_H

#include <qwt_plot_panner.h>

class SplotPanner : public QwtPlotPanner
{
public:
    SplotPanner(QWidget * parent = nullptr);

    void setXRange(qreal min, qreal max);
protected:
    void moveCanvas( int dx, int dy ) override;
private:
    qreal xBottomMin = 0;
    qreal xBottomMax = 0;
};

#endif // SPLOTPANNER_H
