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

#include "line.h"
#include "qglbuilder.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass Line Line
    \brief The Line item represents a geometric line drawn in 3D space.
    \since 4.8
    \ingroup qt3d::qml3d::shapes
    \inherits Item3D

    The Line element in QML provides a means of drawing lines in a 3D
    environment.  For example, the following QML code line between point
    (0,0,0) and (1,1,1).

    \code
    Line {
        startPoint: Qt.vector3D(0,0,0)
        endPoint: Qt.vector3D(1,1,1)
        effect: Effect {
            color: "#aaca00"
        }
    }
    \endcode

    The Line element is part of the \c{Qt3D.Shapes} namespace,
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
Line::Line(QObject *parent) :
    QDeclarativeItem3D(parent)
    , m_startPos(QVector3D(0.0f, 0.0f, 0.0f))
    , m_endPos(QVector3D(1.0f, 1.0f, 1.0f))
{
    //meh
}


/*!
    \qmlproperty real Line::setStartPoint

    This property defines the start point of the line.
    The default value is (0,0,0).
*/
void Line::setStartPoint(QVector3D startPos)
{
    if (m_startPos != startPos) {
        m_startPos = startPos;
        emit startPointChanged();
        update();
    }
}

/*!
    \qmlproperty real Line::setEndPoint

    This property defines the end point of the line.
    The default value is (1,1,1).
*/
void Line::setEndPoint(QVector3D startPos)
{
    if (m_startPos != startPos) {
        m_startPos = startPos;
        emit startPointChanged();
        update();
    }
}

/*!
    \qmlproperty real Line::setSize

    This property defines the width of the line.  The
    default is 1.0
*/
void Line::setWidth(qreal width)
{
    if (m_width != width) {
        m_width = width;
        emit widthChanged();
        update();
    }
}

/*!
    \internal
*/
void Line::drawItem(QGLPainter *painter)
{
    QVector3DArray vertices;
    vertices.append(m_startPos.x(), m_startPos.y(), m_startPos.z());
    vertices.append(m_endPos.x(), m_endPos.y(), m_endPos.z());
    painter->setVertexAttribute(QGL::Position, vertices);
    glLineWidth(m_width);
    painter->draw(QGL::Lines, 2);
}

