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
#include "qglnamespace.h"
#include "qglvertexarray.h"
#include "qglindexarray.h"
#include "qbox3d.h"
#include "qdataarray.h"
#include "qvectorarray.h"

QT_BEGIN_NAMESPACE

class QGeometryDataPrivate;
class QVertexRefs;
class QLogicalVertex;

class QGeometryData
{
public:
    QGeometryData();
    QGeometryData(const QGeometryData &);
    QGeometryData(quint32 fields);
    ~QGeometryData();

    QGeometryData &operator=(const QGeometryData &);

    void appendGeometry(const QGeometryData &data);
    int appendVertex(const QLogicalVertex &v);
    QLogicalVertex vertexAt(int i) const;
    QGLVertexArray toVertexArray() const;
    void normalizeNormals();
    QBox3D boundingBox() const;
    void setCommonNormal(const QVector3D &n);
    QVector3D commonNormal() const;
    QGeometryData zippedWith(const QGeometryData &other) const;
    void zipWith(const QGeometryData &other);
    void clear();
    void clear(QGL::VertexAttribute);

    void appendVertex(const QVector3D &v);
    void appendAttribute(float a, QGL::VertexAttribute field);
    void appendAttribute(const QVector2D &a, QGL::VertexAttribute field);
    void appendAttribute(const QVector3D &a, QGL::VertexAttribute field);
    void appendAttribute(const QVariant &a, QGL::VertexAttribute field);
    void appendNormal(const QVector3D &n);
    void appendTexCoord(const QVector2D &t, QGL::VertexAttribute field);
    void appendColor(const QColor4b &c);

    void appendVertexArray(const QVector3DArray &ary);
    void appendAttributeArray(const QCustomDataArray &ary, QGL::VertexAttribute field);
    void appendNormalArray(const QVector3DArray &ary);
    void appendTexCoordArray(const QVector2DArray &ary, QGL::VertexAttribute field);
    void appendColorArray(const QDataArray<QColor4b> &ary);

    QVector3D &vertexRef(int i);
    QVector3DArray vertices() const;

    QVector3D &normalRef(int i);
    QVector3DArray normals() const;

    QColor4b &colorRef(int i);
    QDataArray<QColor4b> colors() const;

    QVector2D &texCoordRef(int i, QGL::VertexAttribute field = QGL::TextureCoord0);
    QVector2DArray texCoords(QGL::VertexAttribute field) const;

    float &floatAttributeRef(int i, QGL::VertexAttribute field = QGL::CustomVertex0);
    QVector2D &vector2DAttributeRef(int i, QGL::VertexAttribute field = QGL::CustomVertex0);
    QVector3D &vector3DAttributeRef(int i, QGL::VertexAttribute field = QGL::CustomVertex0);
    QCustomDataArray attributes(QGL::VertexAttribute field) const;

    inline static quint32 fieldMask(QGL::VertexAttribute f) { return (quint32)0x01 << f; }
    bool hasField(QGL::VertexAttribute field) const;
    void enableField(QGL::VertexAttribute field);
    quint32 fields() const;
    int count() const;
private:
    void detach();

    QGeometryDataPrivate *d;
};

QT_END_NAMESPACE

#endif // QGEOMETRYDATA_H
