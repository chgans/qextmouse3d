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

#ifndef INLINEMESH_H
#define INLINEMESH_H

#include "mesh.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class InlineMesh : public Mesh
{
    Q_OBJECT
    Q_ENUMS(PrimitiveType)
    Q_PROPERTY(PrimitiveType primitiveType READ primitiveType WRITE setPrimitiveType NOTIFY primitiveTypeChanged)
    Q_PROPERTY(bool smooth READ smooth WRITE setSmooth NOTIFY smoothChanged)
    Q_PROPERTY(QVariant vertices READ vertices WRITE setVertices NOTIFY verticesChanged)
    Q_PROPERTY(QVariant normals READ normals WRITE setNormals NOTIFY normalsChanged)
    Q_PROPERTY(QVariant textureCoords READ textureCoords WRITE setTextureCoords NOTIFY textureCoordsChanged)
public:
    InlineMesh(QObject *parent = 0);
    ~InlineMesh();

    enum PrimitiveType
    {
        Triangles,
        TriangleFan,
        TriangleStrip,
        Quads,
        QuadStrip
    };

    PrimitiveType primitiveType() const;
    void setPrimitiveType(PrimitiveType type);

    bool smooth() const;
    void setSmooth(bool value);

    QVariant vertices() const;
    void setVertices(const QVariant &value);

    QVariant normals() const;
    void setNormals(const QVariant &value);

    QVariant textureCoords() const;
    void setTextureCoords(const QVariant &value);

    void draw(QGLPainter *painter, int branchId);

Q_SIGNALS:
    void primitiveTypeChanged();
    void smoothChanged();
    void verticesChanged();
    void normalsChanged();
    void textureCoordsChanged();

private:
    PrimitiveType m_primitiveType;
    QVariant m_vertices;
    QVariant m_normals;
    QVariant m_textureCoords;
    bool m_smooth;
    bool m_changed;
    QGLSceneNode *m_node;
};

QML_DECLARE_TYPE(InlineMesh)

QT_END_NAMESPACE

QT_END_HEADER

#endif
