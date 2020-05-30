QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += network charts

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
    commander.cpp \
    display/donutbreakdown/dispiechart.cpp \
    display/donutbreakdown/donutbreakdownchart.cpp \
    display/donutbreakdown/mainslice.cpp \
    display/linechart/linechart.cpp \
    display/linechart/linechartview.cpp \
    display/linechart/markerline.cpp \
    display/tasklist/filterwidget.cpp \
    display/tasklist/tasklist.cpp \
    main.cpp \
    mainwindow.cpp \
    mem.cpp \
    rb.cpp \
    process.cpp

HEADERS += \
    commander.h \
    display/donutbreakdown/dispiechart.h \
    display/donutbreakdown/donutbreakdownchart.h \
    display/donutbreakdown/mainslice.h \
    display/linechart/linechart.h \
    display/linechart/linechartview.h \
    display/linechart/markerline.h \
    display/tasklist/filterwidget.h \
    display/tasklist/tasklist.h \
    mainwindow.h \
    mem.h \
    rb.h \
    process.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    lpmqc_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    uimages.qrc
