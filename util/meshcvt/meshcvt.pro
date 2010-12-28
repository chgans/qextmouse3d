
TARGET = meshcvt

SOURCES += \
    meshcvt.cpp \

win32:DEFINES+=_CRT_SECURE_NO_WARNINGS

LIBS += -L../../lib -L../../bin

include(../../src/threed/threed_dep.pri)
