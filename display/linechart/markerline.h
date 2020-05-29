#ifndef MARKERLINE_H
#define MARKERLINE_H

#include <QGraphicsItem>

class MarkerLine : public QGraphicsItem
{
public:
    MarkerLine(QGraphicsItem *parent = nullptr);

    void changeGeo(const QRectF &rect);
    void changeX(qreal x);
    void setBrief(const QString &str);
    void setBriefSide(bool isRight);
    void setContents(const QVector<std::tuple<QColor, QString, qreal>> &contents);
protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
private:
    QRectF  geo;
    QString brief;
    bool    briefSide = true;
    QVector<std::tuple<QColor, QString, qreal>> pointContents;

    void drawContents(bool isRight, QPainter *painter, const QPoint &str);
};

#endif // MARKERLINE_H
