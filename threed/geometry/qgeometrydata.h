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

#ifndef QGEOMETRYDATA_H
#define QGEOMETRYDATA_H

#include "qcolor4b.h"
#include "qglnamespace.h"
#include "qglvertexarray.h"
#include "qglindexarray.h"
#include "qbox3d.h"
#include "qarray.h"
#include "qvectorarray.h"

QT_BEGIN_NAMESPACE

class QGeometryDataPrivate;
class QLogicalVertex;

namespace QGL
{
    inline quint32 fieldMask(QGL::VertexAttribute f) { return (quint32)0x01 << f; }
}

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
    void reserve(int amount);

    void appendIndex(int index);
    void appendIndices(int index1, int index2, int index3);
    QGLIndexArray indices() const;

    void appendVertex(const QVector3D &v0);
    void appendVertex(const QVector3D &v0, const QVector3D &v1);
    void appendVertex(const QVector3D &v0, const QVector3D &v1, const QVector3D &v2);
    void appendVertex(const QVector3D &v0, const QVector3D &v1, const QVector3D &v2, const QVector3D &v3);

    void appendAttribute(float a, QGL::VertexAttribute field = QGL::CustomVertex0);
    void appendAttribute(float a, float b, QGL::VertexAttribute field = QGL::CustomVertex0);
    void appendAttribute(float a, float b, float c, QGL::VertexAttribute field = QGL::CustomVertex0);
    void appendAttribute(float a, float b, float c, float d, QGL::VertexAttribute field = QGL::CustomVertex0);
    void appendAttribute(const QVector2D &a, QGL::VertexAttribute field = QGL::CustomVertex0);
    void appendAttribute(const QVector3D &a, QGL::VertexAttribute field = QGL::CustomVertex0);
    void appendAttribute(const QVariant &a, QGL::VertexAttribute field = QGL::CustomVertex0);

    void appendNormal(const QVector3D &n0);
    void appendNormal(const QVector3D &n0, const QVector3D &n1);
    void appendNormal(const QVector3D &n0, const QVector3D &n1, const QVector3D &n2);
    void appendNormal(const QVector3D &n0, const QVector3D &n1, const QVector3D &n2, const QVector3D &n3);

    void appendTexCoord(const QVector2D &t0, QGL::VertexAttribute field = QGL::TextureCoord0);
    void appendTexCoord(const QVector2D &t0, const QVector2D &t1, QGL::VertexAttribute field = QGL::TextureCoord0);
    void appendTexCoord(const QVector2D &t0, const QVector2D &t1, const QVector2D &t2, QGL::VertexAttribute field = QGL::TextureCoord0);
    void appendTexCoord(const QVector2D &t0, const QVector2D &t1, const QVector2D &t2, const QVector2D &t3, QGL::VertexAttribute field = QGL::TextureCoord0);

    void appendColor(const QColor4B &c0);
    void appendColor(const QColor4B &c0, const QColor4B &c1);
    void appendColor(const QColor4B &c0, const QColor4B &c1, const QColor4B &c2);
    void appendColor(const QColor4B &c0, const QColor4B &c1, const QColor4B &c2, const QColor4B &c3);

    void appendVertexArray(const QVector3DArray &ary);
    void appendAttributeArray(const QCustomDataArray &ary, QGL::VertexAttribute field = QGL::CustomVertex0);
    void appendNormalArray(const QVector3DArray &ary);
    void appendTexCoordArray(const QVector2DArray &ary, QGL::VertexAttribute field = QGL::TextureCoord0);
    void appendColorArray(const QArray<QColor4B> &ary);

    QVector3D &vertexRef(int i);
    QVector3DArray vertices() const;
    QVector3D vertex(int i) const;

    QVector3D &normalRef(int i);
    QVector3DArray normals() const;
    QVector3D normal(int i) const;

    QColor4B &colorRef(int i);
    QArray<QColor4B> colors() const;
    QColor4B color(int i) const;

    QVector2D &texCoordRef(int i, QGL::VertexAttribute field = QGL::TextureCoord0);
    QVector2DArray texCoords(QGL::VertexAttribute field = QGL::TextureCoord0) const;
    QVector2D texCoord(int i, QGL::VertexAttribute field = QGL::TextureCoord0) const;

    float &floatAttributeRef(int i, QGL::VertexAttribute field = QGL::CustomVertex0);
    QVector2D &vector2DAttributeRef(int i, QGL::VertexAttribute field = QGL::CustomVertex0);
    QVector3D &vector3DAttributeRef(int i, QGL::VertexAttribute field = QGL::CustomVertex0);
    QCustomDataArray attributes(QGL::VertexAttribute field = QGL::CustomVertex0) const;
    float floatAttribute(int i, QGL::VertexAttribute field = QGL::CustomVertex0) const;
    QVector2D vector2DAttribute(int i, QGL::VertexAttribute field = QGL::CustomVertex0) const;
    QVector3D vector3DAttribute(int i, QGL::VertexAttribute field = QGL::CustomVertex0) const;

    bool hasField(QGL::VertexAttribute field) const;
    void enableField(QGL::VertexAttribute field);
    quint32 fields() const;
    int count() const;
    int count(QGL::VertexAttribute field) const;
protected:
    const QVector3DArray *vertexData() const;
private:
    void detach();
#ifndef QT_NO_DEBUG
    void check() const;
#else
    void check() const {}
#endif
    friend class QLogicalVertex;

    QGeometryDataPrivate *d;
};

#ifndef QT_NO_DEBUG_STREAM
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QGeometryData &vertices);
#endif

QT_END_NAMESPACE

#endif // QGEOMETRYDATA_H
