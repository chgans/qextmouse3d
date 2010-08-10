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
#include "qglbuilder.h"
#include "qvector2darray.h"
#include "qvector3darray.h"
#include <QtGui/qquaternion.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLSphere
    \brief The QGLSphere class represents the geometry of a simple sphere in 3D space.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::geometry

    The following example creates a sphere of 2 units in diameter and
    draws it at (10, 25, 0) in a QGLPainter:

    \code
    QGLBuilder list;
    list << QGLSphere(2);
    painter.translate(10, 25, 0);
    list.draw(&painter);
    \endcode;

    The QGLSphere class specifies positions, normals and 2d texture
    co-ordinates for all of the vertices that make up the sphere.

    The texture co-ordinates are fixed at construction time.  This
    is because constructing the sphere can involve generating additional
    vertices which need to interpolate the texture co-ordinates of their
    neighboring vertices.

    The default mode of QGLSphere is a "UV sphere", which divides the
    sphere up into longitudinal and latitudinal sections.  The longitudinal
    slices meet at the poles, which in a single unit sphere are defined to
    be at (0, 0, +0.5) and (0, 0, -0.5).  This choice is the simplest to
    texture map as the texture will only distort along the x-axis of the
    2D texture.  However the density of vertices is significantly higher at
    the poles than it is elsewhere on the sphere and is a poor choice if a
    uniform density of pixels from the texture map is required.

    The companion QGLIcoSphere and QGLCubeSphere classes provide
    alternative division algorithms that can give better results
    for texture mapping.

    \sa QGLIcoSphere, QGLCubeSphere, QGLBuilder
*/

/*!
    \fn QGLSphere::QGLSphere(qreal diameter, int depth)

    Creates a sphere of \a diameter across (default is 1).  When the sphere
    is subdivided into triangles, it will be subdivided no more than
    \a depth times (default is 3).
*/

/*!
    Destroys this sphere object.
*/
QGLSphere::~QGLSphere()
{
}

/*!
    \fn qreal QGLSphere::diameter() const

    Returns the diameter of this sphere.  The default is 1.

    \sa setDiameter()
*/

/*!
    \fn void QGLSphere::setDiameter(qreal diameter)

    Sets the diameter of this sphere to \a diameter.

    \sa diameter()
*/

/*!
    \fn int QGLSphere::subdivisionDepth() const

    Returns the maximum depth when this sphere is subdivided into triangles.
    The default is 3.

    \sa setSubdivisionDepth()
*/

/*!
    \fn void QGLSphere::setSubdivisionDepth(int depth)

    Sets the maximum \a depth when this sphere is subdivided into triangles.

    \sa subdivisionDepth()
*/

/*!
    \class QGLIcoSphere
    \brief The QGLIcoSphere class represents the geometry of a simple sphere in 3D space derived from an icosahedron.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::geometry

    QGLIcoSphere starts with a 20 sided regular shape which then has each
    triangle divided and projected onto the sphere.  The QGLIcoSphere has a
    more even distribution of vertices than QGLSphere and QGLCubeSphere.
    However it also has the most complex mapping of texture points onto the
    sphere.  It is best used when its the pixel distribution of the
    texture should be as even as possible over the sphere.

    \sa QGLSphere, QGLCubeSphere, QGLBuilder
*/

/*!
    \fn QGLIcoSphere::QGLIcoSphere(qreal diameter, int depth)

    Creates a sphere of \a diameter across (default is 1).  When the sphere
    is subdivided into triangles, it will be derived from an icosahedron
    and subdivided no more than \a depth times (default is 3).
*/

/*!
    \class QGLCubeSphere
    \brief The QGLCubeSphere class represents the geometry of a simple sphere in 3D space derived by subdividing a cube.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::geometry

    QGLCubeSphere starts with a cube with its centroid at the origin and
    subdivides its faces such that new vectors are projected onto the sphere.
    The cube sphere has a more even distributions of vertices than
    QGLSphere while still being relatively simple to provide a texture map for.

    \sa QGLSphere, QGLIcoSphere, QGLBuilder
*/

/*!
    \fn QGLCubeSphere::QGLCubeSphere(qreal diameter, int depth)

    Creates a sphere of \a diameter across (default is 1).  When the sphere
    is subdivided into triangles, it will be derived from a cube
    and subdivided no more than \a depth times (default is 3).
*/

/*!
    \relates QGLSphere

    Builds the geometry for \a sphere within the specified
    display \a list.
*/
QGLBuilder& operator<<(QGLBuilder& list, const QGLSphere& sphere)
{
    qreal scale = sphere.diameter();
    int divisions = qMax(sphere.subdivisionDepth() - 1, 0);

    // define a 0 division sphere as 4 points around the equator, 4 points around the bisection at poles.
    // each division doubles the number of points.
    // since each pass of each loop does half a sphere, we multiply by 2 rather than 4.
    int total = 2*(1 << divisions);

    //list.begin(QGL::TRIANGLE);
    //QGeometryData *prim = list.currentPrimitive();
    QGeometryData prim;

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

            prim.appendVertex(va, vb, vc);
            prim.appendNormal(na, nb, nc);
            prim.appendTexCoord(ta, tb, tc);

            prim.appendVertex(va, vc, vd);
            prim.appendNormal(na, nc, nd);
            prim.appendTexCoord(ta, tc, td);
        }
    }

    list.addTriangles(prim);
    return list;
}

// icosahedron is defined by phi, derived from the golden section
// http://en.wikipedia.org/wiki/Icosahedron#Cartesian_coordinates
const qreal phi = 1.618033988749894848f;

/*!
    \relates QGLIcoSphere

    Builds the geometry for \a sphere within the specified
    display \a list.
*/
QGLBuilder& operator<<(QGLBuilder& list, const QGLIcoSphere& sphere)
{
    qreal scale = sphere.diameter();
    int depth = sphere.subdivisionDepth();
    qreal tiny= 1.0f;
    qreal large = phi*tiny;

    float ico[12][3] = {
        { 0.0f, tiny, large },    // A - 0
        { 0.0f, tiny, -large },   // B - 1
        { 0.0f, -tiny, large },   // C - 2
        { 0.0f, -tiny, -large },  // D - 3
        { tiny, large, 0.0f },    // E - 4
        { tiny, -large, 0.0f },   // F - 5
        { -tiny, large, 0.0f },   // G - 6
        { -tiny, -large, 0.0f },  // H - 7
        { large, 0.0f, tiny},    // I - 8
        { large, 0.0f, -tiny},   // J - 9
        { -large, 0.0f, tiny},   // K - 10
        { -large, 0.0f, -tiny}   // L - 11
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

    // Generate the initial vertex list from a plain icosahedron.
    QVector3DArray vertices;
    QVector3DArray normals;
    QVector2DArray texCoords;
    for (int ix = 0; ix < 20; ++ix) {
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

        vertices.append(v0, v1, v2);
        normals.append(n0, n1, n2);
        texCoords.append(t0, t1, t2);
    }

    // Subdivide the icosahedron.
    while (depth-- > 1) {
        QVector3DArray newVertices;
        QVector3DArray newNormals;
        QVector2DArray newTexCoords;

        int count = vertices.count();
        for (int i = 0; i < count; i+= 3) {
            QVector3D v0 = vertices.at(i);
            QVector3D v1 = vertices.at(i+1);
            QVector3D v2 = vertices.at(i+2);

            QVector3D n0 = normals.at(i);
            QVector3D n1 = normals.at(i+1);
            QVector3D n2 = normals.at(i+2);

            QVector2D t0 = texCoords.at(i);
            QVector2D t1 = texCoords.at(i+1);
            QVector2D t2 = texCoords.at(i+2);

            QVector3D n01 = (v0 + v1).normalized();
            QVector3D n12 = (v1 + v2).normalized();
            QVector3D n20 = (v2 + v0).normalized();
            QVector3D v01 = n01 * scale / 2.0f;
            QVector3D v12 = n12 * scale / 2.0f;
            QVector3D v20 = n20 * scale / 2.0f;

            QVector2D t01 = (t0 + t1) / 2;
            QVector2D t12 = (t1 + t2) / 2;
            QVector2D t20 = (t2 + t0) / 2;

            newVertices.append(v0, v01, v20);
            newNormals.append(n0, n01, n20);
            newTexCoords.append(t0, t01, t20);

            newVertices.append(v01, v1, v12);
            newNormals.append(n01, n1, n12);
            newTexCoords.append(t01, t1, t12);

            newVertices.append(v01, v12, v20);
            newNormals.append(n01, n12, n20);
            newTexCoords.append(t01, t12, t20);

            newVertices.append(v20, v12, v2);
            newNormals.append(n20, n12, n2);
            newTexCoords.append(t20, t12, t2);
        }

        vertices = newVertices;
        normals = newNormals;
        texCoords = newTexCoords;
    }

    // Add the final vertices to the builder.
    QGeometryData prim;
    prim.appendVertexArray(vertices);
    prim.appendNormalArray(normals);
    prim.appendTexCoordArray(texCoords);
    list.addTriangles(prim);
    return list;
}

/*!
    \relates QGLCubeSphere

    Builds the geometry for \a sphere within the specified
    display \a list.
*/
QGLBuilder& operator<<(QGLBuilder& list, const QGLCubeSphere& sphere)
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

    qreal scale = sphere.diameter();
    int depth = sphere.subdivisionDepth();

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

    // Generate the initial vertex list from a plain cube.
    QVector3DArray vertices;
    QVector3DArray normals;
    QVector2DArray texCoords;
    for (int ix = 0; ix < 6; ++ix) {
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

        vertices.append(v0, v1, v2, v3);
        normals.append(n0, n1, n2, n3);
        texCoords.append(t0, t1, t2, t3);
    }

    // Subdivide the cube.
    while (depth-- > 1) {
        QVector3DArray newVertices;
        QVector3DArray newNormals;
        QVector2DArray newTexCoords;

        int count = vertices.count();
        for (int i = 0; i < count; i+= 4) {
            QVector3D v0 = vertices.at(i);
            QVector3D v1 = vertices.at(i+1);
            QVector3D v2 = vertices.at(i+2);
            QVector3D v3 = vertices.at(i+3);

            QVector3D n0 = normals.at(i);
            QVector3D n1 = normals.at(i+1);
            QVector3D n2 = normals.at(i+2);
            QVector3D n3 = normals.at(i+3);

            QVector2D t0 = texCoords.at(i);
            QVector2D t1 = texCoords.at(i+1);
            QVector2D t2 = texCoords.at(i+2);
            QVector2D t3 = texCoords.at(i+3);

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

            newVertices.append(v0, v01, vc, v30);
            newNormals.append(n0, n01, nc, n30);
            newTexCoords.append(t0, t01, tc, t30);

            newVertices.append(v01, v1, v12, vc);
            newNormals.append(n01, n1, n12, nc);
            newTexCoords.append(t01, t1, t12, tc);

            newVertices.append(vc, v12, v2, v23);
            newNormals.append(nc, n12, n2, n23);
            newTexCoords.append(tc, t12, t2, t23);

            newVertices.append(v30, vc, v23, v3);
            newNormals.append(n30, nc, n23, n3);
            newTexCoords.append(t30, tc, t23, t3);
        }

        vertices = newVertices;
        normals = newNormals;
        texCoords = newTexCoords;
    }

    // Add the final vertices to the geometry builder.
    QGeometryData prim;
    prim.appendVertexArray(vertices);
    prim.appendNormalArray(normals);
    prim.appendTexCoordArray(texCoords);
    list.addTriangles(prim);
    return list;
}

QT_END_NAMESPACE
