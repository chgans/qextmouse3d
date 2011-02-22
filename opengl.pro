
# Detect libusb on Linux systems.
linux*:!isEqual(LIBUSB_DETECT_DONE,yes): include(config.tests/libusb_detect.pri)
QMAKE_DISTCLEAN += .qmake.cache

TEMPLATE = subdirs
SUBDIRS += src
!gcov: SUBDIRS += util examples demos
SUBDIRS += tests
CONFIG += ordered

include(doc/doc.pri)

# Install qt3d.prf into the Qt mkspecs so that "CONFIG += qt3d"
# can be used in customer applications to build against Qt/3D.
feature.path = $$[QT_INSTALL_DATA]/mkspecs/features
feature.files = $$PWD/features/qt3d.prf $$PWD/features/qt3dquick.prf
INSTALLS += feature

#symbian does not generate make install rule. Copy prf manually
symbian {
    qt3dFile=$$PWD\\features\\qt3d.prf
    qt3dFile=$$replace(qt3dFile,/,\\)
    featuresDir=$$[QT_INSTALL_DATA]\\mkspecs\\features
    featuresDir=$$replace(featuresDir,/,\\)
    system(copy "$$qt3dFile $$featuresDir")

    qt3dQuickFile=$$PWD\\features\\qt3dquick.prf
    qt3dQuickFile=$$replace(qt3dQuickFile,/,\\)
    system(copy "$$qt3dQuickFile $$featuresDir")

    # symbian needs to be at the end, because opengl.pro does an ordered build,
    # and symbian depends on all the others.
    SUBDIRS += symbianinstall

    symbianinstall.subdir = devices/symbian
    symbianinstall.target = sub-symbianinstall
    symbianinstall.depends = $$SUBDIRS
    symbianinstall.depends -= symbianinstall
}
