load(qttest_p4)
TEMPLATE = app
TARGET = tst_matrix_properties
QT += declarative script
macx:CONFIG -= app_bundle
CONFIG += release

SOURCES += tst_matrix_properties.cpp

symbian* {
    data.sources = data/*
    data.path = data
    DEPLOYMENT += data
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}




