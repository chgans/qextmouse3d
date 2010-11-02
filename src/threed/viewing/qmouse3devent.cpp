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

#include "qmouse3devent.h"

QT_BEGIN_NAMESPACE

/*!
    \class QMouse3DEvent
    \brief The QMouse3DEvent class represents a motion in 3D space that was reported by a "3D mouse".
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::viewing

    3D mice usually have six degrees of freedom of motion, corresponding
    to translations along the X, Y, and Z axes, and rotations around
    each of those axes.  QMouse3DEvent provides a way to encapsulate
    these values for reporting the output of a 3D mouse to a QWidget.

    The translation components are given by translateX(), translateY(),
    and translateZ(), in the range -32768 to 32767.  A value of -32768
    on the X axis for example indicates that the 3D mouse is in the maximum
    left-most position at the highest sensitivity.  The rotation components
    are given by rotateX(), rotateY(), and rotateZ(), and also fall within
    the range -32768 to 32767.

    The mouse is assumed to automatically self-center back to zero on all
    axes in the manner of a joystick.

    Applications use QMouse3DEventProvider to register a QWidget to
    receive 3D mouse events.  The events will arrive via the
    QObject::event() override in the widget subclass:

    \code
    MyWidget::MyWidget(QWidget *parent)
        : QWidget(parent)
    {
        QMouse3DEventProvider *provider;
        provider = new QMouse3DEventProvider(this);
        provider->setWidget(this);
    }

    bool MyWidget::event(QEvent *e)
    {
        if (e->type() == QMouse3DEvent::type) {
            QMouse3DEvent *mouse = static_cast<QMouse3DEvent *>(e);
            ...
            return true;
        }
        return QWidget::event(e);
    }
    \endcode

    \sa QMouse3DEventProvider
*/

/*!
    \fn QMouse3DEvent::QMouse3DEvent(short translateX, short translateY, short translateZ, short rotateX, short rotateY, short rotateZ)

    Constructs an event representing a motion in 3D space for the six degrees
    of freedom given by \a translateX, \a translateY, \a translateZ,
    \a rotateX, \a rotateY, and \a rotateZ.
*/

/*!
    Destroys this 3D mouse event.
*/
QMouse3DEvent::~QMouse3DEvent()
{
}

/*!
    \variable QMouse3DEvent::type

    This constant defines the QEvent::type() for 3D mouse events.
    It is typically used inside a QWidget::event() override in a
    subclass to detect 3D mouse events.
*/
const QEvent::Type QMouse3DEvent::type = QEvent::Type(750);

/*!
    \fn short QMouse3DEvent::translateX() const

    Returns the X axis translation value for this motion in 3D space.

    Increasing values of X correspond to the mouse moving right,
    and decreasing values of X correspond to the mouse moving left.
    The mouse co-ordinate system is left-handed.

    \sa translateY(), translateZ(), rotateX()
*/

/*!
    \fn short QMouse3DEvent::translateY() const

    Returns the Y axis translation value for this motion in 3D space.

    Increasing values of Y correspond to the mouse moving down,
    and decreasing values of Y correspond to the mouse moving up.
    The mouse co-ordinate system is left-handed.

    \sa translateX(), translateZ(), rotateY()
*/

/*!
    \fn short QMouse3DEvent::translateZ() const

    Returns the Z axis translation value for this motion in 3D space.

    Increasing values of Z correspond to the mouse knob being pushed
    down, and decreasing values of Z correspond to the mouse knob
    being pulled up.  The mouse co-ordinate system is left-handed.

    \sa translateX(), translateY(), rotateZ()
*/

/*!
    \fn short QMouse3DEvent::rotateX() const

    Returns the X axis rotation value for this motion in 3D space.

    Increasing values of X rotation correspond to the mouse being
    rocked downwards, and decreasing values of X rotation correspond
    to the mouse being rocked upwards.

    \sa rotateY(), rotateZ(), translateX()
*/

/*!
    \fn short QMouse3DEvent::rotateY() const

    Returns the Y axis rotation value for this motion in 3D space.

    Increasing values of Y rotation correspond to the mouse being
    rocked leftwards, and decreasing values of Y rotation correspond
    to the mouse being rocked rightwards.

    \sa rotateX(), rotateZ(), translateY()
*/

/*!
    \fn short QMouse3DEvent::rotateZ() const

    Returns the Z axis rotation value for this motion in 3D space.

    Increasing values of Z rotation correspond to the mouse being
    twisted rightwards, and decreasing values of Z rotation correspond
    to the mouse being twisted leftwards.

    \sa rotateX(), rotateY(), translateZ()
*/

QT_END_NAMESPACE
