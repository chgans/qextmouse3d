# Use subdirs template to suppress generation of unnecessary files
TEMPLATE = subdirs

symbian {
    load(data_caging_paths)

    SUBDIRS=
    # WARNING: Changing TARGET name will break Symbian SISX upgrade functionality
    # DO NOT TOUCH TARGET VARIABLE IF YOU ARE NOT SURE WHAT YOU ARE DOING
    TARGET = "Qt3D$${QT_LIBINFIX}"


    isEmpty(QT_LIBINFIX) {
        # TODO: Use signed UID
        TARGET.UID3 = 0xE001E61E
    } else {
        # Always use experimental UID for infixed configuration to avoid UID clash
        TARGET.UID3 = 0xE001E61E
    }

    qt3dlibraries.sources = Qt3D$${QT_LIBINFIX}.dll
    qt3dlibraries.path = /sys/bin
    DEPLOYMENT += qt3dlibraries

    qt3dsceneformats_plugins.sources = \
                                qscene3ds$${QT_LIBINFIX}.dll \
                                qsceneobj$${QT_LIBINFIX}.dll \
                                qscenebezier$${QT_LIBINFIX}.dll
    qt3dsceneformats_plugins.path = $$QT_PLUGINS_BASE_DIR/sceneformats
    DEPLOYMENT += qt3dsceneformats_plugins

    qt3dimageformats_plugins.sources = \
                                qtga$${QT_LIBINFIX}.dll
    qt3dimageformats_plugins.path = $$QT_PLUGINS_BASE_DIR/imageformats
    DEPLOYMENT += qt3dimageformats_plugins

    contains(QT_CONFIG, declarative): {
        qt3dthreedImports.sources = \
                                qthreedqmlplugin.dll \
                                qmldir
        qt3dthreedImports.path = $$QT_IMPORTS_BASE_DIR/Qt3D
        qt3dshapesImports.sources = \
                                Cube.qml \
                                cube.obj \
                                qmldir
        qt3dshapesImports.path = $$QT_IMPORTS_BASE_DIR/Qt3D/Shapes
        DEPLOYMENT += qt3dthreedImports qt3dshapesImports
    }
}
