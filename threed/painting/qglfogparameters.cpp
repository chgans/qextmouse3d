/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qglfogparameters.h"
#include <QtCore/private/qobject_p.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLFogParameters
    \brief The QGLFogParameters class controls the fog parameters for a scene.
    \ingroup qt3d
    \ingroup qt3d::painting

    When fog is enabled, it affects all fragments that are drawn to the screen.
*/

/*!
    \enum QGLFogParameters::Mode
    This enum defines the type of fog to apply with a QGLFogParameters object.

    \value Linear Apply a linear fog calculation.  This is the default.
    \value Exponential Apply an exponential fog calculation.
    \value Exponential2 Apply a squared exponential fog calculation.
*/

class QGLFogParametersPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QGLFogParameters)
public:
    QGLFogParametersPrivate(int version = QObjectPrivateVersion) :
        QObjectPrivate(version),
        mode(QGLFogParameters::Linear),
        density(1.0f),
        nearDistance(0.0f),
        farDistance(1.0f),
        color(0, 0, 0, 0)
    {
    }

    QGLFogParameters::Mode mode;
    qreal density;
    qreal nearDistance;
    qreal farDistance;
    QColor color;
};

/*!
    Constructs a new fog parameters object with default values
    and attaches it to \a parent.
*/
QGLFogParameters::QGLFogParameters(QObject *parent)
    : QObject(*new QGLFogParametersPrivate(), parent)
{
}

/*!
    Destroys this fog parameters object.
*/
QGLFogParameters::~QGLFogParameters()
{
}

/*!
    \property QGLFogParameters::mode
    \brief the current fog mode.  The default is Linear.

    \sa density(), nearDistance(), farDistance(), color()
    \sa modeChanged()
*/
QGLFogParameters::Mode QGLFogParameters::mode() const
{
    Q_D(const QGLFogParameters);
    return d->mode;
}

void QGLFogParameters::setMode(QGLFogParameters::Mode value)
{
    Q_D(QGLFogParameters);
    if (d->mode != value) {
        d->mode = value;
        emit modeChanged();
        emit fogChanged();
    }
}

/*!
    \property QGLFogParameters::density
    \brief the current fog density.  The default value is 1.
    The fog denisty is used when mode() is Exponential or Exponential2.
    It is ignored when mode() is Linear.

    \sa mode(), densityChanged()
*/
qreal QGLFogParameters::density() const
{
    Q_D(const QGLFogParameters);
    return d->density;
}

void QGLFogParameters::setDensity(qreal value)
{
    Q_D(QGLFogParameters);
    if (d->density != value) {
        d->density = value;
        emit densityChanged();
        emit fogChanged();
    }
}

/*!
    \property QGLFogParameters::nearDistance
    \brief the near distance that is used in Linear fog calculations.
    The near distance is ignored if mode() is Exponential or Exponential2.
    The default value is 0.

    \sa farDistance(), mode(), nearDistanceChanged()
*/
qreal QGLFogParameters::nearDistance() const
{
    Q_D(const QGLFogParameters);
    return d->nearDistance;
}

void QGLFogParameters::setNearDistance(qreal value)
{
    Q_D(QGLFogParameters);
    if (d->nearDistance != value) {
        d->nearDistance = value;
        emit nearDistanceChanged();
        emit fogChanged();
    }
}

/*!
    \property QGLFogParameters::farDistance
    \brief the far distance that is used in Linear fog calculations.
    The far distance is ignored if mode() is Exponential or Exponential2.
    The default value is 1.

    \sa nearDistance(), mode(), farDistanceChanged()
*/
qreal QGLFogParameters::farDistance() const
{
    Q_D(const QGLFogParameters);
    return d->farDistance;
}

void QGLFogParameters::setFarDistance(qreal value)
{
    Q_D(QGLFogParameters);
    if (d->farDistance != value) {
        d->farDistance = value;
        emit farDistanceChanged();
        emit fogChanged();
    }
}

/*!
    \property QGLFogParameters::color
    \brief the fog color.  The default value is (0, 0, 0, 0).

    \sa mode(), colorChanged()
*/
QColor QGLFogParameters::color() const
{
    Q_D(const QGLFogParameters);
    return d->color;
}

void QGLFogParameters::setColor(const QColor& value)
{
    Q_D(QGLFogParameters);
    if (d->color != value) {
        d->color = value;
        emit colorChanged();
        emit fogChanged();
    }
}

/*!
    \fn void QGLFogParameters::modeChanged()

    This signal is emitted when mode() changes.

    \sa mode(), fogChanged()
*/

/*!
    \fn void QGLFogParameters::densityChanged()

    This signal is emitted when density() changes.

    \sa density(), fogChanged()
*/

/*!
    \fn void QGLFogParameters::nearDistanceChanged()

    This signal is emitted when nearDistance() changes.

    \sa nearDistance(), fogChanged()
*/

/*!
    \fn void QGLFogParameters::farDistanceChanged()

    This signal is emitted when farDistance() changes.

    \sa farDistance(), fogChanged()
*/

/*!
    \fn void QGLFogParameters::colorChanged()

    This signal is emitted when color() changes.

    \sa color(), fogChanged()
*/

/*!
    \fn void QGLFogParameters::fogChanged()

    This signal is emitted when any of the fog parameters change.
*/

QT_END_NAMESPACE
