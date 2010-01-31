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

#include "qglsimulator.h"

QGLSimulator::QGLSimulator(QPainter *painter, const QSize& size)
    : m_painter(painter),
      m_size(size),
      m_clearColor(Qt::black),
      m_viewport(0, 0, size.width(), size.height())
{
}

QGLSimulator::~QGLSimulator()
{
}

void QGLSimulator::clear()
{
    m_painter->fillRect(0, 0, m_size.width(), m_size.height(), m_clearColor);
}

void QGLSimulator::setScissor(int x, int y, int width, int height)
{
    QRect rect(x, height - 1 - y, width, height);
    m_painter->setClipRect(rect);
}

void QGLSimulator::clearScissor()
{
    m_painter->setClipping(false);
}

void QGLSimulator::setViewport(int x, int y, int width, int height)
{
    m_viewport = QRect(x, y, width, height);
}

// Set the flat color for drawing triangles.
void QGLSimulator::setColor(const QColor& color)
{
    m_painter->setBrush(QBrush(color));
    m_painter->setPen(QPen());
}

// Draw a set of triangles from a vertex array.
void QGLSimulator::drawTriangles(const QGLVertexArray& array)
{
    QVector<QPointF> points;

    int field = array.fieldIndex(QGL::Position);
    if (field == -1)
        return;
    int size = array.fields().fieldSize(field);

    for (int index = 0; index < array.vertexCount(); ++index) {
        QVector3D v;
        if (size == 3)
            v = array.vector3DAt(index, field);
        else
            v = array.vector2DAt(index, field);
        points.append(project(v));
    }

    m_painter->drawPolygon(points);
}

// Project a 3D point to 2D and convert to left-handed QPainter co-ordinates.
QPointF QGLSimulator::project(const QVector3D& vector) const
{
    QVector3D pt = (m_projectionMatrix * m_modelViewMatrix) * vector;
    qreal x = (pt.x() + 1.0f) * m_viewport.width() * 0.5f + m_viewport.x();
    qreal y = (pt.y() + 1.0f) * m_viewport.height() * 0.5f + m_viewport.y();
    y = (m_viewport.height() - 1) - y;
    return QPointF(x, y);
}
