#ifndef SSCALEDRAW_H
#define SSCALEDRAW_H

#include <QTime>
#include <qwt_scale_draw.h>

class SscaleDraw : public QwtScaleDraw
{
public:
    typedef enum
    {
        POINT,
        TIME,
    }axisType;

    SscaleDraw(const QTime &base);

    void setAxisType(axisType x);
    virtual QwtText label( double v ) const;
private:
    axisType xType = POINT;
    QTime baseTime;
};

#endif // SSCALEDRAW_H
