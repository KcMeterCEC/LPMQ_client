#include <QDateTime>
#include <QDebug>
#include <QMouseEvent>
#include <QLegendMarker>
#include <QAreaSeries>

#include "linechartview.h"
#include "linechart.h"
#include "markerline.h"

LineChartView::LineChartView(QWidget *parent, const QString &title):
    QChartView(parent), disCount(dispMin)
{
    chart = new LineChart();
    Q_CHECK_PTR(chart);
    chart->setTitle(title);

    markerLine = new MarkerLine(chart);
    Q_CHECK_PTR(markerLine);
    markerLine->hide();
    markerLine->setZValue(11);

    setChart(chart);
    QVector<QString> name = {"line0"};
    createLines(name);

    timeAxis.setFormat("dd h:mm:ss");
    timeAxis.setTickCount(10);
    xValueAxis.setMax(100);

    setRenderHint(QPainter::Antialiasing);
}
QRectF LineChartView::getAxisBound(void)
{
    getAxisRect();
    getXOfPos();

    return QRectF(selectX, axisRect.y(),
                  axisRect.width(),
                  axisRect.height());
}
void LineChartView::getAxisRect(void)
{
    QPointF leftTop;
    QPointF rightBottom;

    if(disType == DATE)
    {
        leftTop = chart->mapToPosition(QPointF(timeAxis.min().toMSecsSinceEpoch(),
                                               yAxis.max()));
        rightBottom = chart->mapToPosition(QPointF(timeAxis.max().toMSecsSinceEpoch(),
                                                   yAxis.min()));
    }
    else
    {
        leftTop = chart->mapToPosition(QPointF(xValueAxis.min(),
                                               yAxis.max()));
        rightBottom = chart->mapToPosition(QPointF(xValueAxis.max(),
                                                   yAxis.min()));
    }
    axisRect.setRect(leftTop.rx(), leftTop.ry(),
                     rightBottom.rx() - leftTop.rx(),
                     rightBottom.ry() - leftTop.ry());
}
void LineChartView::resizeEvent(QResizeEvent *event)
{
    markerLine->changeGeo(getAxisBound());

    QChartView::resizeEvent(event);
}
void LineChartView::setNumOfLine(quint8 num, const QVector<QString> &name, lineType type)
{
    linesType = type;
    if(num != lineNum)
    {
        lineNum = num;

        createLines(name);
    }
}
void LineChartView::setChartType(chartType type)
{
    if(type > VALUE)
    {
        type = VALUE;
    }
    disType = type;

    resetAxis();
}
void LineChartView::resetAxis(void)
{
    QList<QAbstractAxis *> axisList = chart->axes();
    if(axisList.size())
    {
        for(auto axis : axisList)
        {
            chart->removeAxis(axis);
        }
    }

    QAbstractAxis *axis = &timeAxis;
    if(disType == VALUE)
    {
        axis = &xValueAxis;
    }
    chart->addAxis(axis, Qt::AlignBottom);
    chart->addAxis(&yAxis, Qt::AlignLeft);

    if(series.size())
    {
        for(auto serie : chart->series())
        {
            serie->attachAxis(axis);
            serie->attachAxis(&yAxis);
        }
    }

}
void LineChartView::clearLinesData(void)
{
    clearLines();
    if(linesVal.size())
    {
        linesVal.clear();
    }
    //reset date start time
    strMSecs = QDateTime::currentDateTime().toMSecsSinceEpoch();

    disOffset = 0;
}
void LineChartView::createLines(const QVector<QString> &name)
{
    //remove all data of old series
    clearLinesData();
    //remove all old series
    chart->removeAllSeries();
    series.clear();

    for(int i = 0; i < lineNum; ++i)
    {
        QLineSeries *line = new QLineSeries();
        line->setName(name[i]);

        series.push_back(line);

        if(linesType == AREA)
        {
            QAreaSeries *area = nullptr;
            if(i >= 1)
            {
                area = new QAreaSeries(line, series[i - 1]);
            }
            else
            {
                area = new QAreaSeries(line);
            }
            area->setName(name[i]);
            chart->addSeries(area);
            line->setColor(area->color());
        }
        else
        {
            chart->addSeries(line);
        }
    }

    resetAxis();
}
void LineChartView::saveLinesData(QVector<QVector<QPointF>> &data)
{
    if(!linesVal.size())
    {
        linesVal.resize(data.size());
    }
    for(int i = 0; i < data.size(); ++i)
    {
        for(int j = 0; j < data[i].size(); ++j)
            linesVal[i].push_back(data[i].at(j));
    }
    refreshLines();
}
bool LineChartView::clearLines(void)
{
    if(!series.size()) return false;

    for(auto line : series)
    {
        line->clear();
    }

    return true;
}
void LineChartView::refreshLines(void)
{
    if(!clearLines()) return;

    quint16 lineLen = disCount < linesVal[0].size() ? disCount : linesVal[0].size();
    quint16 strIndex = linesVal[0].size() - lineLen + disOffset;
    qreal yMax = 0;

    for(int j = 0; j < linesVal.size(); ++j)
    {
        QList<QPointF> seriesPoint;
        for(int i = strIndex; i < linesVal[0].size(); ++i)
        {
            if(disType == DATE)
            {
                seriesPoint.push_back(QPointF(
                                          strMSecs + linesVal[j].at(i).x() * 1000,
                                          linesVal[j].at(i).y()
                                          ));
            }
            else
            {
                seriesPoint.push_back(linesVal[j].at(i));
            }

            if(yMax < linesVal[j].at(i).y())
            {
                yMax = linesVal[j].at(i).y();
            }
        }
        series[j]->append(seriesPoint);
    }
    yAxis.setMax(yMax);

    if(disType == DATE)
    {
        timeAxis.setMax(QDateTime::fromMSecsSinceEpoch(series[0]->at(lineLen - 1).x()));
        timeAxis.setMin(QDateTime::fromMSecsSinceEpoch(series[0]->at(0).x()));
    }
    else
    {
        xValueAxis.setMax(series[0]->at(lineLen - 1).x());
        xValueAxis.setMin(series[0]->at(0).x());
    }

    refreshMarkerLabel();
}
void LineChartView::setDisCount(quint16 count)
{
    disCount = count;
}
bool LineChartView::seriesHasContents(void)
{
    bool ret = false;

    if((series.size()) && (series[0]->pointsVector().size()))
    {
        ret = true;
    }

    return ret;
}
void LineChartView::getXOfPos(void)
{
    if(seriesHasContents() && (selectIndex < series[0]->pointsVector().size()))
    {
        selectX = chart->mapToPosition(series[0]->pointsVector()[selectIndex]).rx();
    }
}
void LineChartView::getIndexOfPos(const QPoint &pos)
{
    QLineSeries *line = series[0];

    QVector<QPointF> points = line->pointsVector();
    QPointF chartVal = chart->mapToValue(pos, chart->series()[0]);

    quint16 index = 0;
    for(;index < points.size() - 1; ++index)
    {
        if((chartVal.rx() >= points[index].rx()) &&
           (chartVal.rx() <= points[index + 1].rx()))
        {
            break;
        }
    }
    if(index < points.size() - 1)
    {
        qreal left = chartVal.rx() - points[index].rx();
        qreal right = points[index + 1].rx() - chartVal.rx();
        if(right < left)
        {
            index += 1;
        }
    }
    selectIndex = index;

    getXOfPos();
}
QString LineChartView::getXLabelAtPoint(void)
{
    QString brief;
    if(disType == DATE)
    {
        QDateTime content(QDateTime::fromMSecsSinceEpoch(series[0]->pointsVector()[selectIndex].rx()));
        brief = content.toString("dd hh:mm:ss");
    }
    else
    {
        brief = QString("%1").arg(series[0]->pointsVector()[selectIndex].rx());
    }

    return brief;
}
void LineChartView::refreshMarkerLabel(void)
{
    getAxisRect();
    if((axisRect.contains(mousePoint)) && (seriesHasContents()))
    {
        getIndexOfPos(mousePoint);
        bool labelSide = true;
        if((mousePoint.rx() - axisRect.x()) > (axisRect.width() / 2))
        {
            labelSide = false;
        }
        markerLine->setBriefSide(labelSide);

        markerLine->setBrief(getXLabelAtPoint());

        QVector<std::tuple<QColor, QString, qreal>> contents;

        for(int i = 0; i < series.size(); ++i)
        {
            QColor color = series[i]->color();
            QString name = series[i]->name();

            QVector<QPointF> points = series[i]->pointsVector();
            qreal   val = points[selectIndex].ry();
            if((i >= 1) && (linesType == AREA))
            {
                val -= series[i - 1]->pointsVector()[selectIndex].ry();
            }

            contents.push_back(std::tuple<QColor, QString, qreal>(color, name, val));
        }
        markerLine->setContents(contents);
        markerLine->changeX(selectX);
        markerLine->show();
    }
    else
    {
        markerLine->hide();
    }
}
void LineChartView::mouseMoveEvent(QMouseEvent *event)
{
    mousePoint = event->pos();
    markerLine->changeGeo(getAxisBound());
    refreshMarkerLabel();

    if(setOffset)
    {
        bool refresh = false;

        if((mousePoint.rx() - mouseLastPoint.rx()) > 0)
        {
            if((linesVal[0].size() - disCount + disOffset) > 0)
            {
                --disOffset;
                refresh = true;
            }
        }
        else
        {
            if(++disOffset <= 0)
            {
                refresh = true;
            }
            else
            {
                disOffset = 0;
            }
        }

        if(refresh)
        {
            refreshLines();
        }

        mouseLastPoint = mousePoint;
    }
}
void LineChartView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        setCursor(Qt::ClosedHandCursor);

        if(linesVal.size())
        {
            setOffset = true;
            mouseLastPoint = event->pos();
        }
    }
}
void LineChartView::mouseReleaseEvent(QMouseEvent *event)
{
    setCursor(Qt::ArrowCursor);
    setOffset = false;
}
void LineChartView::wheelEvent(QWheelEvent *event)
{
    if(!seriesHasContents()) return;

    QPoint degress = event->angleDelta() / 8;
    bool isForward = degress.ry() > 0 ? true : false;

    bool refresh = false;
    if(isForward)
    {
        if(--disCount >= dispMin)
        {
            refresh = true;
        }
        else
        {
            disCount = dispMin;
        }
    }
    else
    {
        bool recal = true;
        while((linesVal[0].size() - disCount + disOffset) <= 0)
        {
            if(disOffset < 0)
            {
                disOffset += 1;
            }
            else
            {
                recal = false;
                break;
            }
        }

        if(recal)
        {
            if(++disCount <= dispMax)
            {
                refresh = true;
            }
            else
            {
                disCount = dispMax;
            }
        }
    }

    if(refresh)
    {
        refreshLines();
    }

    event->accept();
}
