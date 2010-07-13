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

#include "qgraphicstranslation3d.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGraphicsTranslation3D
    \brief The QGraphicsTranslation3D class supports translation of 3D items.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::graphicsview
    \ingroup qt3d::qml3d

    \section1 Properties

    QGraphicsTranslation3D is derived directly from QGraphicsTransform, and
    provides a \c translate property to specify the 3D vector to
    apply to the incoming co-ordinates.

    The \a progress property can be used to perform animation along a
    translation vector by varying the progress value between 0 and 1.
    Overshoot animations are also possible by setting the progress
    value to something outside this range.  The default progress
    value is 1.

    \sa QGraphicsRotation3D, QGraphicsScale3D
*/

class QGraphicsTranslation3DPrivate
{
public:
    QGraphicsTranslation3DPrivate() : progress(1.0f) {}

    QVector3D translate;
    qreal progress;
};

/*!
    Constructs a new translation and attaches it to \a parent.
*/
QGraphicsTranslation3D::QGraphicsTranslation3D(QObject *parent)
    : QGraphicsTransform(parent), d_ptr(new QGraphicsTranslation3DPrivate)
{
}

/*!
    Destroys this translation.
*/
QGraphicsTranslation3D::~QGraphicsTranslation3D()
{
}

/*!
    \property QGraphicsTranslation3D::translate
    \brief the translation to apply to incoming co-ordinates.

    The default value for this property is (0, 0, 0).
*/

QVector3D QGraphicsTranslation3D::translate() const
{
    Q_D(const QGraphicsTranslation3D);
    return d->translate;
}

void QGraphicsTranslation3D::setTranslate(const QVector3D &value)
{
    Q_D(QGraphicsTranslation3D);
    if (d->translate != value) {
        d->translate = value;
        update();
        emit translateChanged();
    }
}

/*!
    \property QGraphicsTranslation3D::xTranslate
    \brief the x component of the translation to apply to
    incoming co-ordinates.

    The default value for this property is 0.
*/

qreal QGraphicsTranslation3D::xTranslate() const
{
    Q_D(const QGraphicsTranslation3D);
    return d->translate.x();
}

void QGraphicsTranslation3D::setXTranslate(qreal value)
{
    Q_D(QGraphicsTranslation3D);
    if (d->translate.x() != value) {
        d->translate.setX(value);
        update();
        emit translateChanged();
    }
}

/*!
    \property QGraphicsTranslation3D::yTranslate
    \brief the y component of the translation to apply to
    incoming co-ordinates.

    The default value for this property is 0.
*/

qreal QGraphicsTranslation3D::yTranslate() const
{
    Q_D(const QGraphicsTranslation3D);
    return d->translate.y();
}

void QGraphicsTranslation3D::setYTranslate(qreal value)
{
    Q_D(QGraphicsTranslation3D);
    if (d->translate.y() != value) {
        d->translate.setY(value);
        update();
        emit translateChanged();
    }
}

/*!
    \property QGraphicsTranslation3D::zTranslate
    \brief the z component of the translation to apply to
    incoming co-ordinates.

    The default value for this property is 0.
*/

qreal QGraphicsTranslation3D::zTranslate() const
{
    Q_D(const QGraphicsTranslation3D);
    return d->translate.z();
}

void QGraphicsTranslation3D::setZTranslate(qreal value)
{
    Q_D(QGraphicsTranslation3D);
    if (d->translate.z() != value) {
        d->translate.setZ(value);
        update();
        emit translateChanged();
    }
}

/*!
    \property QGraphicsTranslation3D::progress
    \brief the progress along the translation vector, from 0 to 1.

    The default value for this property is 1.

    This property can be used to perform animation along a translation
    vector by varying the progress between 0 and 1.  Overshoot animations
    are also possible by setting the value to something outside this range.
*/

qreal QGraphicsTranslation3D::progress() const
{
    Q_D(const QGraphicsTranslation3D);
    return d->progress;
}

void QGraphicsTranslation3D::setProgress(qreal value)
{
    Q_D(QGraphicsTranslation3D);
    if (d->progress != value) {
        d->progress = value;
        update();
        emit progressChanged();
    }
}

/*!
    \internal
*/
void QGraphicsTranslation3D::applyTo(QMatrix4x4 *matrix) const
{
    Q_D(const QGraphicsTranslation3D);
    matrix->translate(d->translate * d->progress);
}

/*!
    \fn void QGraphicsTranslation3D::translateChanged()

    Signal that is emitted when translate() changes.
*/

/*!
    \fn void QGraphicsTranslation3D::progressChanged()

    Signal that is emitted when progress() changes.
*/

QT_END_NAMESPACE
