#ifndef STATISTICCURVE_H
#define STATISTICCURVE_H

#include <QWidget>
#include <QColor>
#include "sscaledraw.h"

class Splot;
class SplotCurve;
class Smarker;
class SscaleDraw;
class SplotPanner;
class SplotMagnifier;
class StatisticCurve : public QWidget
{
    Q_OBJECT
public:
    //! when brush is true,the curves z axis will be arranged from bottom to top.
    explicit StatisticCurve(const QString & title = "statistic curve",
                            bool brush = false,
                            QWidget *parent = nullptr);
    //! set the name and color for each curve
    void setCurvesNum(const QVector<QString> &cs);
    //! add data to each curve
    void addData(const QVector<QPolygonF> &data);
    void setAxisType(SscaleDraw::axisType x);
    void setAxisTitle(const QString &xTitle, const QString &yTitle);
    void clearCurvesData(void);
protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    Splot     *plot = nullptr;
    QVector<SplotCurve *> curves;
    QVector<QPolygonF> curvesData;
    Smarker *marker = nullptr;
    int mouseIdx = 0;
    SscaleDraw *Xscale = nullptr;
    SplotPanner *panner = nullptr;
    SplotMagnifier *magnifier = nullptr;
    bool    hasBrush = false;
    const QColor curveColor[18] =
    {
        QColor(236, 204, 104),
        QColor(123, 237, 159),
        QColor(255, 127, 80),
        QColor(112, 161, 255),
        QColor(164, 176, 190),
        QColor(255, 71, 87),
        QColor(116, 125, 140),
        QColor(255, 165, 2),
        QColor(46, 213, 115),
        QColor(255, 99, 72),
        QColor(206, 214, 224),
        QColor(30, 144, 255),
        QColor(255, 107, 129),
        QColor(83, 82, 237),
        QColor(47, 53, 66),
        QColor(55, 66, 250),
        QColor(87, 96, 111),
        QColor(223, 228, 234)
    };

    void deleteCurves(void);
    void refresh(void);
signals:

private slots:
    void mouseMovedInCanvas(const QPointF & p);
    void mouseRemovedOutCanvas(const QPointF & p);
};

#endif // STATISTICCURVE_H
