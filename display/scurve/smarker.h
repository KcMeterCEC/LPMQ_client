#ifndef SMARKER_H
#define SMARKER_H

#include <qwt_plot_item.h>

class Smarker : public QwtPlotItem
{
public:
    Smarker();

    virtual int rtti() const;

    virtual void draw( QPainter *painter, const QwtScaleMap &xMap,
                       const QwtScaleMap &yMap, const QRectF & canvasRect) const;
    void redraw(qreal x, const QString &name,
                const QVector<std::tuple<QColor, QString, qreal>> &contents);
private:
    qreal curveX = 0;
    bool  isRight = false;
    QString brief;
    QVector<std::tuple<QColor, QString, qreal>> pointContents;

    void drawCenterLine(QPainter *painter, const QRectF & canvasRect) const;
    void drawContents(QPainter *painter, const QRectF & canvasRect) const;
};

#endif // SMARKER_H
