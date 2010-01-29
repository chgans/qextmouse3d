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

#include "qglsphere.h"
#include "qglvertexarray.h"
#include "qglindexarray.h"
#include "qtriangle3d.h"

#include <QQuaternion>
#include <QImage>
#include <QPainter>

QT_BEGIN_NAMESPACE

/*!
    \class QGLSphere
    \brief The QGLSphere class represents the geometry of a simple sphere in 3D space.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::geometry

    The following example creates a sphere of 2 units in diameter and
    draws it at (10, 25, 0) in a QGLPainter:

    \code
    QGLSphere sphere(2);
    painter.translate(10, 25, 0);
    painter.draw(&sphere);
    \endcode;

    The QGLSphere class specifies positions, normals and 2d texture
    co-ordinates for all of the vertices that make up the sphere.

    Unlike QGLCube, the texture co-ordinates are fixed at constructions.  This
    is because constructing the sphere can involve generating additional
    vertices which need to interpolate the texture co-ordinates of their
    neighboring vertices.

    The default mode is a UVSphere.  Other choices include IcoSphere
    and CubeSphere.  UVSphere divides the sphere up into longitudinal and 
    latitudinal sections.  The longitudinal slices meet at the poles, which
    in a single unit sphere are defined to be at (0, 0, +0.5) and (0, 0, -0.5).
    This choice is the simplest to texture map as the texture will only distort
    along the x-axis of the 2D texture.  However the density of vertices is
    significantly higher at the poles than it is elsewhere on the sphere and
    is a poor choice if a uniform density of pixels from the texture map
    is required.

    The CubeSphere involves starting with a cube with its centroid at
    the origin and subdividing its faces such that new vectors are projected
    onto the sphere.  The cube sphere has a more even distributions of
    vertices while still being relatively simple to provide a texture map for.

    The IcoSphere starts with a 20 sided regular shape which then has each
    triangle divided and projected onto the sphere.  The IcoSphere has the
    most even distributions of vertices however also has the most complex
    mapping of texture points onto the sphere.  It is best used when its
    the pixel distribution of the texture should be as even as possible over
    the sphere.

    \sa QGLGeometry, QGLPainter
*/

/*!
    \enum QGLSphere::Mode
    \since 4.7
    This enum defines the method used to generate a sphere.
    \value UVSphere Faces have their vertices align along longitudinal and latitudinal divisions of the sphere.
    \value CubeSphere The initial shape is a cube, which then has each face repeatedly divided in to four new faces with new vertices projected onto the sphere.
    \value IcoSphere  The initial shape is an Icosahedron, which then has each
    face repeatedly divided into four new faces with new vertices projected onto the sphere.
*/

/*!
    Constructs the geometry for a sphere with a diameter of the given \a size
    and attaches it to \a parent.
    The method of generating the sphere is given by mode.

    Depending on the \a mode used, the sphere must have at least 6, 8, or 20 faces
    for the CubeSphere, UVSphere and IcoShere respectively.  The sphere
    will then be subdivided until the sphere can no longer be subdivided without exceeding
    the given \a maxFaces.
*/

QGLSphere::QGLSphere(qreal size, Mode mode, int maxFaces, QObject *parent)
    : QGLGeometry(parent)
{
    switch(mode) {
    case IcoSphere:
        initIco(size, maxFaces);
        break;
    case CubeSphere:
        initCube(size, maxFaces);
        break;
    case UVSphere:
        initUV(size, maxFaces);
        break;
    }
}

/*!
    Constructs the geometry for a sphere with a diameter of 1.0
    and attaches it to \a parent.
    The method of generating the sphere is given by mode.

    Depending on the mode used, the sphere must have at least 6, 8, or 20 faces
    for the CubeSphere, UVSphere and IcoShere respectively.  The sphere
    will then be subdivided until the sphere can no longer be subdivided without exceeding
    the given \a maxFaces.
*/
QGLSphere::QGLSphere(Mode mode, int maxFaces, QObject *parent)
    : QGLGeometry(parent)
{
    switch(mode) {
    case IcoSphere:
        initIco(1.0f, maxFaces);
        break;
    case CubeSphere:
        initCube(1.0f, maxFaces);
        break;
    case UVSphere:
        initUV(1.0f, maxFaces);
        break;
    }
}

/*!
    Destroys this sphere geometry object.
*/
QGLSphere::~QGLSphere() {}

// icosahedron is defined by phi, derived from the golden section
// http://en.wikipedia.org/wiki/Icosahedron#Cartesian_coordinates
const qreal phi = 1.618033988749894848f;

void QGLSphere::initIco(qreal scale, int maxFaces)
{
    qreal small = 1.0f;
    qreal large = phi*small;

    float ico[12][3] = {
        { 0.0f, small, large },    // A - 0
        { 0.0f, small, -large },   // B - 1
        { 0.0f, -small, large },   // C - 2
        { 0.0f, -small, -large },  // D - 3
        { small, large, 0.0f },    // E - 4
        { small, -large, 0.0f },   // F - 5
        { -small, large, 0.0f },   // G - 6
        { -small, -large, 0.0f },  // H - 7
        { large, 0.0f, small },    // I - 8
        { large, 0.0f, -small },   // J - 9
        { -large, 0.0f, small },   // K - 10
        { -large, 0.0f, -small }   // L - 11
    };

    int face[20][3] = {
        { 4, 0, 8 },            // E-A-I
        { 6, 0, 4 },            // G-A-E
        { 6, 10, 0 },           // G-K-A
        { 11, 10, 6 },          // L-K-G
        { 0, 2, 8 },            // A-C-I
        { 10, 2, 0 },           // K-C-A
        { 10, 7, 2 },           // K-H-C
        { 11, 7, 10 },          // L-H-K
        { 2, 5, 8 },            // C-F-I
        { 7, 5, 2 },            // H-F-C
        { 7, 3, 5 },            // H-D-F
        { 11, 3, 7 },           // L-D-H
        { 5, 9, 8 },            // F-J-I
        { 3, 9, 5 },            // D-J-F
        { 3, 1, 9 },            // D-B-J
        { 11, 1, 3 },           // L-B-D
        { 9, 4, 8 },            // J-E-I
        { 1, 4, 9 },            // B-E-J
        { 1, 6, 4 },            // B-G-E
        { 11, 6, 1 }            // L-G-B
    };

    const float u0 = 0.0f;
    const float u1 = 0.173205081f;
    const float u2 = 0.346410162f;
    const float u3 = 0.519615242f;
    const float u4 = 0.692820323f;
    const float u5 = 0.866025402f;
    const float v9 = 0.0f;
    const float v8 = 0.111111111f;
    const float v7 = 0.222222222f;
    const float v6 = 0.333333333f;
    const float v5 = 0.444444444f;
    const float v4 = 0.555555555f;
    const float v3 = 0.666666666f;
    const float v2 = 0.777777777f;
    const float v1 = 0.888888888f;
    const float v0 = 1.0f;

    float tex[20][3][2] = {
        { { u0, v1 }, { u1, v2 }, { u1, v0 } }, // E-A-I
        { { u0, v3 }, { u1, v2 }, { u0, v1 } }, // G-A-E
        { { u0, v3 }, { u1, v4 }, { u1, v2 } }, // G-K-A
        { { u0, v5 }, { u1, v4 }, { u0, v3 } }, // L-K-G
        { { u1, v2 }, { u2, v3 }, { u2, v1 } }, // A-C-I
        { { u1, v4 }, { u2, v3 }, { u1, v2 } }, // K-C-A
        { { u1, v4 }, { u2, v5 }, { u2, v3 } }, // K-H-C
        { { u1, v6 }, { u2, v5 }, { u1, v4 } }, // L-H-K
        { { u2, v3 }, { u3, v4 }, { u3, v2 } }, // C-F-I
        { { u2, v5 }, { u3, v4 }, { u2, v3 } }, // H-F-C
        { { u2, v5 }, { u3, v6 }, { u3, v4 } }, // H-D-F
        { { u2, v7 }, { u3, v6 }, { u2, v5 } }, // L-D-H
        { { u3, v4 }, { u4, v5 }, { u4, v3 } }, // F-J-I
        { { u3, v6 }, { u4, v5 }, { u3, v4 } }, // D-J-F
        { { u3, v6 }, { u4, v7 }, { u4, v5 } }, // D-B-J
        { { u3, v8 }, { u4, v7 }, { u3, v6 } }, // L-B-D
        { { u4, v5 }, { u5, v6 }, { u5, v4 } }, // J-E-I
        { { u4, v7 }, { u5, v6 }, { u4, v5 } }, // B-E-J
        { { u4, v7 }, { u5, v8 }, { u5, v6 } }, // B-G-E
        { { u4, v9 }, { u5, v8 }, { u4, v7 } }  // L-G-B
    };

    setDrawingMode(QGL::Triangles);
    QGLVertexArray vertices(QGL::Position, 3, QGL::Normal, 3, QGL::TextureCoord0, 2);

    for (int ix = 0; ix < 20; ++ix)
    {
        QVector3D n0(ico[face[ix][0]][0], ico[face[ix][0]][1], ico[face[ix][0]][2]);
        QVector3D n1(ico[face[ix][1]][0], ico[face[ix][1]][1], ico[face[ix][1]][2]);
        QVector3D n2(ico[face[ix][2]][0], ico[face[ix][2]][1], ico[face[ix][2]][2]);

        QVector2D t0(tex[ix][0][0], tex[ix][0][1]);
        QVector2D t1(tex[ix][1][0], tex[ix][1][1]);
        QVector2D t2(tex[ix][2][0], tex[ix][2][1]);

        n0 = n0.normalized();
        n1 = n1.normalized();
        n2 = n2.normalized();

        QVector3D v0 = n0 * scale / 2.0f;
        QVector3D v1 = n1 * scale / 2.0f;
        QVector3D v2 = n2 * scale / 2.0f;

        vertices.append(v0);
        vertices.append(n0);
        vertices.append(t0);
        vertices.append(v1);
        vertices.append(n1);
        vertices.append(t1);
        vertices.append(v2);
        vertices.append(n2);
        vertices.append(t2);
    }
    int faces = 20;
    while (faces < maxFaces/4) {
        vertices = subdivideTriangles(scale, vertices);
        faces *= 4;
    }
    setVertexArray(vertices);
}

void QGLSphere::initCube(qreal scale, int maxFaces)
{
    /*
              A-----H
              |     |
              |     |
        A-----D-----E-----H-----A
        |     |     |     |     |
        |     |     |     |     |
        B-----C-----F-----G-----B
              |     |
              |     |
              B-----G

       ^  d  e
       |  c  f
       y  
        x-->
    */

    const qreal offset = 1.0f;
    float cube[8][3] = {
        { -offset,  offset, -offset},    // A - 0
        { -offset, -offset, -offset },   // B - 1
        { -offset, -offset,  offset },   // C - 2
        { -offset,  offset,  offset },  // D - 3
        {  offset,  offset,  offset },   // E - 4
        {  offset, -offset,  offset },    // F - 5
        {  offset, -offset, -offset },   // G - 6
        {  offset,  offset, -offset },  // H - 7
    };

    int face[6][4] = {
        { 0, 1, 2, 3 }, // A-B-C-D
        { 3, 2, 5, 4 }, // D-C-F-E
        { 4, 5, 6, 7 }, // E-F-G-H
        { 7, 6, 1, 0 }, // H-G-B-A
        { 0, 3, 4, 7 }, // A-D-E-H
        { 2, 1, 6, 5 }, // C-B-G-F
    };

    const float v3 = 0.0f;
    const float v2 = 0.333333333f;
    const float v1 = 0.666666666f;
    const float v0 = 1.0f;

    const float u0 = 0.0f;
    const float u1 = 0.25f;
    const float u2 = 0.5f;
    const float u3 = 0.75f;
    const float u4 = 1.0f;

    float tex[6][4][2] = {
        { {u0, v1}, {u0, v2}, {u1, v2}, {u1, v1} }, // A-B-C-D
        { {u1, v1}, {u1, v2}, {u2, v2}, {u2, v1} }, // D-C-F-E
        { {u2, v1}, {u2, v2}, {u3, v2}, {u3, v1} }, // E-F-G-H
        { {u3, v1}, {u3, v2}, {u4, v2}, {u4, v1} }, // H-G-B-A
        { {u1, v0}, {u1, v1}, {u2, v1}, {u2, v0} }, // A-D-E-H
        { {u1, v2}, {u1, v3}, {u2, v3}, {u2, v2} }, // C-B-G-F
    };

    setDrawingMode(QGL::Triangles);
    QGLVertexArray vertices(QGL::Position, 3, QGL::Normal, 3, QGL::TextureCoord0, 2);

    for (int ix = 0; ix < 6; ++ix)
    {
        QVector3D n0(cube[face[ix][0]][0], cube[face[ix][0]][1], cube[face[ix][0]][2]);
        QVector3D n1(cube[face[ix][1]][0], cube[face[ix][1]][1], cube[face[ix][1]][2]);
        QVector3D n2(cube[face[ix][2]][0], cube[face[ix][2]][1], cube[face[ix][2]][2]);
        QVector3D n3(cube[face[ix][3]][0], cube[face[ix][3]][1], cube[face[ix][3]][2]);

        QVector2D t0(tex[ix][0][0], tex[ix][0][1]);
        QVector2D t1(tex[ix][1][0], tex[ix][1][1]);
        QVector2D t2(tex[ix][2][0], tex[ix][2][1]);
        QVector2D t3(tex[ix][3][0], tex[ix][3][1]);

        n0 = n0.normalized();
        n1 = n1.normalized();
        n2 = n2.normalized();
        n3 = n3.normalized();

        QVector3D v0 = n0 * scale / 2.0f;
        QVector3D v1 = n1 * scale / 2.0f;
        QVector3D v2 = n2 * scale / 2.0f;
        QVector3D v3 = n3 * scale / 2.0f;

        vertices.append(v0);
        vertices.append(n0);
        vertices.append(t0);
        vertices.append(v1);
        vertices.append(n1);
        vertices.append(t1);
        vertices.append(v2);
        vertices.append(n2);
        vertices.append(t2);
        vertices.append(v3);
        vertices.append(n3);
        vertices.append(t3);
    }
    int faces = 6;
    while (faces < maxFaces/4) {
        vertices = subdivideQuads(scale, vertices);
        faces *= 4;
    }
    setVertexArray(vertices);

    // Create an index array for converting the quad vertices into triangles.
    QGLIndexArray indices;
    int count = vertices.vertexCount();
    for (int index = 0; index < count; index += 4) {
        indices.append(index);
        indices.append(index + 1);
        indices.append(index + 2);

        indices.append(index);
        indices.append(index + 2);
        indices.append(index + 3);
    }
    setIndexArray(indices);
}

void QGLSphere::initUV(qreal scale, int maxFaces)
{
    // determine division by reversing max faces.
    int faces = 8;
    int divisions = 0;
    while (faces < maxFaces/4) {
        faces*= 4;
        divisions++;
    }

    // define a 0 division sphere as 4 points around the equator, 4 points around the bisection at poles.
    // each division doubles the number of points.
    // since each pass of each loop does half a sphere, we multiply by 2 rather than 4.
    int total = 2*(1 << divisions);

    setDrawingMode(QGL::Triangles);
    QGLVertexArray vertices(QGL::Position, 3, QGL::Normal, 3, QGL::TextureCoord0, 2);

    const QVector3D initialVector(0, 0, 1);
    const QVector3D zAxis(0, 0, 1);
    const QVector3D yAxis(0, 1, 0);
    for(int vindex = 0; vindex < total; vindex++) {
        qreal vFrom = qreal(vindex) / qreal(total);
        qreal vTo = qreal(vindex+1) / qreal(total);
        QQuaternion ryFrom = QQuaternion::fromAxisAndAngle(yAxis, 180.0f * vFrom);
        QQuaternion ryTo = QQuaternion::fromAxisAndAngle(yAxis, 180.0f * vTo);
        for (int uindex = 0; uindex < 2*total; uindex++) {
            qreal uFrom = qreal(uindex) / qreal(total);
            qreal uTo = qreal(uindex+1) / qreal(total);
            QQuaternion rzFrom = QQuaternion::fromAxisAndAngle(zAxis, 180.0f * uFrom);
            QQuaternion rzTo = QQuaternion::fromAxisAndAngle(zAxis, 180.0f * uTo);
            // four points
            QVector3D na, nb, nc, nd;
            QVector3D va, vb, vc, vd;

            na = ryFrom.rotatedVector(initialVector);
            na = rzFrom.rotatedVector(na);
            
            nb = ryTo.rotatedVector(initialVector);
            nb = rzFrom.rotatedVector(nb);

            nc = ryTo.rotatedVector(initialVector);
            nc = rzTo.rotatedVector(nc);

            nd = ryFrom.rotatedVector(initialVector);
            nd = rzTo.rotatedVector(nd);

            QVector2D ta(uFrom/2.0f, 1.0-vFrom);
            QVector2D tb(uFrom/2.0f, 1.0-vTo);
            QVector2D tc(uTo/2.0f, 1.0-vTo);
            QVector2D td(uTo/2.0f, 1.0-vFrom);

            va = na * scale / 2.0f;
            vb = nb * scale / 2.0f;
            vc = nc * scale / 2.0f;
            vd = nd * scale / 2.0f;

            vertices.append(va);
            vertices.append(na);
            vertices.append(ta);
            vertices.append(vb);
            vertices.append(nb);
            vertices.append(tb);
            vertices.append(vc);
            vertices.append(nc);
            vertices.append(tc);

            vertices.append(va);
            vertices.append(na);
            vertices.append(ta);
            vertices.append(vc);
            vertices.append(nc);
            vertices.append(tc);
            vertices.append(vd);
            vertices.append(nd);
            vertices.append(td);
        }
    }
    setVertexArray(vertices);
}

QGLVertexArray QGLSphere::subdivideTriangles(qreal scale, const QGLVertexArray &array)
{
    QGLVertexArray vertices(QGL::Position, 3, QGL::Normal, 3, QGL::TextureCoord0, 2);

    for (int i = 0; i < array.vertexCount(); i+= 3) {
        QVector3D v0 = array.vector3DAt(i  , 0);
        QVector3D n0 = array.vector3DAt(i  , 1);
        QVector3D v1 = array.vector3DAt(i+1, 0);
        QVector3D n1 = array.vector3DAt(i+1, 1);
        QVector3D v2 = array.vector3DAt(i+2, 0);
        QVector3D n2 = array.vector3DAt(i+2, 1);

        QVector2D t0 = array.vector2DAt(i, 2);
        QVector2D t1 = array.vector2DAt(i+1, 2);
        QVector2D t2 = array.vector2DAt(i+2, 2);

        QVector3D n01 = (v0 + v1).normalized();
        QVector3D n12 = (v1 + v2).normalized();
        QVector3D n20 = (v2 + v0).normalized();
        QVector3D v01 = n01 * scale / 2.0f;
        QVector3D v12 = n12 * scale / 2.0f;
        QVector3D v20 = n20 * scale / 2.0f;

        QVector2D t01 = (t0 + t1) / 2;
        QVector2D t12 = (t1 + t2) / 2;
        QVector2D t20 = (t2 + t0) / 2;

        vertices.append(v0);
        vertices.append(n0);
        vertices.append(t0);
        vertices.append(v01);
        vertices.append(n01);
        vertices.append(t01);
        vertices.append(v20);
        vertices.append(n20);
        vertices.append(t20);

        vertices.append(v01);
        vertices.append(n01);
        vertices.append(t01);
        vertices.append(v1);
        vertices.append(n1);
        vertices.append(t1);
        vertices.append(v12);
        vertices.append(n12);
        vertices.append(t12);

        vertices.append(v01);
        vertices.append(n01);
        vertices.append(t01);
        vertices.append(v12);
        vertices.append(n12);
        vertices.append(t12);
        vertices.append(v20);
        vertices.append(n20);
        vertices.append(t20);

        vertices.append(v20);
        vertices.append(n20);
        vertices.append(t20);
        vertices.append(v12);
        vertices.append(n12);
        vertices.append(t12);
        vertices.append(v2);
        vertices.append(n2);
        vertices.append(t2);
    }
    return vertices;
}

QGLVertexArray QGLSphere::subdivideQuads(qreal scale, const QGLVertexArray &array)
{
    QGLVertexArray vertices(QGL::Position, 3, QGL::Normal, 3, QGL::TextureCoord0, 2);

    for (int i = 0; i < array.vertexCount(); i+= 4) {
        QVector3D v0 = array.vector3DAt(i  , 0);
        QVector3D n0 = array.vector3DAt(i  , 1);
        QVector3D v1 = array.vector3DAt(i+1, 0);
        QVector3D n1 = array.vector3DAt(i+1, 1);
        QVector3D v2 = array.vector3DAt(i+2, 0);
        QVector3D n2 = array.vector3DAt(i+2, 1);
        QVector3D v3 = array.vector3DAt(i+3, 0);
        QVector3D n3 = array.vector3DAt(i+3, 1);

        QVector2D t0 = array.vector2DAt(i, 2);
        QVector2D t1 = array.vector2DAt(i+1, 2);
        QVector2D t2 = array.vector2DAt(i+2, 2);
        QVector2D t3 = array.vector2DAt(i+3, 2);

        QVector3D n01 = (v0 + v1).normalized();
        QVector3D n12 = (v1 + v2).normalized();
        QVector3D n23 = (v2 + v3).normalized();
        QVector3D n30 = (v3 + v0).normalized();
        QVector3D nc = (v0 + v1 + v2 + v3).normalized();
        QVector3D v01 = n01 * scale / 2.0f;
        QVector3D v12 = n12 * scale / 2.0f;
        QVector3D v23 = n23 * scale / 2.0f;
        QVector3D v30 = n30 * scale / 2.0f;
        QVector3D vc = nc * scale / 2.0f;

        QVector2D t01 = (t0 + t1) / 2;
        QVector2D t12 = (t1 + t2) / 2;
        QVector2D t23 = (t2 + t3) / 2;
        QVector2D t30 = (t3 + t0) / 2;
        QVector2D tc = (t2 + t0) / 2;

        vertices.append(v0);
        vertices.append(n0);
        vertices.append(t0);
        vertices.append(v01);
        vertices.append(n01);
        vertices.append(t01);
        vertices.append(vc);
        vertices.append(nc);
        vertices.append(tc);
        vertices.append(v30);
        vertices.append(n30);
        vertices.append(t30);

        vertices.append(v01);
        vertices.append(n01);
        vertices.append(t01);
        vertices.append(v1);
        vertices.append(n1);
        vertices.append(t1);
        vertices.append(v12);
        vertices.append(n12);
        vertices.append(t12);
        vertices.append(vc);
        vertices.append(nc);
        vertices.append(tc);

        vertices.append(vc);
        vertices.append(nc);
        vertices.append(tc);
        vertices.append(v12);
        vertices.append(n12);
        vertices.append(t12);
        vertices.append(v2);
        vertices.append(n2);
        vertices.append(t2);
        vertices.append(v23);
        vertices.append(n23);
        vertices.append(t23);

        vertices.append(v30);
        vertices.append(n30);
        vertices.append(t30);
        vertices.append(vc);
        vertices.append(nc);
        vertices.append(tc);
        vertices.append(v23);
        vertices.append(n23);
        vertices.append(t23);
        vertices.append(v3);
        vertices.append(n3);
        vertices.append(t3);
    }
    return vertices;
}

QT_END_NAMESPACE
