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
    grabberprogressdialog.cpp \
    calibration_utils/main.cpp \
    calibration_utils/colorring.cpp \
    calibration_utils/pointcatched.cpp \
    calibration_utils/transformationutils.cpp \
    calibration_utils/visualizationutils.cpp

HEADERS  += \
    mainwindow.h \
    rscloud.h \
    calibrationdialog.h \
    cloudsgrabber.h \
    grabberprogressdialog.h \
    calibration_utils/colorring.h \
    calibration_utils/pointcatched.h \
    calibration_utils/transformationutils.h \
    calibration_utils/visualizationutils.h

FORMS    += \
    mainwindow.ui \
    calibrationdialog.ui


