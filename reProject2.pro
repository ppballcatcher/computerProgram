#-------------------------------------------------
#
# Project created by QtCreator 2014-11-24T17:59:00
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4) : QT += widgets serialport

TARGET = reProject2
TEMPLATE = app
CONFIG += c++11


SOURCES += \
    main.cpp\
    mainwindow.cpp \
    settingsdialog.cpp \
    serialthread.cpp

HEADERS  += \
    mainwindow.h \
    settingsdialog.h \
    hit.h \
    serialthread.h


FORMS    += \
    mainwindow.ui \
    settingsdialog.ui

OTHER_FILES += \
    images/application-exit.png \
    images/clear.png \
    images/connect.png \
    images/disconnect.png \
    images/settings.png
