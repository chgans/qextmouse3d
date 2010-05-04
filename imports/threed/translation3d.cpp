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

#include "translation3d.h"

QT_BEGIN_NAMESPACE

/*!
    \class Translation3D
    \brief The Translation3D class contains parameters and functions required to support 3d translation of items in the QML/3D environment.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::qml3d

    \section1 Properties

    Translation3D is derived directly from QGraphicsTransform, and
    provides a \c translate property to specify the 3D vector to
    apply to transform incoming co-ordinates.

    \sa Rotation3D, Scale3D
*/

/*!
    Constructs a new translation and attaches it to \a parent.
*/
Translation3D::Translation3D(QObject *parent)
    : QGraphicsTransform(parent)
{
}

/*!
    Destroys this translation.
*/
Translation3D::~Translation3D()
{
}

/*!
    \property Translation3D::translate
    \brief the translation to apply to incoming co-ordinates.

    The default value for this property is (0, 0, 0).
*/

void Translation3D::setTranslate(const QVector3D &value)
{
    if (m_translate != value) {
        m_translate = value;
        update();
        emit translateChanged();
    }
}

/*!
    \property Translation3D::xTranslate
    \brief the x component of the translation to apply to
    incoming co-ordinates.

    The default value for this property is 0.
*/

void Translation3D::setXTranslate(qreal value)
{
    if (m_translate.x() != value) {
        m_translate.setX(value);
        update();
        emit translateChanged();
    }
}

/*!
    \property Translation3D::yTranslate
    \brief the y component of the translation to apply to
    incoming co-ordinates.

    The default value for this property is 0.
*/

void Translation3D::setYTranslate(qreal value)
{
    if (m_translate.y() != value) {
        m_translate.setY(value);
        update();
        emit translateChanged();
    }
}

/*!
    \property Translation3D::zTranslate
    \brief the z component of the translation to apply to
    incoming co-ordinates.

    The default value for this property is 0.
*/

void Translation3D::setZTranslate(qreal value)
{
    if (m_translate.z() != value) {
        m_translate.setZ(value);
        update();
        emit translateChanged();
    }
}

/*!
    \internal
*/
void Translation3D::applyTo(QMatrix4x4 *matrix) const
{
    matrix->translate(m_translate);
}

/*!
    \fn void Translation3D::translateChanged()

    Signal that is emitted when translate() changes.
*/
