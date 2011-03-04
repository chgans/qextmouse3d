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

#include "qmouse3ddevice_p.h"
#include "qmouse3ddevicelist_p.h"
#include <QtGui/qapplication.h>
#include <QtGui/qwidget.h>
#include <QtGui/qevent.h>

QT_BEGIN_NAMESPACE

/*!
    \class QMouse3DDevice
    \brief The QMouse3DDevice class manages a single 3D mouse device.
    \internal
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::viewing

    This is an internal class, for use by implementors of 3D mouse
    plug-ins.  The API may change in future versions.

    3D mice can deliver three kinds of events via the provided
    helper functions: a motion() in 3D space, a keyPress() event,
    or a keyRelease() event.

    QMouse3DHandler takes care of determining which widget should
    receive 3D mouse events if multiple widgets have been registered
    by the application.  The device only deals with a single widget
    at a time, which is specified by calls to setWidget().

    The subclass can override setWidget() to install or remove event
    processing hooks on the widget.  If the widget is set to null,
    then 3D mouse event delivery should be disabled for this
    application.

    Certain special keys on 3D mice can be used for locking the
    mouse into rotation mode, translation mode, to clamp the
    movement to the dominant axis, or to change the sensitivity of
    the mouse to wrist movements.

    The subclass should call toggleFilter() or adjustSensitivity()
    whenever a special key is pressed.  The motion() function will
    internally filter the event to take the current filters into account.

    \sa QMouse3DEvent, QMouse3DHandler
*/

class QMouse3DDevicePrivate
{
public:
    QMouse3DDevicePrivate()
        : widget(0)
        , provider(0)
    {
    }

    QWidget *widget;
    QMouse3DEventProvider *provider;
};

QMouse3DDevice *QMouse3DDevice::testDevice1 = 0;
QMouse3DDevice *QMouse3DDevice::testDevice2 = 0;

/*!
    Constructs a 3D mouse device and attaches it to \a parent.
*/
QMouse3DDevice::QMouse3DDevice(QObject *parent)
    : QObject(parent)
    , d_ptr(new QMouse3DDevicePrivate)
{
}

/*!
    Destroys this 3D mouse device.
*/
QMouse3DDevice::~QMouse3DDevice()
{
}

/*!
    \fn bool QMouse3DDevice::isAvailable() const

    Returns true if this device is available to deliver 3D mouse events.

    \sa name()
*/

/*!
    \fn QStringList QMouse3DDevice::deviceNames() const

    Returns the names the low-level 3D mouse devices that are being
    managed by this device object, and which are currently available.
    This value is intended for debugging purposes.

    Multiple names can be provided because the device object could
    be handling multiple instances of the same type of device.

    \sa isAvailable()
*/

/*!
    Returns the event provider that is currently associated
    with this mouse device and widget().  This can be used to
    access filtering properties from QMouse3DEventProvider.

    \sa setProvider()
*/
QMouse3DEventProvider *QMouse3DDevice::provider() const
{
    Q_D(const QMouse3DDevice);
    return d->provider;
}

/*!
    Sets the event \a provider that should be currently associated
    with this mouse device and widget().

    \sa provider()
*/
void QMouse3DDevice::setProvider(QMouse3DEventProvider *provider)
{
    Q_D(QMouse3DDevice);
    d->provider = provider;
}

/*!
    Returns the widget that should receive 3D mouse events from
    this device; null if 3D mouse event delivery is currently disabled.

    \sa setWidget()
*/
QWidget *QMouse3DDevice::widget() const
{
    Q_D(const QMouse3DDevice);
    return d->widget;
}

/*!
    Sets the \a widget that should receive 3D mouse events from
    this device.  Event delivery to a previous widget should be
    discontinued.  If \a widget is null then 3D mouse events
    from this device should be disabled.

    \sa widget()
*/
void QMouse3DDevice::setWidget(QWidget *widget)
{
    Q_D(QMouse3DDevice);
    d->widget = widget;
}

/*!
    Notifies the subclass that QMouse3DEventProvider::filters()
    has changed to \a filters.  The default implementation
    does nothing.

    If the 3D mouse device has state LED's or an LCD screen,
    then the subclass may override this function to update the
    state whenever the filters change in the application.

    \sa updateSensitivity()
*/
void QMouse3DDevice::updateFilters(QMouse3DEventProvider::Filters filters)
{
    Q_UNUSED(filters);
}

/*!
    Notifies the subclass that the QMouse3DEventProvider::sensitivity()
    has changed to \a sensitivity.  The default implementation
    does nothing.

    If the 3D mouse device has state LED's or an LCD screen,
    then the subclass may override this function to update the
    state whenever the sensitivity changes in the application.

    \sa updateFilters()
*/
void QMouse3DDevice::updateSensitivity(qreal sensitivity)
{
    Q_UNUSED(sensitivity);
}

/*!
    Delivers a key press event to widget() for \a key.  Any of the key codes
    from Qt::Key or QGL::Mouse3DKeys may be passed to this function.

    \sa keyRelease()
*/
void QMouse3DDevice::keyPress(int key)
{
    Q_D(QMouse3DDevice);
    if (d->widget) {
        QKeyEvent event(QEvent::KeyPress, key, Qt::NoModifier);
        QApplication::sendEvent(d->widget, &event);
    }
}

/*!
    Delivers a key release event to widget() for \a key.  Any of the key codes
    from Qt::Key or QGL::Mouse3DKeys may be passed to this function.

    \sa keyPress()
*/
void QMouse3DDevice::keyRelease(int key)
{
    Q_D(QMouse3DDevice);
    if (d->widget) {
        QKeyEvent event(QEvent::KeyRelease, key, Qt::NoModifier);
        QApplication::sendEvent(d->widget, &event);
    }
}

/*!
    Toggles the specified \a filter option on provider(), if permitted
    by QMouse3DEventProvider::keyFilters().
*/
void QMouse3DDevice::toggleFilter(QMouse3DEventProvider::Filter filter)
{
    Q_D(QMouse3DDevice);
    if (d->provider && (d->provider->keyFilters() & filter) != 0)
        d->provider->toggleFilter(filter);
}

/*!
    Adjusts the sensitivity value on provider() by multiplying
    it by \a factor, if permitted by QMouse3DEventProvider::keyFilters().
*/
void QMouse3DDevice::adjustSensitivity(qreal factor)
{
    Q_D(QMouse3DDevice);
    if (d->provider &&
            (d->provider->keyFilters() &
                    QMouse3DEventProvider::Sensitivity) != 0) {
        d->provider->setSensitivity(d->provider->sensitivity() * factor);
    }
}

static inline short clampRange(int value)
{
    return short(qMin(qMax(value, -32768), 32767));
}

/*!
    Delivers a 3D mouse \a event to widget() after applying filtering for
    rotation-lock, translation-lock, dominant-lock, and mouse sensitivity.
*/
void QMouse3DDevice::motion(QMouse3DEvent *event)
{
    Q_D(QMouse3DDevice);
    if (!d->widget || !d->provider)
        return;
    int values[6];
    qreal sensitivity = d->provider->sensitivity();
    QMouse3DEventProvider::Filters filters = d->provider->filters();
    if ((filters & QMouse3DEventProvider::Sensitivity) != 0) {
        values[0] = int(event->translateX() * sensitivity);
        values[1] = int(event->translateY() * sensitivity);
        values[2] = int(event->translateZ() * sensitivity);
        values[3] = int(event->rotateX() * sensitivity);
        values[4] = int(event->rotateY() * sensitivity);
        values[5] = int(event->rotateZ() * sensitivity);
    } else {
        values[0] = event->translateX();
        values[1] = event->translateY();
        values[2] = event->translateZ();
        values[3] = event->rotateX();
        values[4] = event->rotateY();
        values[5] = event->rotateZ();
    }
    if (!(filters & QMouse3DEventProvider::Translations)) {
        values[0] = 0;
        values[1] = 0;
        values[2] = 0;
    }
    if (!(filters & QMouse3DEventProvider::Rotations)) {
        values[3] = 0;
        values[4] = 0;
        values[5] = 0;
    }
    if (filters & QMouse3DEventProvider::DominantAxis) {
        int largest = 0;
        int value = qAbs(values[0]);
        for (int index = 1; index < 6; ++index) {
            int value2 = qAbs(values[index]);
            if (value2 > value) {
                largest = index;
                value = value2;
            }
        }
        for (int index = 0; index < 6; ++index) {
            if (index != largest)
                values[index] = 0;
        }
    }
    QMouse3DEvent ev(clampRange(values[0]),
                     clampRange(values[1]),
                     clampRange(values[2]),
                     clampRange(values[3]),
                     clampRange(values[4]),
                     clampRange(values[5]));
    QApplication::sendEvent(d->widget, &ev);
}

QT_END_NAMESPACE
