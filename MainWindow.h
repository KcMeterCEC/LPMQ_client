#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "DockManager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QMainWindow *parent = nullptr);
    ~MainWindow();
private:
    //The main container for docking
    ads::CDockManager       *dockManager;

    void toolBarsCreate(void);
};
#endif // MAINWINDOW_H
