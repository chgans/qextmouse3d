TEMPLATE = subdirs

!win32:!embedded:!mac:!symbian:CONFIG += x11

x11: SUBDIRS = magellan
