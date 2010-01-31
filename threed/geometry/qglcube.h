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

#ifndef QGLCUBE_H
#define QGLCUBE_H

#include "qt3dglobal.h"
#include <QtGui/qvector2d.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLDisplayList;

class Q_QT3D_EXPORT QGLCube
{
public:
    explicit QGLCube(qreal size = 1.0f) : m_size(size) {}

    qreal size() const { return m_size; }
    void setSize(qreal size) { m_size = size; }

private:
    qreal m_size;
};

class Q_QT3D_EXPORT QGLCubeFace
{
public:
    enum Face
    {
        Left,
        Top,
        Right,
        Bottom,
        Front,
        Back
    };

    explicit QGLCubeFace(QGLCubeFace::Face face, qreal size = 1.0f)
        : m_size(size), m_face(face),
          m_bottomLeftTextureCoord(0.0f, 0.0f),
          m_topRightTextureCoord(1.0f, 1.0f) {}

    qreal size() const { return m_size; }
    void setSize(qreal size) { m_size = size; }

    QGLCubeFace::Face face() const { return m_face; }
    void setFace(QGLCubeFace::Face face) { m_face = face; }

    QVector2D bottomLeftTextureCoord() const
        { return m_bottomLeftTextureCoord; }
    void setBottomLeftTextureCoord(const QVector2D& value)
        { m_bottomLeftTextureCoord = value; }
    void setBottomLeftTextureCoord(qreal x, qreal y)
        { m_bottomLeftTextureCoord = QVector2D(x, y); }

    QVector2D topRightTextureCoord() const
        { return m_topRightTextureCoord; }
    void setTopRightTextureCoord(const QVector2D& value)
        { m_topRightTextureCoord = value; }
    void setTopRightTextureCoord(qreal x, qreal y)
        { m_topRightTextureCoord = QVector2D(x, y); }

private:
    qreal m_size;
    QGLCubeFace::Face m_face;
    QVector2D m_bottomLeftTextureCoord;
    QVector2D m_topRightTextureCoord;
};

Q_QT3D_EXPORT QGLDisplayList& operator<<(QGLDisplayList& list, const QGLCube& cube);
Q_QT3D_EXPORT QGLDisplayList& operator<<(QGLDisplayList& list, const QGLCubeFace& face);

QT_END_NAMESPACE

QT_END_HEADER

#endif
