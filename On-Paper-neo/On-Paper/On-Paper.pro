#-------------------------------------------------
#
# Project created by QtCreator 2017-07-01T11:48:25
#
#-------------------------------------------------

QT       += core gui
CONFIG   += c++11
CONFIG   += c++14
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = On-Paper
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        onpapermain.cpp \
    ARCapturer.cpp \
    GestureJudge.cpp \
    HandDetector.cpp \
    OnPaper.cpp \
    Painter.cpp \
    PaperFun.cpp \
    glcanvas.cpp \
    optestingpanel.cpp

HEADERS += \
        onpapermain.h \
    ARCapturer.h \
    cvutils.h \
    GestureJudge.h \
    HandDetector.h \
    json.hpp \
    LayerManager.h \
    OnPaper.h \
    Painter.h \
    PaperFun.h \
    glcanvas.h \
    optestingpanel.h

FORMS += \
        onpapermain.ui \
    optestingpanel.ui

SUBDIRS += \
    On-Paper.pro

DISTFILES += \
    camera.yml \
    On-Paper.pro.user

PKGCONFIG += poppler-qt5
CONFIG += link_pkgconfig

LIBS += /usr/lib/libopencv_*.so
LIBS += /usr/lib/libaruco.so.2.0

RESOURCES += \
    resources.qrc





