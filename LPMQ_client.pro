QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    MainWindow.cpp \
    control/commander.cpp \
    control/targetio.cpp \
    control/targetmem.cpp \
    control/targetps.cpp \
    display/scurve/slegend.cpp \
    display/scurve/smarker.cpp \
    display/scurve/splot.cpp \
    display/scurve/splotcurve.cpp \
    display/scurve/splotgrid.cpp \
    display/scurve/splotmagnifier.cpp \
    display/scurve/splotpanner.cpp \
    display/scurve/sscaledraw.cpp \
    display/scurve/ssymbol.cpp \
    display/scurve/statisticcurve.cpp \
    main.cpp \
    rb.cpp

HEADERS += \
    MainWindow.h \
    control/commander.h \
    control/targetio.h \
    control/targetmem.h \
    control/targetps.h \
    display/scurve/slegend.h \
    display/scurve/smarker.h \
    display/scurve/splot.h \
    display/scurve/splotcurve.h \
    display/scurve/splotgrid.h \
    display/scurve/splotmagnifier.h \
    display/scurve/splotpanner.h \
    display/scurve/sscaledraw.h \
    display/scurve/ssymbol.h \
    display/scurve/statisticcurve.h \
    rb.h

QT += network
#
# Third party library
#

# Qt-Advanced-Docking-System
INCLUDEPATH += 3rdlib/ads/include
LIBS += -L"3rdlib/ads/lib" -lqtadvanceddockingd

# QSS
RESOURCES += 3rdlib/qdarkstyle/style.qrc \
    pic/pic.qrc

# Qwt
win32
{
    QWT_ROOT = C:/Qwt-6.1.4
}

unix
{

}

include ($${QWT_ROOT}/features/qwt.prf)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
