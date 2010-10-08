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

#include "qmouse3ddevice_p.h"
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
    the mouse to wrist movements.  If the operating system's
    3D mouse driver or the subclass takes care of this detail itself,
    then the subclass should call motion() with the filter argument
    set to false.

    If the driver or subclass does not handle locking itself, then
    call changeMode() whenever a special key is pressed and then
    call motion() with the filter argument set to true.  The motion()
    function will internally filter the event to take the current mode
    into account.

    \sa QMouse3DEvent, QMouse3DHandler
*/

class QMouse3DDevicePrivate
{
public:
    QMouse3DDevicePrivate()
        : widget(0)
        , flags(QMouse3DDevice::Mode_Translation |
                QMouse3DDevice::Mode_Rotation)
        , sensitivity(0.5f)
    {
    }

    QWidget *widget;
    int flags;
    qreal sensitivity;
};

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
    Changes the event filtering behavior of motion() in response
    to a special \a mode.  This is typically used with 3D mice that
    have buttons for locking the mouse into rotation mode,
    translation mode, to clamp the movement to the dominant axis,
    or to change the sensitivity of the mouse to wrist movements.

    \sa motion()
*/
void QMouse3DDevice::changeMode(int mode)
{
    Q_D(QMouse3DDevice);
    if (mode == Mode_Rotation) {
        d->flags ^= mode;
        if ((d->flags & (QMouse3DDevice::Mode_Translation |
                         QMouse3DDevice::Mode_Rotation)) == 0) {
            // If we turned off rotation and translation was already off,
            // then turn translation back on again.
            d->flags |= QMouse3DDevice::Mode_Translation;
        }
    } else if (mode == Mode_Translation) {
        d->flags ^= mode;
        if ((d->flags & (QMouse3DDevice::Mode_Translation |
                         QMouse3DDevice::Mode_Rotation)) == 0) {
            // If we turned off translation and rotation was already off,
            // then turn rotation back on again.
            d->flags |= QMouse3DDevice::Mode_Rotation;
        }
    } else if (mode == Mode_Dominant) {
        d->flags ^= mode;
    } else if (mode == Mode_IncreaseSensitivity) {
        d->sensitivity += 0.1f;
        if (d->sensitivity > 1.0f)
            d->sensitivity = 1.0f;
    } else if (mode == Mode_DecreaseSensitivity) {
        d->sensitivity -= 0.1f;
        if (d->sensitivity < 0.1f)
            d->sensitivity = 0.1f;
    }
}

/*!
    Delivers a 3D mouse \a event to widget().  If \a filter is true,
    then apply filtering for rotation-lock, translation-lock, dominant-lock,
    and mouse sensitivity.  Set \a filter to false if the 3D mouse event
    source has already filtered the data itself.

    \sa changeMode()
*/
void QMouse3DDevice::motion(QMouse3DEvent *event, bool filter)
{
    Q_D(QMouse3DDevice);
    if (!d->widget)
        return;
    if (filter) {
        qreal values[6];
        values[0] = event->translateX() * d->sensitivity;
        values[1] = event->translateY() * d->sensitivity;
        values[2] = event->translateZ() * d->sensitivity;
        values[3] = event->rotateX() * d->sensitivity;
        values[4] = event->rotateY() * d->sensitivity;
        values[5] = event->rotateZ() * d->sensitivity;
        if (!(d->flags & QMouse3DDevice::Mode_Translation)) {
            values[0] = 0.0f;
            values[1] = 0.0f;
            values[2] = 0.0f;
        }
        if (!(d->flags & QMouse3DDevice::Mode_Rotation)) {
            values[3] = 0.0f;
            values[4] = 0.0f;
            values[5] = 0.0f;
        }
        if (d->flags & QMouse3DDevice::Mode_Dominant) {
            int largest = 0;
            qreal value = qAbs(values[0]);
            for (int index = 1; index < 6; ++index) {
                qreal value2 = qAbs(values[index]);
                if (value2 > value) {
                    largest = index;
                    value = value2;
                }
            }
            for (int index = 0; index < 6; ++index) {
                if (index != largest)
                    values[index] = 0.0f;
            }
        }
        QMouse3DEvent ev(values[0], values[1], values[2],
                         values[3], values[4], values[5]);
        QApplication::sendEvent(d->widget, &ev);
    } else {
        QApplication::sendEvent(d->widget, event);
    }
}

QT_END_NAMESPACE
