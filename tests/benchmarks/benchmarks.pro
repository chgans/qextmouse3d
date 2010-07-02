TEMPLATE = subdirs
SUBDIRS = qarray \
    QGLBuilder_perf
contains(QT_CONFIG, declarative):SUBDIRS += matrix_properties
