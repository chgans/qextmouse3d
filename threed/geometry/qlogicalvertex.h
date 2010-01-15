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

#include "qgeometrydata.h"

QT_BEGIN_NAMESPACE

class QLogicalVertex
{
public:
    inline QLogicalVertex();
    inline QLogicalVertex(QGeometryData data, int index);
    inline QLogicalVertex(const QVector3D &a);
    inline QLogicalVertex(const QVector3D &a, const QVector3D &n);
    inline QLogicalVertex(const QVector3D &a,  const QVector3D &n, const QVector2D &t);
    inline QLogicalVertex(const QVector3D &a, QColor4b color);
    ~QLogicalVertex() {}

    inline const QVector3D &vertex() const;
    inline void setVertex(const QVector3D &v);
    inline QVector3D &vertexRef();

    operator QVector3D () { return vertex(); }

    inline const QVector3D &attribute(QGL::VertexAttribute attr) const;
    inline void setAttribute(const QVector3D &v, QGL::VertexAttribute attr);
    inline QVector3D &attributeRef(QGL::VertexAttribute attr);

    inline const QVector3D &normal() const;
    inline void setNormal(const QVector3D &n);
    inline QVector3D &normalRef();

    inline const QVector2D &texCoord() const;
    inline void setTexCoord(const QVector2D &t);
    inline QVector2D &texCoordRef();

    inline const QColor4b &color() const;
    inline void setColor(const QColor4b &c);
    inline QColor4b &colorRef();

    inline const QVector2D &texCoord(QGL::VertexAttribute attr) const;
    inline void setTexCoord(const QVector2D &t, QGL::VertexAttribute attr);
    inline QVector2D &texCoordRef(QGL::VertexAttribute attr);

    inline bool hasField(QGL::VertexAttribute type) const;
    inline quint32 fields() const;
    inline int index() const;
    inline bool isNull() const;

    bool operator==(const QLogicalVertex &rhs) const;

private:
    QGeometryData m_data;
    int m_index;
};

inline QLogicalVertex::QLogicalVertex()
    : m_index(-1)
{
}

inline QLogicalVertex::QLogicalVertex(QGeometryData data, int index)
    : m_data(data)
    , m_index(index)
{
    Q_ASSERT(index < data.count());
}

inline QLogicalVertex::QLogicalVertex(const QVector3D &a)
    : m_index(0)
{
    m_data.appendVertex(a);
}

inline QLogicalVertex::QLogicalVertex(const QVector3D &a, const QVector3D &n)
    : m_index(0)
{
    m_data.appendVertex(a);
    m_data.appendNormal(n);
}

inline QLogicalVertex::QLogicalVertex(const QVector3D &a, const QVector3D &n, const QVector2D &t)
    : m_index(0)
{
    m_data.appendVertex(a);
    m_data.appendNormal(n);
    m_data.appendNormal(t);
}

inline QLogicalVertex::QLogicalVertex(const QVector3D &a, QColor4b color)
    : m_index(0)
{
    m_data.appendVertex(a);
    m_data.appendColor(color);
}

inline const QVector3D &QLogicalVertex::vertex() const
{
    return m_data.vertices().at(m_index);
}

inline void QLogicalVertex::setVertex(const QVector3D &v)
{
    if (m_index == m_data.count())
        m_data.appendVertex(v);
    else
        m_data.vertexRef(m_index) = v;
}

inline QVector3D &QLogicalVertex::vertexRef()
{
    return m_data.vertexRef(m_index);
}

inline const QVector3D &QLogicalVertex::attribute(QGL::VertexAttribute attr) const
{
    return m_data.attributes(attr).at(m_index);
}

inline void QLogicalVertex::setAttribute(const QVector3D &v, QGL::VertexAttribute attr)
{
    if (m_index == m_data.count())
        m_data.appendAttribute(v, attr);
    else
        m_data.attributeRef(m_index, attr) = v;
}

inline QVector3D &QLogicalVertex::attributeRef(QGL::VertexAttribute attr)
{
    return m_data.attributeRef(m_index, attr);
}

inline const QVector3D &QLogicalVertex::normal() const
{
    return m_data.normals().at(m_index);
}

inline void QLogicalVertex::setNormal(const QVector3D &n)
{
    if (m_index == m_data.count())
        m_data.appendNormal(n);
    else
        m_data.normalRef(m_index) = n;
}

inline QVector3D &QLogicalVertex::normalRef()
{
    return m_data.normalRef(m_index);
}

inline const QVector2D &QLogicalVertex::texCoord() const
{
    return m_data.texCoords(QGL::TextureCoord0).at(m_index);
}

inline void QLogicalVertex::setTexCoord(const QVector2D &t)
{
    if (m_index == m_data.count())
        m_data.appendTexCoord(t, QGL::TextureCoord0);
    else
        m_data.texCoordRef(m_index, QGL::TextureCoord0) = t;
}

inline QVector2D &QLogicalVertex::texCoordRef()
{
    return m_data.texCoordRef(m_index, QGL::TextureCoord0);
}

inline const QVector2D &QLogicalVertex::texCoord(QGL::VertexAttribute attr) const
{
    return m_data.texCoords(attr).at(m_index);
}

inline void QLogicalVertex::setTexCoord(const QVector2D &t, QGL::VertexAttribute attr)
{
    Q_ASSERT(m_index <= m_data.count());
    if (m_index == m_data.count())
        m_data.appendTexCoord(t, attr);
    else
        m_data.texCoordRef(m_index, attr) = t;
}

inline QVector2D &QLogicalVertex::texCoordRef(QGL::VertexAttribute attr)
{
    return m_data.texCoordRef(m_index, attr);
}

inline const QColor4b &QLogicalVertex::color() const
{
    return m_data.colors().at(m_index);
}

inline void QLogicalVertex::setColor(const QColor4b &c)
{
    Q_ASSERT(m_index <= m_data.count());
    if (m_index == m_data.count())
        m_data.appendColor(c);
    else
        m_data.colorRef(m_index) = c;
}

inline QColor4b &QLogicalVertex::colorRef()
{
    return m_data.colorRef(m_index);
}

inline bool QLogicalVertex::hasField(QGL::VertexAttribute attr) const
{
    return m_data.hasField(attr);
}

inline quint32 QLogicalVertex::fields() const
{
    return m_data.fields();
}

inline int QLogicalVertex::index() const
{
    return m_index;
}

inline bool QLogicalVertex::isNull() const
{
    return (m_index == -1);
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QLogicalVertex &section);
#endif

QT_END_NAMESPACE

#endif // QLOGICALVERTEX_H
