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

#include "commander.h"

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

    connect(cmd, SIGNAL(resultSysInfo(const QString &)),
            this, SLOT(showSysInfo(const QString &)));

    connectLab = new QLabel(this);
    Q_CHECK_PTR(connectLab);
    ui->statusbar->addPermanentWidget(connectLab);

    sysInfoStatus = new QLabel(this);
    Q_CHECK_PTR(sysInfoStatus);

    userCfg = new QSettings("kcmetercec","lpmq");
    Q_CHECK_PTR(userCfg);

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
    }
    else
    {
        ui->actionconnect->setIcon(QIcon(":/images/images/link.png"));
        ui->actionconnect->setText(tr("connect"));
        connectLab->setText(tr("disconnected"));
        connectLab->setStyleSheet("color: rgb(255, 0, 0);");
    }
}
void MainWindow::connectReport(bool status, const QString &errStr)
{
    qDebug() << "status " << status;

    if(status)
    {
        disConnectStatus();
        cmd->requestSysInfo();
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
void  MainWindow::showSysInfo(const QString &result)
{
    qDebug() << "sys info result:";
    sysInfoStatus->setToolTip(result);

    QStringList list = result.split(QRegExp("[\t\n:]"));
    list.removeAll("");


    for(auto v = list.cbegin(); v != list.cend(); ++v)
    {
        if(*v == "model name")
        {
            sysInfoStatus->setText(*(v + 1));
            ui->statusbar->addWidget(sysInfoStatus);
        }
    }
}
