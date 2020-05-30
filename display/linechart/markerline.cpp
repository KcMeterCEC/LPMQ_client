#include <QPainter>
#include <QDebug>
#include "markerline.h"

MarkerLine::MarkerLine(QGraphicsItem *parent):
    QGraphicsItem(parent)
{
    changeGeo(QRectF(0, 0, 20, 20));
}
QRectF MarkerLine::boundingRect() const
{
    qreal penWidth = 1;
    qreal x= geo.x() -  penWidth / 2;
    qreal y = geo.y() -  penWidth / 2;
    qreal w = geo.width() + penWidth / 2;
    qreal h = geo.height() + penWidth / 2;

//    qDebug() << "return markerline: x-> " << x
//             << " y-> " << y
//             << " w-> " << w
//             << " h-> " << h;

    return QRectF(x, y, w, h);
}
void MarkerLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
//    painter->setBrush(QColor(100, 100, 100, 255));
//    painter->drawRect(geo);

    //draw center line
    qreal lineX = geo.x() + geo.width() / 2;
    qreal lineY = geo.y();

    painter->setPen(Qt::red);
    painter->drawLine(lineX,
                      geo.y(),
                      lineX,
                      geo.y() + geo.height());
    drawContents(briefSide, painter, QPoint(lineX, lineY));
}
void MarkerLine::drawContents(bool isRight, QPainter *painter, const QPoint &str)
{
    quint8 gap = 10;
    painter->save();
    //draw brief
    qreal lineX = str.x();
    qreal lineY = str.y();
    qreal x = lineX + gap;
    qreal y = lineY + gap;
    qreal w = (geo.width() - 3 * gap) / 2;
    qreal h = 20;
    if(!isRight)
    {
        x = x - 2 * gap - w;
    }

    QRect rect(x, y, w, h);

    painter->setPen(QColor(134, 134, 134, 250));
    painter->setBrush(QColor(134, 134, 134, 250));
    painter->drawRect(rect);

    painter->setPen(QColor(255, 255, 255, 250));
    painter->drawText(rect, Qt::AlignCenter, brief);
    //draw contents
    quint8 contentsCnt = pointContents.size();

    painter->setBrush(QColor(255, 255, 255, 255));
    for(int i = 0; i < contentsCnt; ++i)
    {
        QRect labelRect1(x, y + (i + 1) * h, w * 2 / 4, h);
        QRect labelRect2(x + labelRect1.width(), y + (i + 1) * h, w * 2 / 4, h);

        painter->setPen(std::get<0>(pointContents[i]));

        painter->drawRect(labelRect1);
        painter->drawText(labelRect1, Qt::AlignCenter, std::get<1>(pointContents[i]));

        painter->drawRect(labelRect2);
        painter->drawText(labelRect2, Qt::AlignCenter,
                          QString("%1").arg(std::get<2>(pointContents[i])));
    }

    painter->restore();
}
void MarkerLine::changeGeo(const QRectF &rect)
{
    prepareGeometryChange();
    qreal x = rect.x();
    qreal y = rect.y();
    qreal w = rect.width();
    qreal h = rect.height();

    w = w / 3;
    x = x - w / 2;
    geo.setRect(x, y,  w,  h);
}
void MarkerLine::changeX(qreal x)
{
    x = x - geo.width() / 2;
    geo.setRect(x, geo.y(), geo.width(), geo.height());
    prepareGeometryChange();
}
void MarkerLine::setBrief(const QString &str)
{
    brief = str;
}
void MarkerLine::setBriefSide(bool isRight)
{
    briefSide = isRight;
}
void MarkerLine::setContents(const QVector<std::tuple<QColor, QString, qreal>> &contents)
{
    pointContents.clear();
    pointContents = contents;
}
