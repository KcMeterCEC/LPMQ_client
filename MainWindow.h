#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "DockManager.h"

class QSettings;
class Commander;
class QMessageBox;
class QSpinBox;
class QTimer;
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
    QAction                 *connectTool = nullptr;
    QMessageBox             *msg = nullptr;
    QSpinBox                *period = nullptr;
    QTimer                  *refreshTimer = nullptr;

    void toolBarsCreate(void);
    void msgCreate(void);
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
};
#endif // MAINWINDOW_H
