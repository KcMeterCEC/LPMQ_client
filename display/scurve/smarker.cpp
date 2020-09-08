#include <QDebug>
#include <qwt_plot.h>
#include <cmath>
#include "smarker.h"

Smarker::Smarker()
{
    setZ(1000);
}
int Smarker::rtti() const
{
    return QwtPlotItem::Rtti_PlotUserItem;
}
void Smarker::draw(QPainter *painter,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF & canvasRect) const
{
    drawCenterLine(painter, canvasRect);
    drawContents(painter, canvasRect);
}
void Smarker::drawCenterLine(QPainter *painter, const QRectF & canvasRect) const
{
    qreal lineX = curveX;
    qreal lineY = canvasRect.y();
    qreal len = canvasRect.height();
    painter->save();
    painter->setPen(Qt::red);
    painter->drawLine(lineX, lineY,
                      lineX, lineY + len);
    painter->restore();
}
void Smarker::drawContents(QPainter *painter, const QRectF & canvasRect) const
{
    const quint8 gap = 10;

    painter->save();

    //! draw brief
    qreal w = canvasRect.width() / 3;
    qreal x = curveX - w / 2;
    qreal y = canvasRect.y() + gap;
    qreal h = 20;
    w /= 2;

    if(isRight)
    {
        x += w;
    }

    QRect rect(x, y, w, h);

    painter->setPen(QColor(134, 134, 134, 250));
    painter->setBrush(QColor(134, 134, 134, 250));
    painter->drawRect(rect);

    painter->setPen(QColor(255, 255, 255, 250));
    painter->drawText(rect, Qt::AlignCenter, brief);
    //! draw contents
    quint8 contentsCnt = pointContents.size();

    painter->setBrush(QColor(255, 255, 255, 255));
    for(int i = 0; i < contentsCnt; ++i)
    {
        QRect labelRect1(x, y + (i + 1) * h, w / 2, h);
        QRect labelRect2(x + labelRect1.width(), y + (i + 1) * h, w / 2, h);

        painter->setPen(std::get<0>(pointContents[i]));

        painter->drawRect(labelRect1);
        painter->drawText(labelRect1, Qt::AlignCenter, std::get<1>(pointContents[i]));

        painter->drawRect(labelRect2);
        painter->drawText(labelRect2, Qt::AlignCenter,
                          QString("%1").arg(std::get<2>(pointContents[i])));
    }

    painter->restore();
}
void  Smarker::redraw(qreal x, const QString &name, const QVector<std::tuple<QColor, QString, qreal> > &contents)
{
    if(x > 0)
    {
        isRight = false;
    }
    else
    {
        isRight = true;
    }
    curveX = std::fabs(x);
    brief = name;

    pointContents.clear();
    pointContents = contents;
}
