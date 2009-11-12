TEMPLATE = app
TARGET = qml3d
CONFIG += qt debug warn_on
QT += opengl declarative network
SOURCES = main.cpp item3d.cpp qml3dview.cpp mesh.cpp viewport.cpp \
          effect.cpp cube.cpp shaderprogram.cpp sphere.cpp rotation3d.cpp
HEADERS =  item3d.h qml3dview.h mesh.h viewport.h effect.h \
           cube.h shaderprogram.h sphere.h rotation3d.h
LIBS += -L../../lib -L../../bin
DESTDIR = ../../bin

include(../../threed/threed_dep.pri)
