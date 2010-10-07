TEMPLATE = subdirs

!win32:!embedded:!mac:!symbian:CONFIG += x11

x11:SUBDIRS += 3dxwarex11
linux*:SUBDIRS += linuxinput
