#ifndef DISLINECHART_H
#define DISLINECHART_H

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QSplineSeries>
#include <QValueAxis>
#include <QObject>

QT_CHARTS_USE_NAMESPACE

class DisLineChart : public QObject
{
    Q_OBJECT
public:
    explicit DisLineChart(QObject *parent = nullptr);

    QChartView *createPsChart(void);
    void insertPsChart(quint8 cpuNum);
    bool  psLineCreated(void);
    void  refreshPsChart(const QMap<QString, double> &info);

private:
    QVector <QLineSeries *> psLine;
    QChart *psLineChart;
    bool  hasPsLine = false;
    QValueAxis *psAxisX;
    QValueAxis *psAxisY;
    qreal       yMax = 0;
signals:
private slots:
    void       psPointAdded(int index);
};

#endif // DISLINECHART_H
