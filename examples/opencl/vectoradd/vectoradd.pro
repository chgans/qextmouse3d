TEMPLATE = app
TARGET = vectoradd
CONFIG += qt debug warn_on
SOURCES = vectoradd.cpp
LIBS += -L../../../lib -L../../../bin

include(../../../opencl/opencl_dep.pri)
