/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qmouse3deventprovider.h"
#include "qmouse3ddevice_p.h"
#include "qmouse3ddeviceplugin_p.h"
#include <QtCore/qatomic.h>
#include <QtCore/private/qfactoryloader_p.h>
#include <QtCore/qlibraryinfo.h>

QT_BEGIN_NAMESPACE

/*!
    \class QMouse3DEventProvider
    \brief The QMouse3DEventProvider class handles 3D mouse devices, configuring delivery of events to widgets.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::viewing

    3D mice usually have six degrees of freedom of motion, corresponding
    to translations along the X, Y, and Z axes, and rotations around
    each of those axes.  Such mice also typically have an array of
    special purpose buttons for 3D navigation operations.

    QMouse3DEventProvider simplifies the process of interfacing a widget()
    to a 3D mouse device.  Once initialized, the application can detect
    the presence of a 3D mouse with isAvailable() and availableChanged().
    If there are multiple 3D mice attached to the machine, then this
    class will make it appear as though there is a single event source.

    Motions in 3D are delivered to widget() in the form of a QMouse3DEvent
    and special purpose buttons are delivered to widget() in the form of a
    QKeyEvent.  Any key code that appears in the Qt::Key enumeration may
    be delivered from a 3D mouse.  In addition, the QGL::Motion3DKeys
    enumeration defines extra key codes that are specific to 3D mice.

    Multiple QMouse3DEventProvider objects can be created by an
    application, each associated with a separate widget().  Internally,
    the widget that currently has focus will receive 3D mouse events.
    QMouse3DEventProvider can also be used without a widget to
    obtain information about the 3D mice that are attached to the machine.

    \sa QMouse3DEvent
*/

class QMouse3DDeviceList : public QObject
{
    Q_OBJECT
public:
    QMouse3DDeviceList(QObject *parent = 0);
    ~QMouse3DDeviceList();

    QBasicAtomicInt ref;
    QWidget *currentWidget;
    QList<QMouse3DDevice *> devices;

    void attachWidget(QWidget *widget);
    void detachWidget(QWidget *widget);

Q_SIGNALS:
    void availableChanged();

private:
    void setWidget(QWidget *widget);
};

static QMouse3DDeviceList *deviceList = 0;

#if !defined (QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
    (QMouse3DDeviceFactoryInterface_iid, QLatin1String("/mouse3d")))
#endif

QMouse3DDeviceList::QMouse3DDeviceList(QObject *parent)
    : QObject(parent)
    , currentWidget(0)
{
    ref = 1;
#if !defined (QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
    QFactoryLoader *l = loader();
    QStringList keys = l->keys();
    for (int index = 0; index < keys.size(); ++index) {
        if (QMouse3DDeviceFactoryInterface *factory
                = qobject_cast<QMouse3DDeviceFactoryInterface*>
                    (l->instance(keys.at(index)))) {
            QMouse3DDevice *device = factory->create();
            if (device) {
                devices.append(device);
                device->setParent(this);
                connect(device, SIGNAL(availableChanged()),
                        this, SIGNAL(availableChanged()));
            }
        }
    }
#endif
}

QMouse3DDeviceList::~QMouse3DDeviceList()
{
}

void QMouse3DDeviceList::attachWidget(QWidget *widget)
{
    // TODO: keep track of the currently active/focused widget.
    if (currentWidget != widget)
        setWidget(widget);
}

void QMouse3DDeviceList::detachWidget(QWidget *widget)
{
    if (currentWidget == widget)
        setWidget(0);
}

void QMouse3DDeviceList::setWidget(QWidget *widget)
{
    currentWidget = widget;
    for (int index = 0; index < devices.size(); ++index) {
        QMouse3DDevice *device = devices.at(index);
        if (device->isAvailable())
            device->setWidget(widget);
    }
}

class QMouse3DEventProviderPrivate
{
public:
    QMouse3DEventProviderPrivate()
        : widget(0)
    {
        if (!deviceList) {
            deviceList = new QMouse3DDeviceList();
            devices = deviceList;
        } else {
            devices = deviceList;
            devices->ref.ref();
        }
    }
    ~QMouse3DEventProviderPrivate()
    {
        if (widget)
            devices->detachWidget(widget);
        if (!devices->ref.deref()) {
            delete deviceList;
            deviceList = 0;
        }
    }

    QWidget *widget;
    QMouse3DDeviceList *devices;
};

/*!
    Constructs an event provider for the 3D mice attached to this
    machine and associate it with \a parent.
*/
QMouse3DEventProvider::QMouse3DEventProvider(QObject *parent)
    : QObject(parent)
    , d_ptr(new QMouse3DEventProviderPrivate)
{
    Q_D(QMouse3DEventProvider);
    connect(d->devices, SIGNAL(availableChanged()),
            this, SIGNAL(availableChanged()));
}

/*!
    Destroys this 3D mouse event provider.
*/
QMouse3DEventProvider::~QMouse3DEventProvider()
{
}

/*!
    Returns true if a 3D mouse device is currently available for
    use by the application; false otherwise.

    If there are multiple 3D mice attached to the machine, this will
    return true if any single one of them is available.

    \sa availableChanged(), deviceNames()
*/
bool QMouse3DEventProvider::isAvailable() const
{
    Q_D(const QMouse3DEventProvider);
    QList<QMouse3DDevice *> devices = d->devices->devices;
    for (int index = 0; index < devices.size(); ++index) {
        if (devices[index]->isAvailable())
            return true;
    }
    return false;
}

/*!
    Returns the list of all 3D mouse devices that are attached to
    the machine, associated with this event provider, and which
    are currently available.  This list is intended for debugging
    purposes.

    \sa isAvailable()
*/
QStringList QMouse3DEventProvider::deviceNames() const
{
    Q_D(const QMouse3DEventProvider);
    QList<QMouse3DDevice *> devices = d->devices->devices;
    QStringList names;
    for (int index = 0; index < devices.size(); ++index) {
        if (devices[index]->isAvailable())
            names += devices[index]->deviceNames();
    }
    return names;
}

/*!
    Returns the widget that will receive QMouse3DEvent and QKeyEvent
    events from the machine's 3D mouse device; or null if the widget
    has not yet been set.

    \sa setWidget()
*/
QWidget *QMouse3DEventProvider::widget() const
{
    Q_D(const QMouse3DEventProvider);
    return d->widget;
}

/*!
    Sets the \a widget that will receive QMouse3DEvent and QKeyEvent
    events from the machine's 3D mouse device.

    \sa widget()
*/
void QMouse3DEventProvider::setWidget(QWidget *widget)
{
    Q_D(QMouse3DEventProvider);
    if (d->widget != widget) {
        if (d->widget)
            d->devices->detachWidget(d->widget);
        d->widget = widget;
        if (widget)
            d->devices->attachWidget(widget);
    }
}

/*!
    \fn void QMouse3DEventProvider::availableChanged()

    Signal that is emitted when isAvailable() changes.  This can be
    used by an application to detect when a 3D mouse device is
    plugged in or unplugged.

    If there are multiple 3D mice attached to the machine, availableChanged()
    will be emitted when the first is plugged in, or when all have been
    unplugged.  Use deviceNames() to fetch the names of all devices that
    are attached to the machine at present.

    \sa isAvailable(), deviceNames()
*/

QT_END_NAMESPACE

#include "qmouse3deventprovider.moc"
