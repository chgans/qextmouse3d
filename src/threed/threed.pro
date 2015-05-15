TEMPLATE = lib
TARGET = QExtMouse3D$${QT_LIBINFIX}
gcov {
    CONFIG += staticlib warn_on
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
} else {
    CONFIG += dll warn_on
}
DESTDIR = ../../lib

win32 {
    DLLDESTDIR = ../../bin
    !static:DEFINES += QT_MAKEDLL

    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

symbian {
    DEFINES += QT_MAKEDLL
    CONFIG += epocallowdlldata
    contains(QT_EDITION, OpenSource) {
        TARGET.CAPABILITY = LocalServices NetworkServices ReadUserData UserEnvironment WriteUserData
    } else {
        TARGET.CAPABILITY = All -Tcb
    }
}

include(threed.pri)
PUBLIC_HEADERS = $$HEADERS
HEADERS += $$PRIVATE_HEADERS
DEFINES += QT_BUILD_QEXTMOUSE3D_LIB

!symbian {
    target.path += $$[QT_INSTALL_LIBS]
    INSTALLS += target

    install_headers.path = $$[QT_INSTALL_HEADERS]/QExtMouse3D
    install_headers.files = $$PUBLIC_HEADERS
    INSTALLS += install_headers
} else {
    exportHeaders.input = PUBLIC_HEADERS
    exportHeaders.output = $$[QT_INSTALL_HEADERS]/QExtMouse3D/${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}
    exportHeaders.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
    exportHeaders.CONFIG += no_link_no_clean
    exportHeaders.variable_out = PRE_TARGETDEPS
    QMAKE_EXTRA_COMPILERS += exportHeaders
}
