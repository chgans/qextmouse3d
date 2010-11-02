# -------------------------------------------------
# Project created by QtCreator 2009-09-28T09:59:40
# -------------------------------------------------
QT += opengl network
CONFIG += qt
TARGET = shaderwizard
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    shaderwizardglwidget.cpp \
    qglheightmap.cpp \
    qglslsyntaxhighlighter.cpp \
    materialinspector.cpp
HEADERS += mainwindow.h \
    shaderwizardglwidget.h \
    qglheightmap.h \
    rippleheightmap.h \
    qglslsyntaxhighlighter.h \
    materialinspector.h
FORMS += mainwindow.ui \
    materialinspector.ui
RESOURCES = shaderwizard.qrc
LIBS += -L../../lib \
    -L../../bin
OTHER_FILES += qtlogo.png \
    shaders/toon_shader.vsh \
    shaders/toon_shader.fsh \
    shaders/per_pixel_texture.vsh \
    shaders/per_pixel_texture.fsh \
    shaders/per_pixel_lighting.vsh \
    shaders/per_pixel_lighting.fsh \
    shaders/multitexture.fsh \
    shaders/minimal_texture_shader.vsh \
    shaders/minimal_texture_shader.fsh \
    shaders/flat_color_shader.vsh \
    shaders/flat_color_shader.fsh
include(../../src/threed/threed_dep.pri)
