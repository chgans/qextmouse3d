TEMPLATE = subdirs
SUBDIRS += threed plugins util examples demos tests
contains(QT_CONFIG, declarative): SUBDIRS += imports
CONFIG += ordered

include(doc/doc.pri)
