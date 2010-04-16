TEMPLATE = app
TARGET = modelviewer
CONFIG += qt \
    debug \
    warn_on
QT += opengl
SOURCES += main.cpp \
    viewer.cpp \
    controls.cpp \
    optionsdialog.cpp
HEADERS += viewer.h \
    controls.h \
    optionsdialog.h
LIBS += -L../../lib \
    -L../../bin
DESTDIR = ../../bin
include(../../threed/threed_dep.pri)
RESOURCES += modelviewer.qrc
FORMS += controls.ui \
    optionsdialog.ui
