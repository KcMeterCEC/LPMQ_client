#include <QLabel>
#include <QToolBar>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QGridLayout>
#include <QSettings>
#include <QMessageBox>
#include <QDebug>

#include "MainWindow.h"
#include "control/commander.h"

MainWindow::MainWindow(QMainWindow *parent)
    : QMainWindow(parent),
      dockManager(new ads::CDockManager(this)),
      userCfg(new QSettings("kcmetercec", "lpmq")),
      msg(new QMessageBox)
{
    setWindowIcon(QIcon(":/images/basic/monitor.png"));
    setMinimumSize(960, 540);

    cmd = Commander::getInstance();
    connect(cmd, &Commander::connectChanged, this, &MainWindow::connectMsg);

    toolBarsCreate();
    msgCreate();
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
}
void MainWindow::msgCreate(void)
{
    msg->setWindowIcon(QIcon(":/images/basic/monitor.png"));
    msg->setDefaultButton(QMessageBox::Ok);
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
void MainWindow::refreshConnectStatus(void)
{
    if(cmd->connectStatus())
    {
        connectTool->setIcon(QIcon(":/images/basic/disconnect.png"));
        connectTool->setToolTip(tr("disconnect from server"));
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