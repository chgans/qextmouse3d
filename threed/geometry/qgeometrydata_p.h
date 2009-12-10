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

#ifndef QGEOMETRYDATA_H
#define QGEOMETRYDATA_H

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

#include "qcolor4b.h"
#include "qlogicalvertex.h"
#include "qglnamespace.h"
#include "qglvertexarray.h"
#include "qbox3d.h"

class QGeometryData
{
public:
    QGeometryData(QLogicalVertex::Types type = QLogicalVertex::Vertex);
    ~QGeometryData();

    void reserve(int capacity);
    void squeeze();

    inline int appendVertex(const QLogicalVertex &v);
    QLogicalVertex vertexAt(int) const;
    QGLVertexArray toVertexArray() const;
    void normalizeNormals();

    inline void appendVertex(const QVector3D &v);
    inline void appendNormal(const QVector3D &n);
    inline void appendTexCoord(const QVector2D &t);
    inline void appendColor(const QColor4b &c);
    inline void appendIndex(int);

    inline const QVector3D *vertexConstData() const;
    inline QVector3D *vertexData();
    inline QGL::VectorArray vertices() const;

    inline const QVector3D *normalConstData() const;
    inline QVector3D *normalData();
    inline QGL::VectorArray normals() const;

    inline const QVector2D *texCoordConstData() const;
    inline QVector2D *texCoordData();
    inline QGL::TexCoordArray texCoords() const;

    inline const QColor4b *colorConstData() const;
    inline QColor4b *colorData();
    inline QGL::ColorArray colors() const;

    inline const int *indexConstData() const;
    inline int *indexData();
    inline const QGL::IndexArray indices() const;

    inline bool hasType(QLogicalVertex::Types type) const;
    inline void enableTypes(QLogicalVertex::Types type);
    inline QLogicalVertex::Types types() const;
    inline QBox3D boundingBox() const;
    inline int vertexCount() const;
    inline int count() const;

private:
    QGL::VectorArray m_vertices;
    QGL::VectorArray *m_normals;
    QGL::TexCoordArray *m_texCoords;
    QGL::ColorArray *m_colors;
    QGL::IndexArray m_indices;
    QLogicalVertex::Types m_dataTypes;
};

inline int QGeometryData::appendVertex(const QLogicalVertex &v)
{
    int index = m_vertices.count();
    if (v.hasType(QLogicalVertex::Normal))
        appendNormal(v.normal());
    if (v.hasType(QLogicalVertex::Texture))
        appendTexCoord(v.texCoord());
    if (v.hasType(QLogicalVertex::Color))
        appendColor(v.color());
    m_vertices.append(v.vertex());
    m_indices.append(index);
    return index;
}

inline QLogicalVertex QGeometryData::vertexAt(int index) const
{
    QLogicalVertex lv(m_vertices.at(index));
    if (m_dataTypes & QLogicalVertex::Normal)
        lv.setNormal(m_normals->at(index));
    if (m_dataTypes & QLogicalVertex::Texture)
        lv.setTexCoord(m_texCoords->at(index));
    if (m_dataTypes & QLogicalVertex::Color)
        lv.setColor(m_colors->at(index));
    return lv;
}

inline void QGeometryData::appendVertex(const QVector3D &v)
{
    m_vertices.append(v);
}

inline void QGeometryData::appendNormal(const QVector3D &n)
{
    enableTypes(QLogicalVertex::Normal);
    m_normals->append(n);
}

inline void QGeometryData::appendTexCoord(const QVector2D &t)
{
    enableTypes(QLogicalVertex::Texture);
    m_texCoords->append(t);
}

inline void QGeometryData::appendColor(const QColor4b &c)
{
    enableTypes(QLogicalVertex::Color);
    m_colors->append(c);
}

inline void QGeometryData::appendIndex(int index)
{
    m_indices.append(index);
}

inline const QVector3D *QGeometryData::vertexConstData() const
{
    return m_vertices.constData();
}

inline QVector3D *QGeometryData::vertexData()
{
    return m_vertices.data();
}

inline QGL::VectorArray QGeometryData::vertices() const
{
    return m_vertices;  // return copy
}

inline const QVector3D *QGeometryData::normalConstData() const
{
    return m_normals ? m_normals->constData() : 0;
}

inline QVector3D *QGeometryData::normalData()
{
    return m_normals ? m_normals->data() : 0;
}

inline QGL::VectorArray QGeometryData::normals() const
{
    if (m_normals)
        return *m_normals;
    return QGL::VectorArray();
}

inline const QVector2D *QGeometryData::texCoordConstData() const
{
    return m_texCoords ? m_texCoords->constData() : 0;
}

inline QVector2D *QGeometryData::texCoordData()
{
    return m_texCoords ? m_texCoords->data() : 0;
}

inline QGL::TexCoordArray QGeometryData::texCoords() const
{
    if (m_texCoords)
        return *m_texCoords;  // return copy
    return QGL::TexCoordArray();
}

inline const QColor4b *QGeometryData::colorConstData() const
{
    return m_colors ? m_colors->constData() : 0;
}

inline QColor4b *QGeometryData::colorData()
{
    return m_colors ? m_colors->data() : 0;
}

inline QGL::ColorArray QGeometryData::colors() const
{
    if (m_colors)
        return *m_colors;
    return QGL::ColorArray();
}

inline const int *QGeometryData::indexConstData() const
{
    return m_indices.constData();
}

inline int *QGeometryData::indexData()
{
    return m_indices.data();
}

inline const QGL::IndexArray QGeometryData::indices() const
{
    return m_indices;
}

inline bool QGeometryData::hasType(QLogicalVertex::Types type) const
{
    return (type & m_dataTypes);
}

inline void QGeometryData::enableTypes(QLogicalVertex::Types types)
{
    if (types & QLogicalVertex::Normal && !m_normals)
    {
        m_normals = new QGL::VectorArray(m_vertices.count());
        m_normals->reserve(m_vertices.capacity());
        m_dataTypes |= QLogicalVertex::Normal;
    }
    if (types & QLogicalVertex::Texture && !m_texCoords)
    {
        m_texCoords = new QGL::TexCoordArray(m_vertices.count(),
                                             QLogicalVertex::InvalidTexCoord);
        m_texCoords->reserve(m_vertices.capacity());
        m_dataTypes |= QLogicalVertex::Texture;
    }
    if (types & QLogicalVertex::Color && !m_colors)
    {
        m_colors = new QGL::ColorArray(m_vertices.count());
        m_colors->reserve(m_vertices.capacity());
        m_dataTypes |= QLogicalVertex::Color;
    }
    m_dataTypes |= types;
}

inline QLogicalVertex::Types QGeometryData::types() const
{
    return m_dataTypes;
}

inline QBox3D QGeometryData::boundingBox() const
{
    QBox3D bb;
    for (int i = 0; i < m_vertices.count(); ++i)
        bb.expand(m_vertices.at(i));
    return bb;
}

inline int QGeometryData::vertexCount() const
{
    return m_vertices.count();
}

inline int QGeometryData::count() const
{
    return m_indices.count();
}

#endif // QGEOMETRYDATA_H
