TEMPLATE = app
TARGET = basket
CONFIG += qt warn_on
QT += opengl
SOURCES = basketview.cpp main.cpp
HEADERS = basketview.h basket_data.h
RESOURCES = basket.qrc
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../src/threed/threed_dep.pri)
