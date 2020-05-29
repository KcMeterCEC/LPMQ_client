#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChartView>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Commander;
class QLabel;
class QSettings;
class QTimer;
class QSpinBox;
class DisPieChart;
class DisLineChart;
class TaskList;
class LineChartView;

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void    mouseDoubleClickEvent(QMouseEvent *event);
private slots:
    void on_actionconnect_triggered();
    void connectReport(bool status, const QString &errStr);
    void execOverview();

    void showSysInfo(const QMap<QString, QString> &info);
    void showCpuUsage(const QMap<QString, double> &info);

    void showMemUsage(const QMap<QString, qulonglong> &info);

    void triggerValueChanged(int value);

    void on_actionclear_triggered();

private:
    Ui::MainWindow  *ui;
    Commander       *cmd = nullptr;
    QLabel          *connectLab = nullptr;
    QLabel          *sysInfoStatus = nullptr;
    QSettings       *userCfg = nullptr;
    QTimer          *overviewTimer = nullptr;
    QSpinBox        *timeAdj = nullptr;
    DisPieChart     *overviewPie = nullptr;
    DisLineChart    *overviewLine = nullptr;
    LineChartView   *psLineChart;

    QChartView      *psChart = nullptr;
    QChartView      *memChart = nullptr;
    QChartView      *memLine = nullptr;

    TaskList        *taskOverview = nullptr;

    void    disConnectStatus(void);
    void    refreshTriggerTime(int value);

    void    closeEvent(QCloseEvent *event);
};
#endif // MAINWINDOW_H
