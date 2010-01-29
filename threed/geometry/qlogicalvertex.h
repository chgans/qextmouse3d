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

#ifndef QLOGICALVERTEX_H
#define QLOGICALVERTEX_H

#include "qgeometrydata.h"
#include "qcustomdataarray.h"

QT_BEGIN_NAMESPACE

class QLogicalVertex
{
public:
    inline QLogicalVertex();
    inline QLogicalVertex(QGeometryData data, int index);
    inline QLogicalVertex(const QVector3D &a);
    inline QLogicalVertex(const QVector3D &a, const QVector3D &n);
    inline QLogicalVertex(const QVector3D &a,  const QVector3D &n, const QVector2D &t);
    inline QLogicalVertex(const QVector3D &a, QColor4B color);
    ~QLogicalVertex() {}

    inline QVector3D vertex() const;
    inline void setVertex(const QVector3D &v);
    inline QVector3D &vertexRef();

    operator QVector3D () { return vertex(); }

    inline QVariant attribute(QGL::VertexAttribute field = QGL::CustomVertex0) const;
    inline void setAttribute(float value, QGL::VertexAttribute attr);
    inline void setAttribute(const QVector2D &v, QGL::VertexAttribute field = QGL::CustomVertex0);
    inline void setAttribute(const QVector3D &v, QGL::VertexAttribute field = QGL::CustomVertex0);
    inline float &floatAttributeRef(QGL::VertexAttribute field = QGL::CustomVertex0);
    inline QVector2D &vector2DAttributeRef(QGL::VertexAttribute field = QGL::CustomVertex0);
    inline QVector3D &vector3DAttributeRef(QGL::VertexAttribute field = QGL::CustomVertex0);
    inline float floatAttribute(QGL::VertexAttribute field = QGL::CustomVertex0) const;
    inline QVector2D vector2DAttribute(QGL::VertexAttribute field = QGL::CustomVertex0) const;
    inline QVector3D vector3DAttribute(QGL::VertexAttribute field = QGL::CustomVertex0) const;
    inline QCustomDataArray::ElementType attributeType(QGL::VertexAttribute field = QGL::CustomVertex0);

    inline QVector3D normal() const;
    inline void setNormal(const QVector3D &n);
    inline QVector3D &normalRef();

    inline QColor4B color() const;
    inline void setColor(const QColor4B &c);
    inline QColor4B &colorRef();

    inline QVector2D texCoord(QGL::VertexAttribute attr = QGL::TextureCoord0) const;
    inline void setTexCoord(const QVector2D &t, QGL::VertexAttribute attr = QGL::TextureCoord0);
    inline QVector2D &texCoordRef(QGL::VertexAttribute attr = QGL::TextureCoord0);

    inline bool hasField(QGL::VertexAttribute type) const;
    inline quint32 fields() const;
    inline int index() const;
    inline QGeometryData data() const;
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
    m_data.appendTexCoord(t);
}

inline QLogicalVertex::QLogicalVertex(const QVector3D &a, QColor4B color)
    : m_index(0)
{
    m_data.appendVertex(a);
    m_data.appendColor(color);
}

inline QVector3D QLogicalVertex::vertex() const
{
    return m_data.vertex(m_index);
}

inline void QLogicalVertex::setVertex(const QVector3D &v)
{
    if (m_index == -1)
        m_index = 0;
    if (m_index == m_data.count(QGL::Position))
        m_data.appendVertex(v);
    else
        m_data.vertexRef(m_index) = v;
}

inline QVector3D &QLogicalVertex::vertexRef()
{
    return m_data.vertexRef(m_index);
}

inline QVariant QLogicalVertex::attribute(QGL::VertexAttribute attr) const
{
    return m_data.attributes(attr).at(m_index);
}

inline void QLogicalVertex::setAttribute(float v, QGL::VertexAttribute attr)
{
    if (m_index == -1)
        m_index = 0;
    if (m_index == m_data.count(attr))
        m_data.appendAttribute(v, attr);
    else
        m_data.floatAttributeRef(m_index, attr) = v;
}

inline void QLogicalVertex::setAttribute(const QVector2D &v, QGL::VertexAttribute attr)
{
    if (m_index == -1)
        m_index = 0;
    if (m_index == m_data.count(attr))
        m_data.appendAttribute(v, attr);
    else
        m_data.vector2DAttributeRef(m_index, attr) = v;
}

inline void QLogicalVertex::setAttribute(const QVector3D &v, QGL::VertexAttribute attr)
{
    if (m_index == -1)
        m_index = 0;
    if (m_index == m_data.count(attr))
        m_data.appendAttribute(v, attr);
    else
        m_data.vector3DAttributeRef(m_index, attr) = v;
}

inline float &QLogicalVertex::floatAttributeRef(QGL::VertexAttribute field)
{
    return m_data.floatAttributeRef(m_index, field);
}

inline QVector2D &QLogicalVertex::vector2DAttributeRef(QGL::VertexAttribute field)
{
    return m_data.vector2DAttributeRef(m_index, field);
}

inline QVector3D &QLogicalVertex::vector3DAttributeRef(QGL::VertexAttribute field)
{

    return m_data.vector3DAttributeRef(m_index, field);
}

inline float QLogicalVertex::floatAttribute(QGL::VertexAttribute field) const
{
    return m_data.floatAttribute(m_index, field);
}

inline QVector2D QLogicalVertex::vector2DAttribute(QGL::VertexAttribute field) const
{
    return m_data.vector2DAttribute(m_index, field);
}

inline QVector3D QLogicalVertex::vector3DAttribute(QGL::VertexAttribute field) const
{
    return m_data.vector3DAttribute(m_index, field);
}

inline QCustomDataArray::ElementType QLogicalVertex::attributeType(QGL::VertexAttribute field)
{
    return m_data.attributes(field).elementType();
}

inline QVector3D QLogicalVertex::normal() const
{
    const QVector3D &norm = m_data.commonNormal();
    if (norm.isNull())
        return m_data.normals().at(m_index);
    return norm;
}

inline void QLogicalVertex::setNormal(const QVector3D &n)
{
    if (m_index == -1)
        m_index = 0;
    if (m_index == m_data.count(QGL::Normal))
        m_data.appendNormal(n);
    else
        m_data.normalRef(m_index) = n;
}

inline QVector3D &QLogicalVertex::normalRef()
{
    return m_data.normalRef(m_index);
}

inline QVector2D QLogicalVertex::texCoord(QGL::VertexAttribute attr) const
{
    return m_data.texCoord(m_index, attr);
}

inline void QLogicalVertex::setTexCoord(const QVector2D &t, QGL::VertexAttribute attr)
{
    Q_ASSERT(attr >= QGL::TextureCoord0 && attr <= QGL::TextureCoord7);
    if (m_index == -1)
        m_index = 0;
    if (m_index == m_data.count(attr))
        m_data.appendTexCoord(t, attr);
    else
        m_data.texCoordRef(m_index, attr) = t;
}

inline QVector2D &QLogicalVertex::texCoordRef(QGL::VertexAttribute attr)
{
    return m_data.texCoordRef(m_index, attr);
}

inline QColor4B QLogicalVertex::color() const
{
    return m_data.color(m_index);
}

inline void QLogicalVertex::setColor(const QColor4B &c)
{
    if (m_index == -1)
        m_index = 0;
    if (m_index == m_data.count(QGL::Color))
        m_data.appendColor(c);
    else
        m_data.colorRef(m_index) = c;
}

inline QColor4B &QLogicalVertex::colorRef()
{
    return m_data.colorRef(m_index);
}

inline bool QLogicalVertex::hasField(QGL::VertexAttribute attr) const
{
    if (attr == QGL::Normal)
        return (m_data.hasField(attr) || !m_data.commonNormal().isNull());
    return m_data.hasField(attr);
}

inline quint32 QLogicalVertex::fields() const
{
    static const quint32 nmask = 0x01 << QGL::Normal;
    quint32 f = m_data.fields();
    if (!(f & nmask) && !m_data.commonNormal().isNull())
        f |= nmask;
    return f;
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
