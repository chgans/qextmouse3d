/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmouse3dhaldevice.h"
#include <QtCore/qdebug.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

QT_BEGIN_NAMESPACE

#define HAL_DBUS_SERVICE "org.freedesktop.Hal"
#define HAL_DBUS_PATH "/org/freedesktop/Hal"
#define HAL_DBUS_INTERFACE "org.freedesktop.Hal"
#define HAL_DBUS_MANAGER_PATH "/org/freedesktop/Hal/Manager"
#define HAL_DBUS_MANAGER_INTERFACE "org.freedesktop.Hal.Manager"
#define HAL_DEVICE_PATH "/org/freedesktop/Hal/Device"
#define HAL_DEVICE_INTERFACE "org.freedesktop.Hal.Device"
#define HAL_DEVICES_PATH "/org/freedesktop/Hal/devices"

QMouse3DHalDevice::QMouse3DHalDevice(QObject *parent)
    : QMouse3DDevice(parent)
    , connection(QDBusConnection::systemBus())
    , iface(0)
{
    // Attempt to connect to HAL.  Do nothing if we cannot find it.
    iface = new QDBusInterface(QLatin1String(HAL_DBUS_SERVICE),
                               QLatin1String(HAL_DBUS_MANAGER_PATH),
                               QLatin1String(HAL_DBUS_MANAGER_INTERFACE),
                               connection);
    if (!iface->isValid()) {
        delete iface;
        iface = 0;
        return;
    }

    // Connect to device add/remove signals.
    connection.connect(QLatin1String(HAL_DBUS_SERVICE),
                       HAL_DBUS_MANAGER_PATH,
                       QLatin1String(HAL_DBUS_MANAGER_INTERFACE),
                       QLatin1String("DeviceAdded"),
                       this, SLOT(deviceAdded(QString)));
    connection.connect(QLatin1String(HAL_DBUS_SERVICE),
                       HAL_DBUS_MANAGER_PATH,
                       QLatin1String(HAL_DBUS_MANAGER_INTERFACE),
                       QLatin1String("DeviceRemoved"),
                       this, SLOT(deviceRemoved(QString)));

    // Find all mouse devices that are currently connected.
    QDBusReply<QStringList> reply = iface->call
        (QLatin1String("FindDeviceByCapability"), QLatin1String("input"));
    if (reply.isValid()) {
        QStringList devices = reply.value();
        for (int index = 0; index < devices.size(); ++index) {
            QDBusInterface *deviceIface = new QDBusInterface
                (QLatin1String(HAL_DBUS_SERVICE), devices.at(index),
                 QLatin1String(HAL_DEVICE_INTERFACE), connection);
            if (deviceIface->isValid())
                mouseAdded(devices.at(index), deviceIface);
            delete deviceIface;
        }
    }
}

QMouse3DHalDevice::~QMouse3DHalDevice()
{
    delete iface;
    qDeleteAll(devices);
}

bool QMouse3DHalDevice::isAvailable() const
{
    return !devices.isEmpty();
}

QStringList QMouse3DHalDevice::deviceNames() const
{
    QStringList names;
    for (int index = 0; index < devices.size(); ++index)
        names += devices[index]->realName;
    return names;
}

void QMouse3DHalDevice::setProvider(QMouse3DEventProvider *provider)
{
    QMouse3DDevice::setProvider(provider);
    for (int index = 0; index < devices.size(); ++index)
        devices[index]->device->setProvider(provider);
}

void QMouse3DHalDevice::setWidget(QWidget *widget)
{
    QMouse3DDevice::setWidget(widget);
    for (int index = 0; index < devices.size(); ++index)
        devices[index]->device->setWidget(widget);
}

void QMouse3DHalDevice::updateFilters(QMouse3DEventProvider::Filters filters)
{
    QMouse3DDevice::updateFilters(filters);
    for (int index = 0; index < devices.size(); ++index)
        devices[index]->device->updateFilters(filters);
}

void QMouse3DHalDevice::updateSensitivity(qreal sensitivity)
{
    QMouse3DDevice::updateSensitivity(sensitivity);
    for (int index = 0; index < devices.size(); ++index)
        devices[index]->device->updateSensitivity(sensitivity);
}

void QMouse3DHalDevice::deviceAdded(const QString &path)
{
    QDBusInterface *deviceIface = new QDBusInterface
        (QLatin1String(HAL_DBUS_SERVICE), path,
         QLatin1String(HAL_DEVICE_INTERFACE), connection);
    if (!deviceIface->isValid()) {
        delete deviceIface;
        return;
    }
    QDBusReply<bool> reply = deviceIface->call
        (QLatin1String("QueryCapability"), QLatin1String("input"));
    if (reply.isValid() && reply.value())
        mouseAdded(path, deviceIface);
    delete deviceIface;
}

void QMouse3DHalDevice::deviceRemoved(const QString &path)
{
    for (int index = 0; index < devices.size(); ++index) {
        MouseInfo *info = devices[index];
        if (info->halPath == path) {
            devices.removeAt(index);
            delete info;
            emit availableChanged();
            break;
        }
    }
}

void QMouse3DHalDevice::mouseAdded
    (const QString &path, QDBusInterface *deviceIface)
{
    // Get the name of the /dev/input/eventN device for this mouse.
    QDBusReply<QString> reply = deviceIface->call
        (QLatin1String("GetPropertyString"), QLatin1String("input.device"));
    if (!reply.isValid())
        return;
    QString devName = reply.value();
    if (devName.isEmpty())
        return;

    // If we already have this device, then bail out.  It is common
    // for HAL to send DeviceAdded notifications for the same
    // /dev/input/eventN under multiple paths.
    for (int index = 0; index < devices.size(); ++index) {
        if (devices[index]->devName == devName)
            return;
    }

    // Open the device and check that it reports X, Y, Z, RX, RY, and RZ.
    // The device should be EV_ABS, but some 3Dconnexion mice report as
    // EV_REL instead, so look for both.
    int fd = ::open(devName.toLatin1().constData(), O_RDONLY | O_NONBLOCK, 0);
    if (fd < 0)
        return;
    unsigned long bits = 0;
    int found = 0;
    if (::ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(bits)), &bits) >= 0)
        found = ((bits & 0x3F) == 0x3F);
    if (!found && ::ioctl(fd, EVIOCGBIT(EV_REL, sizeof(bits)), &bits) >= 0)
        found = ((bits & 0x3F) == 0x3F);
    if (!found) {
        // Not 3D - some other kind of mouse.
        ::close(fd);
        return;
    }

    // Get the real name of the device.
    QString realName;
    char name[256];
    memset(name, 0, sizeof(name));
    if (::ioctl(fd, EVIOCGNAME(sizeof(name)), name) >= 0)
        realName = QLatin1String(name);
    ::close(fd);

    // Add an entry to the device list.
    QMouse3DLinuxInputDevice *device =
        new QMouse3DLinuxInputDevice(devName, realName);
    MouseInfo *info = new MouseInfo(path, devName, realName, device);
    devices.append(info);

    // Tell the application that there is a new mouse attached.
    emit availableChanged();
}

QT_END_NAMESPACE
