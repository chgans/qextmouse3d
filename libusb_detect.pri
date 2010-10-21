
# writeVar(filename, name, value)
defineTest(writeVar) {
    system(echo "$$2 = $$3" >>\"$$1\")
}

# writeAppendVar(filename, name, value)
defineTest(writeAppendVar) {
    system(echo "$$2 += $$3" >>\"$$1\")
}

# configTest(name, qmake_vars)
defineTest(configTest) {
    NAME=$$1
    IN_DIR="$$PWD/config.tests/$$NAME"
    OUT_DIR="$$OUT_PWD/config.tests/$$NAME"
    !exists($$OUT_DIR): system(mkdir -p "$$OUT_DIR")
    verbose {
        VERBOSE_PREFIX="set -x ;"
        VERBOSE_SUFFIX=""
        message("************ Running config.test/$$NAME $$2")
    } else {
        VERBOSE_PREFIX=""
        VERBOSE_SUFFIX=">/dev/null 2>/dev/null"
    }
    system((cd "$$OUT_DIR"; $$VERBOSE_PREFIX $$QMAKE_QMAKE "$$IN_DIR" \
                LIBS\\*=\"$$LIBS\" \
                INCLUDEPATH\\*=\"$$INCLUDEPATH\" \
                QMAKE_CXXFLAGS\\*=\"$$QMAKE_CXXFLAGS\" \
                LIBS\\*=\"$$CONFIG_TEST_LIBS\" \
                INCLUDEPATH\\*=\"$$CONFIG_TEST_INCLUDES\" \
                QMAKE_CXXFLAGS\\*=\"$$CONFIG_TEST_DEFINES\" \
                CONFIG\\*=\"$$CONFIG_TEST_CONFIGS\" \
                $$2 && make clean && make) $$VERBOSE_SUFFIX) {
        system((cd "$$OUT_DIR"; \
                $$VERBOSE_PREFIX make distclean) $$VERBOSE_SUFFIX)
        verbose {
            message("************ Success: config.test/$$NAME $$2")
        }
        return(true)
    } else {
        system((cd "$$OUT_DIR"; \
                $$VERBOSE_PREFIX make distclean) $$VERBOSE_SUFFIX)
        verbose {
            message("************ Failed: config.test/$$NAME $$2")
        }
        return(false)
    }
}

CONFIG_TEST_LIBS =
CONFIG_TEST_INCLUDES =
CONFIG_TEST_DEFINES =
CONFIG_TEST_CONFIGS =

CACHE_FILE=$$OUT_PWD/.qmake.cache
configTest(libusb) {
    writeAppendVar($$CACHE_FILE, CONFIG, have_libusb)
}
writeVar($$CACHE_FILE, LIBUSB_DETECT_DONE, yes)
