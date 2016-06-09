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
    rscloudgrabber.cpp \
    rscloud.cpp \
    calibrationdialog.cpp

HEADERS  += \
    mainwindow.h \
    rscloudgrabber.h \
    rscloud.h \
    calibrationdialog.h

FORMS    += \
    mainwindow.ui \
    calibrationdialog.ui


