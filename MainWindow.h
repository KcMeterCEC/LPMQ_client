#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "DockManager.h"

class QSettings;
class Commander;
class QMessageBox;
class QSpinBox;
class QTimer;
class QLabel;
class TargetPs;
class TargetMem;
class StatisticCurve;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QMainWindow *parent = nullptr);
    ~MainWindow();
private:
    //The main container for docking
    ads::CDockManager       *dockManager = nullptr;
    QSettings               *userCfg = nullptr;
    Commander               *cmd = nullptr;
    TargetPs                *ps = nullptr;
    TargetMem               *mem = nullptr;
    QAction                 *connectTool = nullptr;
    QMessageBox             *msg = nullptr;
    QSpinBox                *period = nullptr;
    QTimer                  *refreshTimer = nullptr;
    QLabel                  *statCpuInfo = nullptr;
    StatisticCurve          *psCurve = nullptr;
    QVector<QPolygonF>      psCurveData;
    StatisticCurve          *memCurve = nullptr;
    quint64                 timeElaspe = 0;

    void toolBarsCreate(void);
    void msgCreate(void);
    void widgetCreate(void);
    void requestConnect(void);
    void refreshConnectStatus(void);
    void msgWarning(const QString &str);
    void msgError(const QString &str);
    void msgInfo(const QString &str);
    void resetTimerPeriod(int val);
private slots:
    void connectSet(bool checked);
    void connectMsg(bool isCon, const QString &str);
    void periodChanged(int val);
    void execRequest(void);

    void showSysInfo(const QMap<QString, QString> &info);
    void showCpuUsage(const QMap<QString, double> &info);
    void showMemUsage(const QMap<QString, double> &info);
};
#endif // MAINWINDOW_H
