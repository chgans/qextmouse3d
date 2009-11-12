TEMPLATE = app
TARGET = converting
CONFIG += qt debug warn_on
QT += opengl
SOURCES = converting.cpp
LIBS += -L../../../lib -L../../../bin

include(../../../threed/threed_dep.pri)
