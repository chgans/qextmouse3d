TEMPLATE = lib
TARGET = Qt3D$${QT_LIBINFIX}
gcov {
    CONFIG += staticlib warn_on
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
} else {
    CONFIG += dll warn_on
}
QT += opengl \
    network
DESTDIR = ../lib

win32 {
    CONFIG += debug_and_release debug_and_release_config build_all
    DLLDESTDIR = ../bin
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
DEFINES += QT_BUILD_QT3D_LIB

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

!symbian {
    target.path += $$[QT_INSTALL_LIBS]
    INSTALLS += target

    install_headers.path = $$[QT_INSTALL_HEADERS]/Qt3D
    install_headers.files = $$PUBLIC_HEADERS
    INSTALLS += install_headers
} else {
    load(data_caging_paths)

    qt3dMwHeaders = painting/* \
                       enablers/* \
                       global/* \
                       effects/* \
                       geometry/* \
                       viewing/* \
                       math3d/* \
                       scene/* \
                       graphicsview/* \
                       animation/* \
                       textures/* \
                       surfaces/*

    for(api, qt3dMwHeaders) {
        entries=$$files($$api);
        #files() attaches a ';' at the end which needs to be removed
        entries=$$replace(entries, ;,)
        for(entry, entries) {
            exists($$entry) {
                contains(PUBLIC_HEADERS, $$basename(entry)) {
                    BLD_INF_RULES.prj_exports += "$$entry $$MW_LAYER_PUBLIC_EXPORT_PATH(Qt3D/$$basename(entry))"
                }
            }
        }
    }
}
