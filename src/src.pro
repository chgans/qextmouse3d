TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = threed
contains(QT_CONFIG, declarative): SUBDIRS += quick3d imports
SUBDIRS += plugins
