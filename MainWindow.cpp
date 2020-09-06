#include <QLabel>
#include <QToolBar>

#include "MainWindow.h"

MainWindow::MainWindow(QMainWindow *parent)
    : QMainWindow(parent)
{
    setWindowIcon(QIcon(":/images/basic/monitor.png"));

    toolBarsCreate();

    //Create the dock manage.Registers itself as the central widget.
    dockManager = new ads::CDockManager(this);

    // Create example content label - this can be any application specific
    // widget
    QLabel* l = new QLabel();
    l->setWordWrap(true);
    l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    l->setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ");

    // Create a dock widget with the title Label 1 and set the created label
    // as the dock widget content
    ads::CDockWidget* DockWidget = new ads::CDockWidget("Label 1");
    DockWidget->setWidget(l);

    addAction(DockWidget->toggleViewAction());

    // Add the dock widget to the top dock widget area
    dockManager->addDockWidget(ads::TopDockWidgetArea, DockWidget);
}
void MainWindow::toolBarsCreate(void)
{
    QToolBar *toolBar = addToolBar(tr("tools"));
    QAction *connectTool = toolBar->addAction(QIcon(":/images/basic/connect.png"), tr("connect to server"));
}

MainWindow::~MainWindow()
{
}

