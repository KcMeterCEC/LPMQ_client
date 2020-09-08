#include <QLabel>
#include <QToolBar>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QGridLayout>
#include <QSettings>
#include <QMessageBox>
#include <QSpinBox>
#include <QTimer>
#include <QStatusBar>
#include <QMenuBar>
#include <QDebug>

#include "MainWindow.h"
#include "DockAreaWidget.h"
#include "DockWidget.h"
#include "control/commander.h"
#include "control/targetps.h"
#include "display/scurve/statisticcurve.h"

MainWindow::MainWindow(QMainWindow *parent)
    : QMainWindow(parent),
      dockManager(new ads::CDockManager(this)),
      userCfg(new QSettings("kcmetercec", "lpmq")),
      msg(new QMessageBox),
      period(new QSpinBox(this)),
      refreshTimer(new QTimer(this)),
      psCurve(new StatisticCurve(tr("history of cpu usage")))
{
    setWindowIcon(QIcon(":/images/basic/monitor.png"));
    setMinimumSize(960, 540);

    cmd = Commander::getInstance();
    connect(cmd, &Commander::connectChanged, this, &MainWindow::connectMsg);
    connect(cmd, &Commander::resultSysInfo, this, &MainWindow::showSysInfo);

    ps = TargetPs::getInstance();
    connect(ps, &TargetPs::resultCpuUsage, this, &MainWindow::showCpuUsage);

    toolBarsCreate();
    msgCreate();
    widgetCreate();
}

MainWindow::~MainWindow()
{
    delete userCfg;
}

void MainWindow::toolBarsCreate(void)
{
    QToolBar *toolBar = addToolBar(tr("tools"));

    connectTool = toolBar->addAction("connect");
    refreshConnectStatus();
    connect(connectTool, &QAction::triggered, this, &MainWindow::connectSet);

    period->setValue(5);
    period->setRange(1, 60);
    period->setAlignment(Qt::AlignCenter);
    period->setToolTip(tr("The cycle of request"));
    toolBar->addWidget(period);
    connect(period, SIGNAL(valueChanged(int)), this, SLOT(periodChanged(int)));
    connect(refreshTimer, &QTimer::timeout, this, &MainWindow::execRequest);
    resetTimerPeriod(period->value());
}
void MainWindow::widgetCreate(void)
{
    QMenuBar *menu = menuBar();

    ads::CDockWidget *dockWidget = new ads::CDockWidget("PS overview");
    dockWidget->setWidget(psCurve);
    menu->addAction(dockWidget->toggleViewAction());
    psCurve->setAxisTitle("time elaspe", "%");
    psCurve->setAxisType(SscaleDraw::TIME);

    dockManager->addDockWidget(ads::TopDockWidgetArea, dockWidget);
}
void MainWindow::resetTimerPeriod(int val)
{
    refreshTimer->setInterval(val * 1000);
}
void MainWindow::periodChanged(int val)
{
    resetTimerPeriod(val);
}
void MainWindow::execRequest(void)
{
    cmd->requestCpuUsage();
}
void MainWindow::msgCreate(void)
{
    msg->setWindowIcon(QIcon(":/images/basic/monitor.png"));
    msg->setDefaultButton(QMessageBox::Ok);

    QStatusBar *stat = statusBar();
    statCpuInfo = new QLabel();
    stat->addWidget(statCpuInfo);
}
void MainWindow::msgWarning(const QString &str)
{
    msg->setIcon(QMessageBox::Warning);
    msg->setText(str);
    msg->exec();
}
void MainWindow::msgError(const QString &str)
{
    msg->setIcon(QMessageBox::Critical);
    msg->setText(str);
    msg->exec();
}
void MainWindow::msgInfo(const QString &str)
{
    msg->setIcon(QMessageBox::Information);
    msg->setText(str);
    msg->exec();
}
void MainWindow::requestConnect(void)
{
    QDialog conDialog(this);
    conDialog.setMinimumSize(192, 108);

    QLabel ipLabel(&conDialog);
    QLineEdit ipSet(&conDialog);
    QLabel portLabel(&conDialog);
    QSpinBox portSet(&conDialog);
    QDialogButtonBox button(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                            Qt::Horizontal, &conDialog);
    connect(&button, &QDialogButtonBox::accepted,
            &conDialog, &QDialog::accept);
    connect(&button, &QDialogButtonBox::rejected,
            &conDialog, &QDialog::reject);
    QGridLayout gridLayout(&conDialog);

    gridLayout.addWidget(&ipLabel, 0, 0, 1, 1);
    gridLayout.addWidget(&ipSet, 0, 1, 1, 4);
    gridLayout.addWidget(&portLabel, 1, 0, 1, 1);
    gridLayout.addWidget(&portSet, 1, 1, 1, 4);
    gridLayout.addWidget(&button, 2, 0, 1, 5);

    ipLabel.setText(tr("IP:"));
    portLabel.setText(tr("Port:"));
    portSet.setRange(1025, UINT16_MAX);
    ipSet.setPlaceholderText(tr("*.local or IP address"));
    if(userCfg->contains("serverAddr"))
    {
        ipSet.setText(userCfg->value("serverAddr").toString());
    }
    if(userCfg->contains("serverPort"))
    {
        portSet.setValue(userCfg->value("serverPort").toInt());
    }

    if(conDialog.exec() == QDialog::Accepted)
    {
        userCfg->setValue("serverAddr", ipSet.text());
        userCfg->setValue("serverPort", portSet.value());
        cmd->connect2Server(ipSet.text(), portSet.value());
    }
}
void MainWindow::connectSet(bool checked)
{
    Q_UNUSED(checked);

    if(cmd->connectStatus())
    {
        cmd->disconnectFromServer();
        refreshConnectStatus();
    }
    else
    {
        requestConnect();
    }
}
void MainWindow::showSysInfo(const QMap<QString, QString> &info)
{
    QStatusBar *stat = statusBar();

    statCpuInfo->setText(info.value("model name"));

    QString result;

    QMap<QString, QString>::const_iterator i = info.constBegin();
    while (i != info.constEnd())
    {
        result += i.key() + ": " + i.value();
        ++i;
        if(i != info.constEnd())
        {
            result += "\n";
        }
    }
    stat->setToolTip(result);
}
void MainWindow::refreshConnectStatus(void)
{
    if(cmd->connectStatus())
    {
        connectTool->setIcon(QIcon(":/images/basic/disconnect.png"));
        connectTool->setToolTip(tr("disconnect from server"));

        refreshTimer->start();
        cmd->requestSysInfo();
    }
    else
    {
        connectTool->setIcon(QIcon(":/images/basic/connect.png"));
        connectTool->setToolTip(tr("connect to server"));
    }
}
void MainWindow::connectMsg(bool isCon, const QString &str)
{
    refreshConnectStatus();

    if(!isCon)
    {
        msgError(str);
    }
}
void MainWindow::showCpuUsage(const QMap<QString, double> &info)
{
    int cpuCnt = static_cast<int>(info.value("cpu count"));
    if(psCurveData.size() != cpuCnt)
    {
        qDebug() << "set curve size " << cpuCnt;

        psCurveData.resize(cpuCnt);

        QVector<QString> lines;
        for(int i = 0; i < cpuCnt; ++i)
        {
            lines.push_back(QString("cpu%1").arg(i));
        }
        psCurve->setCurvesNum(lines);
    }
}
