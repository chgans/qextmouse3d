# -------------------------------------------------
# Project created by QtCreator 2009-09-28T09:59:40
# -------------------------------------------------
QT += opengl
CONFIG += qt \
    debug
TARGET = shaderwizard
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    shaderwizardglwidget.cpp \
    qglheightmap.cpp \
    qglshaderprogrameffect.cpp \
    qglslsyntaxhighlighter.cpp \
    materialinspector.cpp
HEADERS += mainwindow.h \
    shaderwizardglwidget.h \
    qglheightmap.h \
    rippleheightmap.h \
    qglshaderprogrameffect.h \
    qglslsyntaxhighlighter.h \
    materialinspector.h
FORMS += mainwindow.ui \
    materialinspector.ui
RESOURCES = shaderwizard.qrc
LIBS += -L../../lib \
    -L../../bin
OTHER_FILES += qtlogo.png \
    toon_shader.fsh \
    toon_shader.vsh \
    per_pixel_lighting.fsh \
    per_pixel_lighting.vsh \
    flat_color_shader.fsh \
    flat_color_shader.vsh \
    per_pixel_lighting_color.fsh \
    per_pixel_lighting_color.vsh \
    minimal_texture_shader.fsh \
    minimal_texture_shader.vsh \
    per_pixel_texture.fsh \
    per_pixel_texture.vsh
include(../../threed/threed_dep.pri)
