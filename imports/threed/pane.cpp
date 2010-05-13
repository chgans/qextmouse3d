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

#include "pane.h"

QT_BEGIN_NAMESPACE

/*!
    \class Pane
    \brief The Pane class represents a rectangular pane in 3D space that can be textured with an effect.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::qml3d

    \code
    Pane {
        width: 2
        height: 1.5
        effect: Effect { texture: "picture.jpg" }
    }
    \endcode
*/

/*!
    Constructs a new 3D pane and attaches it to \a parent.
*/
Pane::Pane(QObject *parent)
    : Item3d(parent)
    , m_width(1.0f)
    , m_height(1.0f)
    , m_twoSided(true)
    , m_orientation(Pane::Normal)
{
}

/*!
    Destroys this 3D pane.
*/
Pane::~Pane()
{
}

/*!
    \property Pane::width
    \brief the width of the pane.  The default value is 1.

    \sa height()
*/

qreal Pane::width() const
{
    return m_width;
}

void Pane::setWidth(qreal value)
{
    if (m_width != value) {
        m_width = value;
        emit widthChanged();
    }
}

/*!
    \property Pane::height
    \brief the height of the pane.  The default value is 1.

    \sa width()
*/

qreal Pane::height() const
{
    return m_height;
}

void Pane::setHeight(qreal value)
{
    if (m_height != value) {
        m_height = value;
        emit heightChanged();
    }
}

/*!
    \property Pane::twoSided
    \brief the two-sided state.

    If false, then lit materials will always use a normal that faces
    forward with respect to the pane, which will make the back side
    appear dark.  If true, then lit materials will always use a normal
    that faces towards the viewer so that both sides appear lit.

    The default value is true.
*/

bool Pane::twoSided() const
{
    return m_twoSided;
}

void Pane::setTwoSided(bool value)
{
    if (m_twoSided != value) {
        m_twoSided = value;
        emit twoSidedChanged();
    }
}

/*!
    \enum Pane::Orientation
    This enum defines the orientation of the texture co-ordinates
    for the image displayed on a pane.

    \value Normal Normal orientation with (0, 0) at the bottom-left.
    \value Rot90 Rotate 90 degrees anti-clockwise.
    \value Rot180 Rotate 180 degrees.
    \value Rot270 Rotate 270 degrees anti-clockwise.
*/

/*!
    \property Pane::orientation
    \brief the orientation of the texture co-ordinates for the
    image displayed on the pane.  The default value is Normal.
*/

Pane::Orientation Pane::orientation() const
{
    return m_orientation;
}

void Pane::setOrientation(Pane::Orientation value)
{
    if (m_orientation != value) {
        m_orientation = value;
        emit orientationChanged();
    }
}

/*!
    \internal
*/
void Pane::drawItem(QGLPainter *painter)
{
    static float const texCoordsNormal[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };
    static float const texCoordsRot90[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
    };
    static float const texCoordsRot180[] = {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
    };
    static float const texCoordsRot270[] = {
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
    };
    static float const normalsTowards[] = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f
    };
    static float const normalsAway[] = {
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f
    };
    float vertices[] = {
        -m_width / 2.0f,
        -m_height / 2.0f,
        -m_width / 2.0f,
         m_height / 2.0f,
         m_width / 2.0f,
         m_height / 2.0f,
        -m_width / 2.0f,
        -m_height / 2.0f,
         m_width / 2.0f,
         m_height / 2.0f,
         m_width / 2.0f,
        -m_height / 2.0f
    };
    painter->setVertexAttribute
        (QGL::Position, QGLAttributeValue(2, GL_FLOAT, 0, vertices));
    if (!m_twoSided) {
        painter->setVertexAttribute
            (QGL::Normal, QGLAttributeValue(3, GL_FLOAT, 0, normalsTowards));
    } else {
        QVector3D normal = painter->modelViewMatrix().top().mapVector(QVector3D(0, 0, 1));
        if (normal.z() >= 0.0f) {
            painter->setVertexAttribute
                (QGL::Normal, QGLAttributeValue(3, GL_FLOAT, 0, normalsTowards));
        } else {
            painter->setVertexAttribute
                (QGL::Normal, QGLAttributeValue(3, GL_FLOAT, 0, normalsAway));
        }
    }
    switch (m_orientation) {
    case Rot90:
        painter->setVertexAttribute
            (QGL::TextureCoord0,
             QGLAttributeValue(2, GL_FLOAT, 0, texCoordsRot90));
        break;
    case Rot180:
        painter->setVertexAttribute
            (QGL::TextureCoord0,
             QGLAttributeValue(2, GL_FLOAT, 0, texCoordsRot180));
        break;
    case Rot270:
        painter->setVertexAttribute
            (QGL::TextureCoord0,
             QGLAttributeValue(2, GL_FLOAT, 0, texCoordsRot270));
        break;
    default:
        painter->setVertexAttribute
            (QGL::TextureCoord0,
             QGLAttributeValue(2, GL_FLOAT, 0, texCoordsNormal));
        break;
    }
    painter->draw(QGL::Triangles, 6);
}

/*!
    \fn void Pane::widthChanged()

    Signal that is emitted when width() changes.
*/

/*!
    \fn void Pane::heightChanged()

    Signal that is emitted when height() changes.
*/

/*!
    \fn void Pane::twoSidedChanged()

    Signal that is emitted when twoSided() changes.
*/

/*!
    \fn void Pane::orientationChanged()

    Signal that is emitted when orientation() changes.
*/

QT_END_NAMESPACE
