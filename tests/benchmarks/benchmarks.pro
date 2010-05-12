TEMPLATE = subdirs
SUBDIRS = qarray \
    qgldisplaylist_perf
contains(QT_CONFIG, declarative):SUBDIRS += matrix_properties
