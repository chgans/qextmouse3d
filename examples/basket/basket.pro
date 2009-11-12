TEMPLATE = app
TARGET = basket
CONFIG += qt debug warn_on
QT += opengl
SOURCES = basketview.cpp main.cpp
HEADERS = basketview.h basket_data.h
RESOURCES = basket.qrc
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../threed/threed_dep.pri)
