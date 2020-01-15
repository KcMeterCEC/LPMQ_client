#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Commander;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Commander       *cmd;
private slots:
    void on_actionconnect_N_triggered();
    void connectReport(bool status);
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
