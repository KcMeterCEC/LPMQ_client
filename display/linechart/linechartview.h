#ifndef LINECHARTVIEW_H
#define LINECHARTVIEW_H

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>

QT_CHARTS_USE_NAMESPACE

class LineChart;
class MarkerLine;
class LineChartView : public QChartView
{
    Q_OBJECT
public:
    typedef enum
    {
        DATE,
        VALUE,
    }chartType;

    typedef enum
    {
        NORMAL,
        AREA,
    }lineType;

    explicit LineChartView(QWidget *parent = nullptr, const QString &title = "chart");
    void setNumOfLine(quint8 num, const QVector<QString> &name, lineType type = NORMAL);
    void setDisCount(quint16 count);
    void clearLinesData(void);
    void setChartType(chartType type);
    void saveLinesData(QVector<QVector<QPointF>> &data);
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
private:
    quint8  lineNum = 1;
    quint16 disCount = 10;
    QVector<QVector<QPointF>> linesVal;
    QVector<QLineSeries *> series;
    LineChart   *chart = nullptr;
    chartType   disType = DATE;
    lineType    linesType = NORMAL;
    qint64      strMSecs = 0;
    QDateTimeAxis timeAxis;
    QValueAxis      yAxis;
    QValueAxis      xValueAxis;
    MarkerLine      *markerLine;
    qreal           selectIndex = 0;
    qreal           selectX = 0;
    QRectF          axisRect;
    QPoint          mousePoint;
    QPoint          mouseLastPoint;
    bool            setOffset = false;
    int             disOffset = 0;

    bool clearLines(void);
    void createLines(const QVector<QString> &name);
    void refreshLines(void);
    void resetAxis(void);
    void getIndexOfPos(const QPoint &pos);
    void getXOfPos(void);
    void getAxisRect(void);
    QRectF getAxisBound(void);
    QString getXLabelAtPoint(void);
    bool seriesHasContents(void);
    void refreshMarkerLabel(void);
};

#endif // LINECHARTVIEW_H
