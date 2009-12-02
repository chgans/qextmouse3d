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

#include "qcolor4b.h"
#include "qlogicalvertex.h"
#include "qglnamespace.h"
#include "qglvertexarray.h"

class QGeometryData
{
public:
    QGeometryData(QLogicalVertex::Types type = QLogicalVertex::Vertex);
    ~QGeometryData();

    void reserve(int capacity);
    void squeeze();

    inline int appendVertex(const QLogicalVertex &v);
    QLogicalVertex vertexAt(int) const;
    QGLVertexArray QGeometryData::toVertexArray() const;

    inline void setVertex(int, const QVector3D &v);
    inline void setNormal(int, const QVector3D &n);
    inline void setTexCoord(int, const QVector2D &t);
    inline void setColor(int, const QColor4b &c);

    inline bool hasType(QLogicalVertex::Types type);

    QGL::VectorArray vertices;
    QGL::VectorArray *normals;
    QGL::TexCoordArray *texCoords;
    QGL::ColorArray *colors;
    QGL::IndexArray indices;
    QLogicalVertex::Types dataTypes;
};

inline int QGeometryData::appendVertex(const QLogicalVertex &v)
{
    int index = vertices.count();
    vertices.append(v.vertex());
    indices.append(index);
    if (v.hasType(QLogicalVertex::Normal))
        setNormal(index, v.normal());
    if (v.hasType(QLogicalVertex::Texture))
        setTexCoord(index, v.texCoord());
    if (v.hasType(QLogicalVertex::Color))
        setColor(index, v.color());
    return index;
}

inline QLogicalVertex QGeometryData::vertexAt(int index) const
{
    QLogicalVertex lv(vertices.at(index));
    if (dataTypes & QLogicalVertex::Normal)
        lv.setNormal(normals->at(index));
    if (dataTypes & QLogicalVertex::Texture)
        lv.setTexCoord(texCoords->at(index));
    if (dataTypes & QLogicalVertex::Color)
        lv.setColor(colors->at(index));
    return lv;
}

inline void QGeometryData::setVertex(int index, const QVector3D &v)
{
    vertices[index] = v;
}

inline void QGeometryData::setNormal(int index, const QVector3D &n)
{
    if (!normals)
    {
        normals = new QGL::VectorArray;
        normals->reserve(vertices.capacity());
        dataTypes |= QLogicalVertex::Normal;
    }
    (*normals)[index] = n;
}

inline void QGeometryData::setTexCoord(int index, const QVector2D &t)
{
    if (!texCoords)
    {
        texCoords = new QGL::TexCoordArray;
        texCoords->reserve(vertices.capacity());
        dataTypes |= QLogicalVertex::Texture;
    }
    (*texCoords)[index] = t;
}

inline void QGeometryData::setColor(int index, const QColor4b &c)
{
    if (!colors)
    {
        colors = new QGL::ColorArray;
        colors->reserve(vertices.capacity());
        dataTypes |= QLogicalVertex::Color;
    }
    (*colors)[index] = c;
}

inline bool QGeometryData::hasType(QLogicalVertex::Types type)
{
    return (type & dataTypes);
}

#endif // QGEOMETRYDATA_H
