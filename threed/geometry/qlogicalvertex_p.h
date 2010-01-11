/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#ifndef QLOGICALVERTEX_H
#define QLOGICALVERTEX_H

#include <QtGui/qvector3d.h>
#include <QtGui/qvector2d.h>

#include "qcolor4b.h"

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

QT_BEGIN_NAMESPACE

class QLogicalVertex
{
public:
    static const qreal InvalidTexValue;
    static const QVector2D InvalidTexCoord;

    inline QLogicalVertex();
    inline QLogicalVertex(const QVector3D &a, const QVector3D &n = QVector3D(),
                            const QVector2D &t = InvalidTexCoord);
    inline QLogicalVertex(const QVector3D &a, QColor4b color,
                            const QVector3D &n = QVector3D(),
                            const QVector2D &t = InvalidTexCoord);
    inline const QVector3D &vertex() const;
    inline void setVertex(const QVector3D &v);
    inline QVector3D &vertexRef();
    inline operator QVector3D () const;
    inline const QVector3D &normal() const;
    inline void setNormal(const QVector3D &n);
    inline QVector3D &normalRef();
    inline const QVector2D &texCoord() const;
    inline void setTexCoord(const QVector2D &t);
    inline QVector2D &texCoordRef();
    inline const QColor4b &color() const;
    inline void setColor(const QColor4b &c);
    inline QColor4b &colorRef();
    inline bool hasType(Type t) const;
    bool operator==(const QLogicalVertex &rhs) const;
    inline bool isNull() const;
private:
    QVector3D m_vertex;
    QVector3D m_normal;
    QVector2D m_texCoord;
    QColor4b m_color;
};

QLogicalVertex::QLogicalVertex()
    : m_texCoord(InvalidTexCoord)
    , m_types(None)
{
}

inline QLogicalVertex::QLogicalVertex(const QVector3D &a, const QVector3D &n,
                                      const QVector2D &t)
    : m_vertex(a)
    , m_normal(n)
    , m_texCoord(t)
    , m_types(Vertex)
{
    if (!n.isNull())
        m_types |= Normal;
    if (t != InvalidTexCoord)
        m_types |= Texture;
}

inline QLogicalVertex::QLogicalVertex(const QVector3D &a, QColor4b color,
                                      const QVector3D &n, const QVector2D &t)
    : m_vertex(a)
    , m_normal(n)
    , m_texCoord(t)
    , m_color(color)
    , m_types(Vertex | Color)
{
    if (!n.isNull())
        m_types |= Normal;
    if (t != InvalidTexCoord)
        m_types |= Texture;
}

inline const QVector3D &QLogicalVertex::vertex() const
{
    return m_vertex;
}

inline void QLogicalVertex::setVertex(const QVector3D &v)
{
    m_types |= Vertex;
    m_vertex = v;
}

inline QVector3D &QLogicalVertex::vertexRef()
{
    return m_vertex;
}

inline QLogicalVertex::operator QVector3D () const
{
    return m_vertex;
}

inline const QVector3D &QLogicalVertex::normal() const
{
    return m_normal;
}

inline void QLogicalVertex::setNormal(const QVector3D &n)
{
    if (!n.isNull())
        m_types |= Normal;
    m_normal = n;
}

inline QVector3D &QLogicalVertex::normalRef()
{
    return m_normal;
}

inline const QVector2D &QLogicalVertex::texCoord() const
{
    return m_texCoord;
}

inline void QLogicalVertex::setTexCoord(const QVector2D &t)
{
    if (t != InvalidTexCoord)
        m_types |= Texture;
    m_texCoord = t;
}

inline QVector2D &QLogicalVertex::texCoordRef()
{
    return m_texCoord;
}

inline const QColor4b &QLogicalVertex::color() const
{
    return m_color;
}

inline void QLogicalVertex::setColor(const QColor4b &c)
{
    m_types |= Color;
    m_color = c;
}

inline QColor4b &QLogicalVertex::colorRef()
{
    return m_color;
}

inline bool QLogicalVertex::hasType(Type t) const
{
    return (m_types & t);
}

inline bool QLogicalVertex::operator==(const QLogicalVertex &rhs) const
{
    return (m_types == rhs.m_types &&
            m_vertex == rhs.m_vertex &&
            m_normal == rhs.m_normal &&
            m_texCoord == rhs.m_texCoord &&
            m_color == rhs.m_color);
}

inline bool QLogicalVertex::isNull() const
{
    return m_types == None;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QLogicalVertex &section);
#endif

QT_END_NAMESPACE

#endif // QLOGICALVERTEX_H
