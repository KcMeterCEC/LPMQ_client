#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFormLayout>
#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QDebug>
#include <QMessageBox>
#include <QLabel>
#include <QSettings>
#include <QTimer>
#include <QSpinBox>
#include <QGridLayout>
#include <QMouseEvent>

#include "commander.h"
#include "display/donutbreakdown/dispiechart.h"
#include "display/tasklist/tasklist.h"
#include "display/linechart/linechartview.h"

QT_CHARTS_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/images/monitor.png"));

    cmd = new Commander(this);
    Q_CHECK_PTR(cmd);

    connect(cmd, &Commander::connectStatus,
            this, &MainWindow::connectReport);

    connect(cmd, &Commander::resultSysInfo,
            this, &MainWindow::showSysInfo);

    connect(cmd, &Commander::psResultCpuUsage,
            this, &MainWindow::showCpuUsage);

    connect(cmd, &Commander::memResultMemUsage,
            this, &MainWindow::showMemUsage);

    connectLab = new QLabel(this);
    Q_CHECK_PTR(connectLab);
    ui->statusbar->addPermanentWidget(connectLab);

    sysInfoStatus = new QLabel(this);
    Q_CHECK_PTR(sysInfoStatus);

    userCfg = new QSettings("kcmetercec","lpmq");
    Q_CHECK_PTR(userCfg);

    overviewTimer = new QTimer(this);
    Q_CHECK_PTR(overviewTimer);
    connect(overviewTimer, &QTimer::timeout,
            this, &MainWindow::execOverview);

    timeAdj = new QSpinBox(this);
    Q_CHECK_PTR(timeAdj);
    timeAdj->setValue(5);
    timeAdj->setMinimum(1);
    timeAdj->setMaximum(60);
    timeAdj->setPrefix(tr("period "));
    timeAdj->setSuffix(" s");
    connect(timeAdj, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::triggerValueChanged);
    ui->toolBar->addWidget(timeAdj);

    overviewPie = new DisPieChart(this);
    Q_CHECK_PTR(overviewPie);

    psChart = overviewPie->createPsChart();
    memChart = overviewPie->createMemChart();

    ui->gridLayout->addWidget(psChart, 0, 0, 1, 5);
    ui->gridLayout->addWidget(memChart, 1, 0, 1, 5);

    psLineChart = new LineChartView(this, "history usage of cpu");
    psLineChart->setDisCount(100);
    ui->gridLayout->addWidget(psLineChart, 0, 5, 1, 10);

    memLineChart = new LineChartView(this, "history usage of memory(MB)");
    memLineChart->setDisCount(100);

    QVector<QString> name = {"used", "buffers", "cached", "free"};

    memLineChart->setNumOfLine(name.size(), name, LineChartView::AREA);
    ui->gridLayout->addWidget(memLineChart, 1, 5, 1, 10);

    taskOverview = new TaskList(nullptr, cmd);
    Q_CHECK_PTR(taskOverview);

    disConnectStatus();

    qDebug() << "sizeof cmd is " << sizeof(Header);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete userCfg;
}
void    MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)

    cmd->disconnect2Server();
}


void MainWindow::on_actionconnect_triggered()
{
    if(!cmd->isConnect())
    {
        QDialog conDialog(this);
        QFormLayout layout(&conDialog);

        QString ip(" IP: ");
        QLineEdit  *ipInput = new QLineEdit(&conDialog);

        if(userCfg->contains("serverAddr"))
        {
            ipInput->setText(userCfg->value("serverAddr").toString());
        }
        else
        {
            ipInput->setPlaceholderText(tr("*.local or IP address"));
        }

        layout.addRow(ip, ipInput);

        QString port(tr("Port:"));
        QSpinBox   *portInput = new QSpinBox(&conDialog);
        portInput->setRange(1024, UINT16_MAX);

        if(userCfg->contains("serverPort"))
        {
            portInput->setValue(userCfg->value("serverPort").toInt());
        }
        layout.addRow(port, portInput);

        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                   Qt::Horizontal, &conDialog);
        layout.addRow(&buttonBox);

        QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &conDialog, &QDialog::accept);
        QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &conDialog, &QDialog::reject);

        if(conDialog.exec() == QDialog::Accepted)
        {
            userCfg->setValue("serverAddr", ipInput->text());
            userCfg->setValue("serverPort", portInput->value());
            cmd->connect2Server(ipInput->text(),
                                portInput->value());
        }
    }
    else
    {
        cmd->disconnect2Server();
        disConnectStatus();
    }
}
void    MainWindow::disConnectStatus(void)
{
    if(cmd->isConnect())
    {
        ui->actionconnect->setIcon(QIcon(":/images/images/unlink.png"));
        ui->actionconnect->setText(tr("disconnect"));
        connectLab->setText(tr("connected"));
        connectLab->setStyleSheet("color: rgb(51, 255, 51);");

        refreshTriggerTime(timeAdj->value());
        overviewTimer->start(timeAdj->value() * 1000);

    }
    else
    {
        ui->actionconnect->setIcon(QIcon(":/images/images/link.png"));
        ui->actionconnect->setText(tr("connect"));
        connectLab->setText(tr("disconnected"));
        connectLab->setStyleSheet("color: rgb(255, 0, 0);");

        overviewTimer->stop();
    }
}
void MainWindow::connectReport(bool status, const QString &errStr)
{
    qDebug() << "status " << status;

    if(status)
    {
        disConnectStatus();
        cmd->requestSysInfo();
        cmd->requestCpuUsage();
    }
    else
    {
        qDebug() << "err " << errStr;
        QMessageBox::warning(this,
                             tr("connection error"),
                             errStr,
                             QMessageBox::Ok);

        cmd->disconnect2Server();
        disConnectStatus();
    }
}
void  MainWindow::showSysInfo(const QMap<QString, QString> &info)
{
    QString result;

    QMap<QString, QString>::const_iterator i = info.constBegin();
    while (i != info.constEnd()) {
        result += i.key() + ": " + i.value() + "\n";
        ++i;
    }
    sysInfoStatus->setToolTip(result);

    sysInfoStatus->setText(info.value("model name"));
    ui->statusbar->addWidget(sysInfoStatus);
}
void MainWindow::execOverview()
{
    currentSec += timeAdj->value();

    cmd->requestCpuUsage();
    cmd->requestMemUsage();
    taskOverview->execTaskList();
}
void MainWindow::refreshTriggerTime(int value)
{
    overviewTimer->setInterval(value * 1000);
}
void MainWindow::triggerValueChanged(int value)
{
    refreshTriggerTime(value);
}
void  MainWindow::refreshCpuUsage(const QMap<QString, double> &info)
{
    QVector<QVector<QPointF>> usage;

    usage.resize(info.value("cpu count"));
    for(int i = 0; i < usage.size(); ++i)
    {
        usage[i].push_back(QPointF(currentSec, info.value(QString("cpu%1.usage").arg(i))));
    }
    psLineChart->saveLinesData(usage);
}
void  MainWindow::refreshMemUsage(const QMap<QString, double> &info)
{
    QVector<QVector<QPointF>> usage;
    usage.resize(4);

    usage[0].push_back(QPointF(currentSec, info.value("mem.used")));
    usage[1].push_back(QPointF(currentSec,
    info.value("mem.used") + info.value("mem.buffers")));
    usage[2].push_back(QPointF(currentSec,
    info.value("mem.used") + info.value("mem.buffers") + info.value("mem.cache")));
    usage[3].push_back(QPointF(currentSec,
    info.value("mem.total")));

    memLineChart->saveLinesData(usage);
}
void  MainWindow::showCpuUsage(const QMap<QString, double> &info)
{
    QVector<QString> name;

    name.resize(info.value("cpu count"));
    for(int i = 0; i < name.size(); ++i)
    {
        name[i] = QString("cpu%1").arg(i);
    }
    psLineChart->setNumOfLine(name.size(), name);

    refreshCpuUsage(info);
    overviewPie->refreshPsChart(info);

}
void MainWindow::showMemUsage(const QMap<QString, double> &info)
{
    overviewPie->refreshMemChart(info);
    refreshMemUsage(info);
}

void MainWindow::on_actionclear_triggered()
{
    psLineChart->clearLinesData();
    memLineChart->clearLinesData();
}
void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
//    if(psChart->underMouse() || psLine->underMouse())
//    {
//        qDebug() << "we need task list!";

//        taskOverview->show();
//    }
//    else if(memChart->underMouse() || memLine->underMouse())
//    {
//        qDebug() << "we need detail of memory!";
//    }
}
