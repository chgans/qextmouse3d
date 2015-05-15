#include "qextmouse3dudevdevice.h"

#include <QtCore/qdebug.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <libudev.h>

#include <QDebug>

QT_BEGIN_NAMESPACE

// Qt5 has QDeviceDiscovery

QExtMouse3DUdevDevice::QExtMouse3DUdevDevice(QObject *parent):
    QExtMouse3DDevice(parent)
{
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev;
    int fd;

    /* Create the udev object */
    udev = udev_new();
    if (!udev) {
        qWarning() << "QExtMouse3DUdevDevice: Unable to create udev object";
        return;
    }

    /* Listen to monitor events from the 'input' subsystem
     * Events can be: "add", "remove", "change", "online", and "offline") */
    monitor = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(monitor, "input", NULL);
    udev_monitor_enable_receiving(monitor);
    fd = udev_monitor_get_fd(monitor);
    notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(notifier, SIGNAL(activated(int)),
            this, SLOT(monitorEvent(int)));
    notifier->setEnabled(true);

    /* Walk the devices in the 'input' subsystem. */
    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "input");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);
    udev_list_entry_foreach(dev_list_entry, devices) {
        deviceAdded(udev_list_entry_get_name(dev_list_entry));
    }
    udev_enumerate_unref(enumerate);

}

QExtMouse3DUdevDevice::~QExtMouse3DUdevDevice()
{
    if (monitor)
        udev_monitor_unref(monitor);
    if (udev)
        udev_unref(udev);
    qDeleteAll(devices);
}

bool QExtMouse3DUdevDevice::isAvailable() const
{
    return !devices.isEmpty();
}

QStringList QExtMouse3DUdevDevice::deviceNames() const
{
    QStringList names;
    for (int index = 0; index < devices.size(); ++index)
        names += devices[index]->realName;
    return names;
}

void QExtMouse3DUdevDevice::setProvider(QExtMouse3DEventProvider *provider)
{
    QExtMouse3DDevice::setProvider(provider);
    for (int index = 0; index < devices.size(); ++index)
        devices[index]->device->setProvider(provider);
}

void QExtMouse3DUdevDevice::setWidget(QWidget *widget)
{
    QExtMouse3DDevice::setWidget(widget);
    for (int index = 0; index < devices.size(); ++index)
        devices[index]->device->setWidget(widget);
}

void QExtMouse3DUdevDevice::updateFilters(QExtMouse3DEventProvider::Filters filters)
{
    QExtMouse3DDevice::updateFilters(filters);
    for (int index = 0; index < devices.size(); ++index)
        devices[index]->device->updateFilters(filters);
}

void QExtMouse3DUdevDevice::updateSensitivity(qreal sensitivity)
{
    QExtMouse3DDevice::updateSensitivity(sensitivity);
    for (int index = 0; index < devices.size(); ++index)
        devices[index]->device->updateSensitivity(sensitivity);
}

void QExtMouse3DUdevDevice::deviceAdded(const char *sysPath)
{
    struct udev_device *dev = udev_device_new_from_syspath(udev, sysPath);

    if (!dev)
        return;

    // If the input device node is not a mouse, then bail out.
    // (A mouse provides both event and mouse nodes)
    QString devPath(udev_device_get_devnode(dev));
    if (!devPath.contains(QString("/mouse"))) {
        udev_device_unref(dev);
        return;
    }

    // If we already have this device, then bail out.
    foreach (MouseInfo *info, devices) {
       if (info->devPath == devPath) {
           udev_device_unref(dev);
           return;
       }
    }

    // Get the real name
    QString realName(udev_device_get_property_value(dev, "ID_SERIAL"));
    if (realName.isEmpty())
        realName = udev_device_get_property_value(dev, "ID_MODEL");
    if (realName.isEmpty())
        realName = QString("%1:%2")
                .arg(udev_device_get_property_value(dev, "ID_VENDOR_ID"))
                .arg(udev_device_get_property_value(dev, "ID_MODEL_ID"));

    qDebug() << "deviceAdded:" << realName << devPath;

    // Add an entry to the device list.
    QExtMouse3DLinuxInputDevice *device =
        new QExtMouse3DLinuxInputDevice(realName, realName);
    MouseInfo *info = new MouseInfo(sysPath, devPath, realName, device);
    devices.append(info);

    // Tell the application that there is a new mouse attached.
    emit availableChanged();

    udev_device_unref(dev);
}

void QExtMouse3DUdevDevice::deviceRemoved(const char *sysPath)
{
    for (int index = 0; index < devices.size(); ++index) {
        MouseInfo *info = devices.at(index);
        if (info->sysPath == sysPath) {
            devices.removeAt(index);
            qDebug() << "deviceRemoved" << info->devPath << info->realName;
            delete info;
            emit availableChanged();
            break;
        }
    }
}

void QExtMouse3DUdevDevice::monitorEvent(int)
{
    struct udev_device *dev;
    dev = udev_monitor_receive_device(monitor);
    if (!dev) {
        qWarning() << "Received null device";
        return;
    }

    const char *vid = udev_device_get_property_value(dev, "ID_VENDOR_ID");
    const char *pid = udev_device_get_property_value(dev, "ID_MODEL_ID");
    const char *model = udev_device_get_property_value(dev, "ID_MODEL");
    const char *serial = udev_device_get_property_value(dev, "ID_SERIAL");
    const char *action = udev_device_get_action(dev);

    qDebug() << action << vid << pid << model << serial;

    if (qstrcmp(action, "add") == 0) {
        deviceAdded(udev_device_get_syspath(dev));
    }
    else if (qstrcmp(action, "remove") == 0) {
        deviceRemoved(udev_device_get_syspath(dev));
    }
    udev_device_unref(dev);
}
