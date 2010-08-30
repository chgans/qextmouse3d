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

#include "inlinemesh.h"
#include "qglbuilder.h"
#include "qvector2darray.h"
#include "qvector3darray.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass InlineMesh InlineMesh
    \brief The InlineMesh item provides a way to embed geometry directly into a QML source file.
    \since 4.8
    \ingroup qt3d::qml3d

    InlineMesh is intended for embedding small 3D objects directly into a
    QML file as literal inline vertex data.  For large objects, use the
    \l Mesh element and an external 3D object file in 3DS, obj, or
    some other supported scene format.

    The following example will display a green triangle on the screen:

    \code
    Item3d {
        mesh: triangleMesh
        effect: Effect { color: "green" }
    }

    InlineMesh {
        id: triangleMesh
        vertices: [
            -1, -1, 0,
            1, -1, 0,
            0, 1, 0
        ]
    }
    \endcode

    The \l vertices property specifies the positions of the vertices
    within the triangle.  We can also add explicit normals and texture
    co-ordinates to each of the vertices:

    \code
    Item3d {
        mesh: triangleMesh
        effect: Effect {
            color: "green"
            texture: "image.png"
        }
    }

    InlineMesh {
        id: triangleMesh
        vertices: [
            -1, -1, 0,
            1, -1, 0,
            0, 1, 0
        ]
        normals: [
            -0.5, -0.5, 1,
            0.5, -0.5, 1,
            0, 0.5, 1
        ]
        textureCoords: [
            0, 0,
            1, 0,
            1, 1
        ]
    }
    \endcode

    By default InlineMesh will assume that the supplied data
    defines separate triangles with 3 vertices each.  The type of
    primitive can be modified by setting the primitiveType property:

    \code
    Item3d {
        mesh: quadMesh
        effect: Effect {
            color: "green"
            texture: "image.png"
        }
    }

    InlineMesh {
        id: quadMesh
        primitiveType: InlineMesh.Quads
        vertices: [
            -1, -1, 0,
            1, -1, 0,
            1, 1, 0,
            -1, 1, 0
        ]
        textureCoords: [
            0, 0,
            1, 0,
            1, 1,
            0, 1
        ]
    }
    \endcode

    The values in the \l vertices, \l normals, and \l textureCoords lists
    can be bound to variable values and then animated:

    \code
    Item3d {
        mesh: quadMesh
        effect: Effect { color: "green" }
    }

    property real stretch: 1

    NumberAnimation on stretch {
        running: true
        to: 3
        duration: 2000
    }

    InlineMesh {
        id: quadMesh
        primitiveType: InlineMesh.Quads
        vertices: [
            -stretch, -1, 0,
            stretch, -1, 0,
            stretch, 1, 0,
            -stretch, 1, 0
        ]
    }
    \endcode

    Whenever a vertex is modified, the entire mesh must be recreated,
    so this may have an impact on application performance.  For complex
    animations it is recommended to use the Item3d::transform property
    on a static InlineMesh instead.

    \sa Mesh, {Pyramid inline mesh example}
*/

/*!
    \internal
*/
InlineMesh::InlineMesh(QObject *parent)
    : Mesh(parent)
    , m_primitiveType(Triangles)
    , m_smooth(false)
    , m_changed(true)
    , m_node(0)
{
}

/*!
    \internal
*/
InlineMesh::~InlineMesh()
{
    delete m_node;
}

/*!
    \qmlproperty enumeration InlineMesh::primitiveType

    This property defines the type of primitives in the inline mesh.
    The default is Triangles.

    \list
    \o Triangles Separate triangles with three vertices each.
    \o TriangleFan Triangle fan starting with the first vertex.
    \o TriangleStrip Triangle strip.
    \o Quads Separate quads with four vertices each.
    \o QuadStrip Quad strip.
    \endlist
*/
InlineMesh::PrimitiveType InlineMesh::primitiveType() const
{
    return m_primitiveType;
}

void InlineMesh::setPrimitiveType(PrimitiveType type)
{
    if (m_primitiveType != type) {
        m_primitiveType = type;
        m_changed = true;
        emit primitiveTypeChanged();
        emit dataChanged();
    }
}

/*!
    \qmlproperty bool InlineMesh::smooth

    This property defines how normals are generated if the \l normals
    property is not specified.  If this property is true, then common
    smooth normals are generated at the corners between faces.
    If this property is false (the default), then faceted normals
    are generated separately for each face.

    \sa normals
*/

bool InlineMesh::smooth() const
{
    return m_smooth;
}

void InlineMesh::setSmooth(bool value)
{
    if (m_smooth != value) {
        m_smooth = value;
        m_changed = true;
        emit smoothChanged();
        emit dataChanged();
    }
}

/*!
    \qmlproperty list<real> InlineMesh::vertices

    This property defines the positions for all of the vertices in the
    inline mesh.  Each vertex is given by three real values, defining
    the x, y, and z co-ordinates of the vertex.

    \sa normals, textureCoords
*/
QVariant InlineMesh::vertices() const
{
    return m_vertices;
}

void InlineMesh::setVertices(const QVariant &value)
{
    m_vertices = value;
    m_changed = true;
    emit verticesChanged();
    emit dataChanged();
}

/*!
    \qmlproperty list<real> InlineMesh::normals

    This property defines the normals for all of the vertices in the
    inline mesh.  Each normal is given by three real values, defining
    the x, y, and z co-ordinates of the normal.

    Normals are optional.  If normals are not supplied then InlineMesh
    will derive normals from the \l vertices according to the setting
    of \l smooth.  If normals are supplied, then the number of normals
    must match the number of vertices.

    \sa vertices, textureCoords, smooth
*/
QVariant InlineMesh::normals() const
{
    return m_normals;
}

void InlineMesh::setNormals(const QVariant &value)
{
    m_normals = value;
    m_changed = true;
    emit normalsChanged();
    emit dataChanged();
}

/*!
    \qmlproperty list<real> InlineMesh::textureCoords

    This property defines the texture co-ordinates for all of the vertices
    in the inline mesh.  Each normal is given by two real values, defining
    the components of the 2D texture co-ordinate.

    Texture co-ordinates are optional.  If texture co-ordinates are
    supplied, then the number of texture co-ordinates must match the
    number of \l vertices.

    \sa vertices, normals
*/
QVariant InlineMesh::textureCoords() const
{
    return m_textureCoords;
}

void InlineMesh::setTextureCoords(const QVariant &value)
{
    m_textureCoords = value;
    m_changed = true;
    emit textureCoordsChanged();
    emit dataChanged();
}

static QVector3DArray variantListTo3DArray(const QVariant &variant)
{
    QVector3DArray result;
    QVariantList list = variant.toList();
    for (int index = 0; (index + 2) < list.size(); index += 3) {
        result.append(qreal(list.at(index).toDouble()),
                      qreal(list.at(index + 1).toDouble()),
                      qreal(list.at(index + 2).toDouble()));
    }
    return result;
}

static QVector2DArray variantListTo2DArray(const QVariant &variant)
{
    QVector2DArray result;
    QVariantList list = variant.toList();
    for (int index = 0; (index + 1) < list.size(); index += 2) {
        result.append(qreal(list.at(index).toDouble()),
                      qreal(list.at(index + 1).toDouble()));
    }
    return result;
}

/*!
    \internal
*/
void InlineMesh::draw(QGLPainter *painter, int branchId)
{
    Q_UNUSED(branchId);
    if (!m_node || m_changed) {
        // Destroy the previous node.
        delete m_node;
        m_node = 0;
        m_changed = false;

        // Convert the QVariant script values into float arrays.
        QVector3DArray vertices = variantListTo3DArray(m_vertices);
        QVector3DArray normals = variantListTo3DArray(m_normals);
        QVector2DArray texCoords = variantListTo2DArray(m_textureCoords);

        // Build the new geometry.
        QGLBuilder builder;
        if (m_smooth)
            builder.newSection(QGL::Smooth);
        else
            builder.newSection(QGL::Faceted);
        QGeometryData op;
        for (int index = 0; index < vertices.size(); ++index) {
            op.appendVertex(vertices.at(index));
            if (index < normals.size())
                op.appendNormal(normals.at(index));
            if (index < texCoords.size()) {
                op.appendTexCoord(texCoords.at(index));
            } else {
                // Add a dummy texture co-ordinate to prevent applications
                // crashing when a texture effect is specified but there
                // are no texture co-ordinates in the mesh.
                op.appendTexCoord(QVector2D(0, 0));
            }
        }
        switch (m_primitiveType) {
        case InlineMesh::TriangleFan:
            builder.addTriangleFan(op);
            break;
        case InlineMesh::TriangleStrip:
            builder.addTriangleStrip(op);
            break;
        case InlineMesh::Quads:
            builder.addQuads(op);
            break;
        case InlineMesh::QuadStrip:
            builder.addQuadStrip(op);
            break;
        default:
            builder.addTriangles(op);
            break;
        }

        // Finalize the scene node.
        m_node = builder.finalizedSceneNode();
    }
    if (m_node)
        m_node->draw(painter);
}

QT_END_NAMESPACE
