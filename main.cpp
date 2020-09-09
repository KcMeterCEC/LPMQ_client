#include "MainWindow.h"

#include <QApplication>
#include <QFile>
#include <QSplashScreen>
#include <QDebug>

#define USE_STYLESHEET  (0)

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPixmap pixmap(":images/basic/splash.png");
    QSplashScreen splash(pixmap);
    splash.show();

#if USE_STYLESHEET
    QFile f(":qdarkstyle/style.qss");
    if(!f.exists())
    {
        qWarning() << "Unable to set stylesheet, file not found!";
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }
#endif

    MainWindow w;
    w.show();

    splash.finish(&w);

    return a.exec();
}
