TEMPLATE = subdirs
SUBDIRS += threed plugins util examples demos tests
contains(QT_CONFIG, declarative): SUBDIRS += imports
CONFIG += ordered

include(doc/doc.pri)

# Install qt3d.prf into the Qt mkspecs so that "CONFIG += qt3d"
# can be used in customer applications to build against Qt/3D.
feature.path = $$[QT_INSTALL_DATA]/mkspecs/features
feature.files = $$PWD/features/qt3d.prf
INSTALLS += feature
