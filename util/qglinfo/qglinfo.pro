TARGET = qglinfo
CONFIG += debug_and_release
SOURCES += main.cpp \
    qglinfowindow.cpp \
    qglinfo.cpp \
    aboutdialog.cpp \
    fpswidget.cpp
DESTDIR = ../../bin
QT += opengl
FORMS += qglinfowindow.ui \
    aboutdialog.ui
HEADERS += qglinfowindow.h \
    qglinfo.h \
    aboutdialog.h \
    fpswidget.h
RESOURCES += qglinfo.qrc

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL
