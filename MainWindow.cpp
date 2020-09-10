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
#include <QPolygonF>
#include <QPointF>
#include <QDebug>

#include "MainWindow.h"
#include "DockAreaWidget.h"
#include "DockWidget.h"
#include "control/commander.h"
#include "control/targetps.h"
#include "control/targetmem.h"
#include "display/scurve/statisticcurve.h"

MainWindow::MainWindow(QMainWindow *parent)
    : QMainWindow(parent),
      dockManager(new ads::CDockManager(this)),
      userCfg(new QSettings("kcmetercec", "lpmq")),
      msg(new QMessageBox),
      period(new QSpinBox(this)),
      refreshTimer(new QTimer(this)),
      psCurve(new StatisticCurve(tr("history of cpu usage"))),
      memCurve(new StatisticCurve(tr("history of memory usage"), true))
{
    setWindowIcon(QIcon(":/images/basic/monitor.png"));
    setMinimumSize(960, 540);

    cmd = Commander::getInstance();
    connect(cmd, &Commander::connectChanged, this, &MainWindow::connectMsg);
    connect(cmd, &Commander::resultSysInfo, this, &MainWindow::showSysInfo);

    ps = TargetPs::getInstance();
    connect(ps, &TargetPs::resultCpuUsage, this, &MainWindow::showCpuUsage);

    mem = TargetMem::getInstance();
    connect(mem, &TargetMem::resultMemUsage, this, &MainWindow::showMemUsage);

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

    connectTool = toolBar->addAction(tr("connect"));
    refreshConnectStatus();
    connect(connectTool, &QAction::triggered, this, &MainWindow::connectSet);

    clearTool = toolBar->addAction(tr("clear data of curves"));
    clearTool->setIcon(QIcon(":/images/basic/clear.png"));
    connect(clearTool, &QAction::triggered, this, &MainWindow::clearCurves);

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

    ads::CDockWidget *psDockWidget = new ads::CDockWidget(tr("PS overview"));
    psDockWidget->setWidget(psCurve);
    menu->addAction(psDockWidget->toggleViewAction());
    psCurve->setAxisTitle(tr("time elaspe"), "%");
    psCurve->setAxisType(SscaleDraw::TIME);

    dockManager->addDockWidget(ads::TopDockWidgetArea, psDockWidget);

    ads::CDockWidget *memDockWidget = new ads::CDockWidget(tr("Memory overview"));
    memDockWidget->setWidget(memCurve);
    menu->addAction(memDockWidget->toggleViewAction());

    QVector<QString> lines({"used", "buffers", "cached", "free"});
    memCurve->setCurvesNum(lines);
    memCurve->setAxisTitle("time elaspe", "MByte");
    memCurve->setAxisType(SscaleDraw::TIME);

    dockManager->addDockWidget(ads::BottomDockWidgetArea, memDockWidget);
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
    timeElaspe += refreshTimer->interval() / 1000;
    cmd->requestCpuUsage();
    cmd->requestMemUsage();
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
void MainWindow::clearCurves(void)
{
    psCurve->clearCurvesData();
    memCurve->clearCurvesData();
    timeElaspe = 0;
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
        psCurveData.resize(cpuCnt);

        QVector<QString> lines;
        for(int i = 0; i < cpuCnt; ++i)
        {
            psCurveData[i].resize(1);
            lines.push_back(QString("cpu%1").arg(i));
        }
        psCurve->setCurvesNum(lines);
    }

    for(int i = 0; i < cpuCnt; ++i)
    {
        psCurveData[i][0] = QPointF(timeElaspe,
                                    info.value(QString("cpu%1.usage").arg(i))
                                    );
    }
    psCurve->addData(psCurveData);
}
void MainWindow::showMemUsage(const QMap<QString, double> &info)
{
    QVector<QPolygonF> usage;
    usage.resize(4);

    usage[0].push_back(QPointF(timeElaspe, info.value("mem.used")));
    usage[1].push_back(QPointF(timeElaspe,
    info.value("mem.used") + info.value("mem.buffers")));
    usage[2].push_back(QPointF(timeElaspe,
    info.value("mem.used") + info.value("mem.buffers") + info.value("mem.cache")));
    usage[3].push_back(QPointF(timeElaspe,
    info.value("mem.total")));

    memCurve->addData(usage);
}
