#include <QGridLayout>
#include <QMap>
#include <QPolygonF>
#include <QPoint>
#include <QMouseEvent>
#include <QDebug>
#include "statisticcurve.h"
#include "splot.h"
#include "splotcurve.h"
#include "smarker.h"
#include "sscaledraw.h"
#include "splotpanner.h"
#include "splotmagnifier.h"
#include "slegend.h"

StatisticCurve::StatisticCurve(const QString &title, bool brush, QWidget *parent) :
    QWidget(parent),
    plot(new Splot(title, this)),
    hasBrush(brush),
    marker (new Smarker()),
    Xscale (new SscaleDraw(QTime(0, 0, 0)))
{
    marker->attach(plot);
    plot->setAxisScaleDraw( QwtPlot::xBottom, Xscale);
    plot->insertLegend(new Slegend(), QwtPlot::RightLegend);

    panner = new SplotPanner(plot->canvas());
    panner->setCursor(Qt::ClosedHandCursor);

    magnifier = new SplotMagnifier(plot->canvas());

    connect(plot, &Splot::mouseMoved, this, &StatisticCurve::mouseMovedInCanvas);
    connect(plot, &Splot::mouseRemoved, this, &StatisticCurve::mouseRemovedOutCanvas);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(plot, 0, 0);

    setLayout(layout);

    setMouseTracking(true);
}
void StatisticCurve::setCurvesNum(const QVector<QString> &cs)
{
    deleteCurves();
    for(int i = 0; i < cs.size(); ++i)
    {
        SplotCurve *curve = new SplotCurve(cs[i]);
        QColor color = curveColor[i %
                (sizeof(curveColor) / sizeof(curveColor[0]))];
        curve->setPen(color);
        if(hasBrush)
        {
            curve->setBrush(color);
            curve->setZ(curve->z() - i);
        }
        curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);

        curves.push_back(curve);
        curve->attach(plot);
    }

    curvesData.resize(cs.size());
}
void StatisticCurve::addData(const QVector<QPolygonF> &data)
{
    if(!curves.size())
    {
        return;
    }
    for(int i = 0; i < data.size(); ++i)
    {
        for(int j = 0; j < data[i].size(); ++j)
        {
            curvesData[i].push_back(data[i].at(j));
        }
        curves[i]->setSamples(curvesData[i]);
    }
}
void StatisticCurve::clearCurvesData(void)
{
    if(curvesData.size())
    {
        for(int i = 0; i < curvesData.size(); ++i)
        {
            curvesData[i].clear();
        }
    }
    refresh();
}
void StatisticCurve::deleteCurves(void)
{
    clearCurvesData();
    if(curves.size())
    {
        for(int i = 0; i < curves.size(); ++i)
        {
            curves[i]->detach();
        }
        curves.clear();
    }
}
void StatisticCurve::mouseMovedInCanvas(const QPointF & p)
{
    if(!curvesData.size() || !curvesData[0].size())
    {
        return;
    }
    marker->show();

    qreal currentX = p.x();
    QPolygonF &curveData = curvesData[0];
    mouseIdx = curveData.size() - 1;
    for(int i = 0; i < curveData.size() - 1; ++i)
    {
        if(curveData[i + 1].rx() > currentX)
        {
            mouseIdx = i;

            if((curveData[i + 1].rx() - currentX) <
                    (currentX - curveData[i].rx()))
            {
                mouseIdx += 1;
            }
            break;
        }
    }


    refresh();
}
void StatisticCurve::mouseRemovedOutCanvas(const QPointF & p)
{
    Q_UNUSED(p);

    marker->hide();
    refresh();
}
void StatisticCurve::refresh(void)
{
    if(curvesData.size() && curvesData[0].size())
    {
        qreal x = curvesData[0][mouseIdx].rx();
        qreal drawX = plot->canvasMap(Splot::xBottom).transform(x);
        QString name = Xscale->label(x).text();

        //display contents at left
        if(mouseIdx < curvesData[0].size() / 2)
        {
            drawX = 0 - drawX;
        }

        QVector<std::tuple<QColor, QString, qreal>> contents;

        for(int i = 0; i < curves.size(); ++i)
        {
            QColor color = curves[i]->pen().color();
            QString name = curves[i]->title().text();
            qreal   val = curves[i]->sample(mouseIdx).ry();

            contents.push_back(std::tuple<QColor, QString, qreal>(color, name, val));
        }
        marker->redraw(drawX, name, contents);
    }
    plot->replot();
}
void StatisticCurve::setAxisTitle(const QString &xTitle, const QString &yTitle)
{
    plot->setAxisTitle(QwtPlot::xBottom, xTitle);
    plot->setAxisTitle(QwtPlot::yLeft, yTitle);
}
void StatisticCurve::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    refresh();
}
void StatisticCurve::setAxisType(SscaleDraw::axisType x)
{
    Xscale->setAxisType(x);
}
