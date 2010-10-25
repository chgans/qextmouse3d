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

#include "qmouse3ddevicelist_p.h"
#include "qmouse3ddeviceplugin_p.h"
#include <QtCore/private/qfactoryloader_p.h>
#include <QtCore/qlibraryinfo.h>
#include <QtGui/qwidget.h>
#include <QtGui/qapplication.h>

QT_BEGIN_NAMESPACE

#if !defined (QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
    (QMouse3DDeviceFactoryInterface_iid, QLatin1String("/mouse3d")))
#endif

QMouse3DDeviceList::QMouse3DDeviceList(QObject *parent)
    : QObject(parent)
    , currentWidget(0)
    , currentProvider(0)
{
    ref = 1;
    if (QMouse3DDevice::testDevice1) {
        // Special hook for auto-testing.
        devices.append(QMouse3DDevice::testDevice1);
        connect(QMouse3DDevice::testDevice1, SIGNAL(availableChanged()),
                this, SLOT(availableDeviceChanged()));
        if (QMouse3DDevice::testDevice2) {
            devices.append(QMouse3DDevice::testDevice2);
            connect(QMouse3DDevice::testDevice2,
                    SIGNAL(availableChanged()),
                    this, SIGNAL(availableChanged()));
        }
    } else {
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
                            this, SLOT(availableDeviceChanged()));
                }
            }
        }
    }
#endif
}

QMouse3DDeviceList::~QMouse3DDeviceList()
{
}

void QMouse3DDeviceList::attachWidget
    (QMouse3DEventProvider *provider, QWidget *widget)
{
    // Add the (widget, provider) pair to the map.
    if (widgets.contains(widget)) {
        qWarning("QMouse3DEventProvider: multiple providers for single widget");
        return;
    }
    widgets.insert(widget, provider);

    // Install an event filter to track window activate/deactivate events.
    widget->installEventFilter(this);

    // If the widget is already active, then set it as the current widget.
    // Also do this if we are running the autotests because activating a
    // widget during autotests is unstable at best.
    if (widget->isActiveWindow() || QMouse3DDevice::testDevice1)
        setWidget(provider, widget);
}

void QMouse3DDeviceList::detachWidget
    (QMouse3DEventProvider *provider, QWidget *widget)
{
    // Check that this is the provider that was registered for the widget.
    QMouse3DEventProvider *prov = widgets.value(widget, 0);
    if (prov != provider)
        return;

    // Remove the widget from the map.
    widgets.remove(widget);

    // Remove the window activate/deactivate event filter.
    widget->removeEventFilter(this);

    // If this was the current widget, then deactivate the device.
    if (currentWidget == widget)
        setWidget(0, 0);
}

void QMouse3DDeviceList::setWidget
    (QMouse3DEventProvider *provider, QWidget *widget)
{
    currentProvider = provider;
    currentWidget = widget;
    for (int index = 0; index < devices.size(); ++index) {
        QMouse3DDevice *device = devices.at(index);
        if (device->isAvailable()) {
            device->setProvider(provider);
            device->setWidget(widget);
            if (provider) {
                device->updateFilters(provider->filters());
                device->updateSensitivity(provider->sensitivity());
            } else {
                device->updateFilters(QMouse3DEventProvider::Translations |
                                      QMouse3DEventProvider::Rotations);
                device->updateSensitivity(1.0f);
            }
        }
    }
}

void QMouse3DDeviceList::updateFilters
    (QMouse3DEventProvider *provider, QMouse3DEventProvider::Filters value)
{
    if (currentProvider == provider) {
        for (int index = 0; index < devices.size(); ++index) {
            QMouse3DDevice *device = devices.at(index);
            if (device->isAvailable())
                device->updateFilters(value);
        }
    }
}

void QMouse3DDeviceList::updateSensitivity
    (QMouse3DEventProvider *provider, qreal value)
{
    if (currentProvider == provider) {
        for (int index = 0; index < devices.size(); ++index) {
            QMouse3DDevice *device = devices.at(index);
            if (device->isAvailable())
                device->updateSensitivity(value);
        }
    }
}

static QMouse3DDeviceList *deviceList = 0;

QMouse3DDeviceList *QMouse3DDeviceList::attach()
{
    if (!deviceList) {
        deviceList = new QMouse3DDeviceList();
        return deviceList;
    }
    deviceList->ref.ref();
    return deviceList;
}

void QMouse3DDeviceList::detach(QMouse3DDeviceList *list)
{
    if (!list->ref.deref()) {
        delete list;
        deviceList = 0;
    }
}

void QMouse3DDeviceList::availableDeviceChanged()
{
    QMouse3DDevice *device = qobject_cast<QMouse3DDevice *>(sender());
    if (device && device->isAvailable()) {
        // Newly available devices need to be told the provider,
        // widget, filter state, and sensitivity state.
        device->setProvider(currentProvider);
        device->setWidget(currentWidget);
        if (currentProvider) {
            device->updateFilters(currentProvider->filters());
            device->updateSensitivity(currentProvider->sensitivity());
        } else {
            device->updateFilters(QMouse3DEventProvider::Translations |
                                  QMouse3DEventProvider::Rotations);
            device->updateSensitivity(1.0f);
        }
    }
    emit availableChanged();
}

bool QMouse3DDeviceList::eventFilter(QObject *watched, QEvent *event)
{
    QWidget *widget = qobject_cast<QWidget *>(watched);
    QMouse3DEventProvider *provider = widgets.value(widget, 0);
    if (event->type() == QEvent::WindowActivate) {
        if (provider && widget != currentWidget)
            setWidget(provider, widget);
    } else if (event->type() == QEvent::WindowDeactivate) {
        if (provider && widget == currentWidget) {
            // Post a zero event to the deactivating widget to center
            // any actions that were in progress.
            QMouse3DEvent *mouse = new QMouse3DEvent(0, 0, 0, 0, 0, 0);
            QApplication::postEvent(widget, mouse);
            setWidget(0, 0);
        }
    }
    return false;
}

QT_END_NAMESPACE
