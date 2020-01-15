#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFormLayout>
#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QDebug>

#include "commander.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    cmd = new Commander();
    Q_CHECK_PTR(cmd);

    connect(cmd, SIGNAL(connectStatus(bool)),
            this, SLOT(connectReport(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionconnect_N_triggered()
{
    QDialog conDialog(this);
    QFormLayout layout(&conDialog);

    QString ip(" IP: ");
    QLineEdit  *ipInput = new QLineEdit(&conDialog);
    ipInput->setPlaceholderText(tr("*.local or IP address"));
    layout.addRow(ip, ipInput);

    QString port(tr("Port:"));
    QSpinBox   *portInput = new QSpinBox(&conDialog);
    portInput->setRange(1024, UINT16_MAX);
    layout.addRow(port, portInput);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &conDialog);
    layout.addRow(&buttonBox);

    QObject::connect(&buttonBox, SIGNAL(accepted()), &conDialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &conDialog, SLOT(reject()));

    if(conDialog.exec() == QDialog::Accepted)
    {
        cmd->connect2Server(ipInput->text(),
                            portInput->value());
    }
}
void MainWindow::connectReport(bool status)
{
    qDebug() << "status " << status;
}
