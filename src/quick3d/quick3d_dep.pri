INCLUDEPATH += $$PWD

win32:CONFIG(debug, debug|release) {
    LIBS += -lQt3DQuick$${QT_LIBINFIX}d
} else {
    LIBS += -lQt3DQuick$${QT_LIBINFIX}
}

include(../threed/threed_dep.pri)
