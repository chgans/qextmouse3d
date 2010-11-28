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

#include "point.h"
#include "qglbuilder.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass Point Point
    \brief The Line item represents a geometric point drawn in 3D space.
    \since 4.8
    \ingroup qt3d::qml3d::shapes
    \inherits Item3D

    The Point element in QML provides a means of drawing points in a 3D
    environment.  For example, the following QML code line at point (0,0,0).

    \code
    Point {
        location: Qt.vector3D(0,0,0)
        effect: Effect {
            color: "#aaca00"
        }
    }
    \endcode

    The Point element is part of the \c{Qt3D.Shapes} namespace,
    so the following must appear at the top of any QML file that
    references it:

    \code
    import Qt3D.Shapes 1.0
    \endcode

    \sa Item3D
*/
/*!
    \internal
*/
Point::Point(QObject *parent) :
    QDeclarativeItem3D(parent)
    , m_location(QVector3D(0.0f, 0.0f, 0.0f))
    , m_pointSize(1.0f)
{
    //meh
}


/*!
    \qmlproperty QVector3D Point::location

    This property defines the locaiton of the point to be drawn.
    The default value is (0,0,0).
*/
void Point::setLocation(QVector3D location)
{
    if (m_location != location) {
        m_location = location;
        emit locationChanged();
        update();
    }
}

/*!
    \qmlproperty qreal Point::pointSize

    This property defines the size of the point.  The
    default is 1.0
*/
void Point::setPointSize(qreal pointSize)
{
    if (m_pointSize != pointSize) {
        m_pointSize = pointSize;
        emit pointSizeChanged();
        update();
    }
}

/*!
    \internal
*/
void Point::drawItem(QGLPainter *painter)
{
    QVector3DArray vertices;
    vertices.append(m_location.x(), m_location.y(), m_location.z());
    painter->setVertexAttribute(QGL::Position, vertices);
    glPointSize(m_pointSize);
    painter->draw(QGL::Points, 1);
}

