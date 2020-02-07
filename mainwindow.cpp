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
#include <QtCharts/QChartView>

#include "commander.h"
#include "display/donutbreakdown/dispiechart.h"
#include "display/multiline/dislinechart.h"

QT_CHARTS_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/images/monitor.png"));

    cmd = new Commander(this);
    Q_CHECK_PTR(cmd);

    connect(cmd, SIGNAL(connectStatus(bool, const QString &)),
            this, SLOT(connectReport(bool, const QString &)));

    connect(cmd, SIGNAL(resultSysInfo(const QMap<QString, QString> &)),
            this, SLOT(showSysInfo(const QMap<QString, QString> &)));

    connect(cmd, SIGNAL(psResultCpuUsage(const QMap<QString, double> &)),
            this, SLOT(showCpuUsage(const QMap<QString, double> &)));

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
    connect(overviewTimer, SIGNAL(timeout()),
            this, SLOT(execOverview()));

    timeAdj = new QSpinBox(this);
    Q_CHECK_PTR(timeAdj);
    timeAdj->setValue(5);
    timeAdj->setMinimum(1);
    timeAdj->setMaximum(60);
    timeAdj->setPrefix(tr("period "));
    timeAdj->setSuffix(" s");
    connect(timeAdj, SIGNAL(valueChanged(int)),
            this, SLOT(triggerValueChanged(int)));
    ui->toolBar->addWidget(timeAdj);

    overviewPie = new DisPieChart(this);
    Q_CHECK_PTR(overviewPie);

    QChartView *psChart = overviewPie->createPsChart();
    QChartView *memChart = overviewPie->createMemChart();

    ui->gridLayout->addWidget(psChart, 0, 0, 1, 5);
    ui->gridLayout->addWidget(memChart, 1, 0, 1, 15);

    overviewLine = new DisLineChart(this);
    Q_CHECK_PTR(overviewLine);

    QChartView *psLine = overviewLine->createPsChart();
    Q_CHECK_PTR(psLine);
    ui->gridLayout->addWidget(psLine, 0, 5, 1, 10);



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

        QObject::connect(&buttonBox, SIGNAL(accepted()), &conDialog, SLOT(accept()));
        QObject::connect(&buttonBox, SIGNAL(rejected()), &conDialog, SLOT(reject()));

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
    cmd->requestCpuUsage();
    cmd->requestMemUsage();
}
void MainWindow::refreshTriggerTime(int value)
{
    overviewTimer->setInterval(value * 1000);
}
void MainWindow::triggerValueChanged(int value)
{
    refreshTriggerTime(value);
}
void  MainWindow::showCpuUsage(const QMap<QString, double> &info)
{
    if(!overviewLine->psLineCreated())
    {
        overviewLine->insertPsChart(info.value("cpu count"));
    }
    overviewLine->refreshPsChart(info);
    overviewPie->refreshPsChart(info);
}
void MainWindow::showMemUsage(const QMap<QString, qulonglong> &info)
{
    qDebug() << "total " << info.value("mem.total");
    qDebug() << "free " << info.value("mem.free");
    qDebug() << "buffers " << info.value("mem.buffers");
    qDebug() << "cache " << info.value("mem.cache");
    qDebug() << "used " << info.value("mem.used");

    overviewPie->refreshMemChart(info);
}
