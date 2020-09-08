#include "sscaledraw.h"

SscaleDraw::SscaleDraw(const QTime &base):
    baseTime(base)
{

}
QwtText SscaleDraw::label( double v ) const
{
    QString str(QString("%1").arg(v));

    if(xType == TIME)
    {
        QTime upTime = baseTime.addMSecs(static_cast<int>(v * 1000));

        str = upTime.toString();
    }
    return str;
}
void SscaleDraw::setAxisType(axisType x)
{
    xType = x;
}
