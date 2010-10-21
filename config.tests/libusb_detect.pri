isEmpty(CONFIG_TEST_LOADED): include(config_test.pri)

configTest(libusb) {
    writeCacheAppendVar(CONFIG, have_libusb)
}
writeCacheVar(LIBUSB_DETECT_DONE, yes)
