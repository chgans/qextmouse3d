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

#include "qmouse3ddevicelist_p.h"
#include "qmouse3ddeviceplugin_p.h"
#include <QtCore/private/qfactoryloader_p.h>
#include <QtCore/qlibraryinfo.h>
#include <QtGui/qwidget.h>
#include <QtGui/qapplication.h>

QT_BEGIN_NAMESPACE

#if !defined (QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
    (QExtMouse3DDeviceFactoryInterface_iid, QLatin1String("/mouse3d")))
#endif

QExtMouse3DDeviceList::QExtMouse3DDeviceList(QObject *parent)
    : QObject(parent)
    , currentWidget(0)
    , currentProvider(0)
{
    ref = 1;
    if (QExtMouse3DDevice::testDevice1) {
        // Special hook for auto-testing.
        devices.append(QExtMouse3DDevice::testDevice1);
        connect(QExtMouse3DDevice::testDevice1, SIGNAL(availableChanged()),
                this, SLOT(availableDeviceChanged()));
        if (QExtMouse3DDevice::testDevice2) {
            devices.append(QExtMouse3DDevice::testDevice2);
            connect(QExtMouse3DDevice::testDevice2,
                    SIGNAL(availableChanged()),
                    this, SIGNAL(availableChanged()));
        }
    } else {
#if !defined (QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
        QFactoryLoader *l = loader();
        QStringList keys = l->keys();
        for (int index = 0; index < keys.size(); ++index) {
            if (QExtMouse3DDeviceFactoryInterface *factory
                    = qobject_cast<QExtMouse3DDeviceFactoryInterface*>
                        (l->instance(keys.at(index)))) {
                QExtMouse3DDevice *device = factory->create();
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

QExtMouse3DDeviceList::~QExtMouse3DDeviceList()
{
}

void QExtMouse3DDeviceList::attachWidget
    (QExtMouse3DEventProvider *provider, QWidget *widget)
{
    // Add the (widget, provider) pair to the map.
    if (widgets.contains(widget)) {
        qWarning("QExtMouse3DEventProvider: multiple providers for single widget");
        return;
    }
    widgets.insert(widget, provider);

    // Install an event filter to track window activate/deactivate events.
    widget->installEventFilter(this);

    // If the widget is already active, then set it as the current widget.
    // Also do this if we are running the autotests because activating a
    // widget during autotests is unstable at best.
    if (widget->isActiveWindow() || QExtMouse3DDevice::testDevice1)
        setWidget(provider, widget);
}

void QExtMouse3DDeviceList::detachWidget
    (QExtMouse3DEventProvider *provider, QWidget *widget)
{
    // Check that this is the provider that was registered for the widget.
    QExtMouse3DEventProvider *prov = widgets.value(widget, 0);
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

void QExtMouse3DDeviceList::setWidget
    (QExtMouse3DEventProvider *provider, QWidget *widget)
{
    currentProvider = provider;
    currentWidget = widget;
    for (int index = 0; index < devices.size(); ++index) {
        QExtMouse3DDevice *device = devices.at(index);
        if (device->isAvailable()) {
            device->setProvider(provider);
            device->setWidget(widget);
            if (provider) {
                device->updateFilters(provider->filters());
                device->updateSensitivity(provider->sensitivity());
            } else {
                device->updateFilters(QExtMouse3DEventProvider::Translations |
                                      QExtMouse3DEventProvider::Rotations);
                device->updateSensitivity(1.0f);
            }
        }
    }
}

void QExtMouse3DDeviceList::updateFilters
    (QExtMouse3DEventProvider *provider, QExtMouse3DEventProvider::Filters value)
{
    if (currentProvider == provider) {
        for (int index = 0; index < devices.size(); ++index) {
            QExtMouse3DDevice *device = devices.at(index);
            if (device->isAvailable())
                device->updateFilters(value);
        }
    }
}

void QExtMouse3DDeviceList::updateSensitivity
    (QExtMouse3DEventProvider *provider, qreal value)
{
    if (currentProvider == provider) {
        for (int index = 0; index < devices.size(); ++index) {
            QExtMouse3DDevice *device = devices.at(index);
            if (device->isAvailable())
                device->updateSensitivity(value);
        }
    }
}

static QExtMouse3DDeviceList *deviceList = 0;

QExtMouse3DDeviceList *QExtMouse3DDeviceList::attach()
{
    if (!deviceList) {
        deviceList = new QExtMouse3DDeviceList();
        return deviceList;
    }
    deviceList->ref.ref();
    return deviceList;
}

void QExtMouse3DDeviceList::detach(QExtMouse3DDeviceList *list)
{
    if (!list->ref.deref()) {
        delete list;
        deviceList = 0;
    }
}

void QExtMouse3DDeviceList::availableDeviceChanged()
{
    QExtMouse3DDevice *device = qobject_cast<QExtMouse3DDevice *>(sender());
    if (device && device->isAvailable()) {
        // Newly available devices need to be told the provider,
        // widget, filter state, and sensitivity state.
        device->setProvider(currentProvider);
        device->setWidget(currentWidget);
        if (currentProvider) {
            device->updateFilters(currentProvider->filters());
            device->updateSensitivity(currentProvider->sensitivity());
        } else {
            device->updateFilters(QExtMouse3DEventProvider::Translations |
                                  QExtMouse3DEventProvider::Rotations);
            device->updateSensitivity(1.0f);
        }
    }
    emit availableChanged();
}

bool QExtMouse3DDeviceList::eventFilter(QObject *watched, QEvent *event)
{
    QWidget *widget = qobject_cast<QWidget *>(watched);
    QExtMouse3DEventProvider *provider = widgets.value(widget, 0);
    if (event->type() == QEvent::WindowActivate) {
        if (provider && widget != currentWidget)
            setWidget(provider, widget);
    } else if (event->type() == QEvent::WindowDeactivate) {
        if (provider && widget == currentWidget) {
            // Post a zero event to the deactivating widget to center
            // any actions that were in progress.
            QExtMouse3DEvent *mouse = new QExtMouse3DEvent(0, 0, 0, 0, 0, 0);
            QApplication::postEvent(widget, mouse);
            setWidget(0, 0);
        }
    }
    return false;
}

QT_END_NAMESPACE
