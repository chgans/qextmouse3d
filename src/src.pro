TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = threed plugins
contains(QT_CONFIG, declarative): SUBDIRS += imports
