#-------------------------------------------------
#
# Project created by QtCreator 2014-05-01T14:24:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pcl_visualizer
TEMPLATE = app


SOURCES += main.cpp \
    mainwindow.cpp \
    rscloud.cpp \
    calibrationdialog.cpp \
    cloudsgrabber.cpp \
    grabberprogressdialog.cpp

HEADERS  += \
    mainwindow.h \
    rscloud.h \
    calibrationdialog.h \
    cloudsgrabber.h \
    grabberprogressdialog.h

FORMS    += \
    mainwindow.ui \
    calibrationdialog.ui


