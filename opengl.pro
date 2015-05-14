
# Detect libusb on Linux systems.
linux*:!isEqual(LIBUSB_DETECT_DONE,yes): include(config.tests/libusb_detect.pri)
QMAKE_DISTCLEAN += .qmake.cache

TEMPLATE = subdirs
SUBDIRS += src
!gcov: SUBDIRS += examples
SUBDIRS += tests
CONFIG += ordered

include(doc/doc.pri)

# Install qt3d.prf into the Qt mkspecs so that "CONFIG += qt3d"
# can be used in customer applications to build against Qt/3D.
feature.path = $$[QT_INSTALL_DATA]/mkspecs/features
feature.files = $$PWD/features/qt3d.prf $$PWD/features/qt3dquick.prf
INSTALLS += feature
