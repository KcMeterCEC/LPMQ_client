#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Commander;
class QLabel;
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

    void    disConnectStatus(void);

    void    closeEvent(QCloseEvent *event);
private slots:
    void on_actionconnect_triggered();
    void connectReport(bool status, const QString &errStr);

    void  showSysInfo(const QString &result);
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
