INCLUDEPATH += $$PWD/global
INCLUDEPATH += $$PWD/viewing

gcov {
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
}

win32:CONFIG(debug, debug|release) {
    LIBS += -lQExtMouse3D$${QT_LIBINFIX}d
} else {
    LIBS += -lQExtMouse3D$${QT_LIBINFIX}
}

# Locate the "lib" directory in the build tree and put it before
# the Qt "lib" directory in the library path so that we link
# against the libQt3D.so in the Qt/3D build tree, not the Qt one.
FIND_TOP=..
for(i,forever) {
    exists($$_PRO_FILE_PWD_/$$FIND_TOP/opengl.pro):break()
    FIND_TOP=../$$FIND_TOP
}
load(qt)
QMAKE_LIBDIR=$$OUT_PWD/$$FIND_TOP/lib $$QMAKE_LIBDIR
