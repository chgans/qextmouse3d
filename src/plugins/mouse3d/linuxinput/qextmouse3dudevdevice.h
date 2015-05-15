#ifndef QEXTMOUSE3DUDEVDEVICE_H
#define QEXTMOUSE3DUDEVDEVICE_H

#include "qmouse3ddevice_p.h"
#include "qmouse3dlinuxinputdevice.h"
#include <QtDBus/qdbusconnection.h>
#include <QtDBus/qdbusinterface.h>
#include <QtDBus/qdbusreply.h>

struct udev;
struct udev_monitor;

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QExtMouse3DUdevDevice : public QExtMouse3DDevice
{
    Q_OBJECT
public:
    QExtMouse3DUdevDevice(QObject *parent = 0);
    ~QExtMouse3DUdevDevice();

    bool isAvailable() const;
    QStringList deviceNames() const;

    void setProvider(QExtMouse3DEventProvider *provider);
    void setWidget(QWidget *widget);
    void updateFilters(QExtMouse3DEventProvider::Filters filters);
    void updateSensitivity(qreal sensitivity);

private Q_SLOTS:
    void deviceAdded(const char *path);
    void deviceRemoved(const char *sysPath);
    void monitorEvent(int fd);

private:

    class MouseInfo
    {
    public:
        MouseInfo(const QString &sys, const QString &dev,
                  const QString &rName, QExtMouse3DLinuxInputDevice *idev)
            : sysPath(sys), devPath(dev), realName(rName), device(idev) {}
        ~MouseInfo() { delete device; }

        QString sysPath;
        QString devPath;
        QString realName;
        QExtMouse3DLinuxInputDevice *device;
    };

    struct udev *udev;
    struct udev_monitor *monitor;
    QSocketNotifier *notifier;
    QList<MouseInfo *> devices;

};

QT_END_NAMESPACE

QT_END_HEADER

#endif
