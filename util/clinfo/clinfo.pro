TARGET = clinfo

SOURCES += \
    clinfo.cpp \

LIBS += -L../../lib -L../../bin

include(../../opencl/opencl_dep.pri)
