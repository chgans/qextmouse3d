TEMPLATE = lib
TARGET = QtOpenCL
CONFIG += dll \
    warn_on
win32 { 
    DESTDIR = ../bin
    !static:DEFINES += QT_MAKEDLL
}
else:DESTDIR = ../lib

HEADERS += \
    qclbuffer.h \
    qclcommandqueue.h \
    qclcontext.h \
    qcldevice.h \
    qclerrors.h \
    qclevent.h \
    qclglobal.h \
    qclimage.h \
    qclkernel.h \
    qclmemoryobject.h \
    qclplatform.h \
    qclprogram.h

SOURCES += \
    qclbuffer.cpp \
    qclcommandqueue.cpp \
    qclcontext.cpp \
    qcldevice.cpp \
    qclerrors.cpp \
    qclevent.cpp \
    qclimage.cpp \
    qclkernel.cpp \
    qclmemoryobject.cpp \
    qclplatform.cpp \
    qclprogram.cpp

HEADERS += $$PRIVATE_HEADERS
DEFINES += QT_BUILD_OPENCL_LIB
