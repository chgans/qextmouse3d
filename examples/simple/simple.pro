#-------------------------------------------------
#
# Project created by QtCreator 2015-05-14T17:06:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = simple
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin
include(../../src/threed/threed_dep.pri)
