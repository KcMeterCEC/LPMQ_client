#ifndef SPLOT_H
#define SPLOT_H

#include <qwt_plot.h>

class QwtPlotPicker;
class SplotGrid;
class Splot : public QwtPlot
{
    Q_OBJECT
public:
    explicit Splot(const QwtText &title, QWidget *parent = nullptr);
private:
    QwtPlotPicker *mousePicker = nullptr;
    SplotGrid   *grid = nullptr;
signals:
    void mouseMoved(const QPointF & p);
    void mouseRemoved(const QPointF & p);
};

#endif // SPLOT_H
