#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Commander       *cmd = nullptr;
    QLabel          *connectLab = nullptr;
    QLabel          *sysInfoStatus;
    QSettings       *userCfg;
    QTimer          *overviewTimer;
    QSpinBox        *timeAdj;
    DisPieChart     *overviewPie;
    DisLineChart    *overviewLine;

    void    disConnectStatus(void);
    void    refreshTriggerTime(int value);

    void    closeEvent(QCloseEvent *event);
private slots:
    void on_actionconnect_triggered();
    void connectReport(bool status, const QString &errStr);
    void execOverview();

    void showSysInfo(const QMap<QString, QString> &info);
    void showCpuUsage(const QMap<QString, double> &info);
    void triggerValueChanged(int value);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
