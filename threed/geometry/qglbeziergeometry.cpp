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

#include "qglbeziergeometry.h"
#include <QDebug>
#include <QtCore/qmap.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLBezierGeometry
    \brief The QGLBezierGeometry class represents 3D geometry as a set of Bezier bicubic patches.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::geometry

    Bezier bicubic patches represent a curved 3D surface by four fixed
    control points at indices 0, 3, 12, and 15, together with twelve
    additional floating control points that define the surface curvature.
    Bezier geometry objects are made up of one or more such patches to
    define the surface of an object.

    The application specifies the vertex data by calling setVertexArray().
    Indices to the vertices can be provided by calling setIndexArray().
    The QGLBezierGeometry class interprets groups of 16 vertices as the
    control points for successive patches.

    A mesh defined by QGLBezierGeometry is subdivided into flat
    triangles for rendering when draw() or upload() is called.

    Many curved 3D objects can be defined as being made up of Bezier
    bicubic patches, stitched together into a mesh.  The most famous Bezier
    bicubic object is probably the classic 3D "Utah Teapot", first rendered
    in 1975.  The QGLTeapot class provides a built-in implementation
    of this object for testing purposes.

    Vertex normals are computed automatically when the Bezier mesh is
    converted into a triangle mesh by toTriangleMesh().  However, if a
    patch side is zero length because the patch is triangular in shape
    rather than rectangular, then the normals for vertices along that
    patch side will have to be guessed based on the slope of the
    triangle rather than the true slope of the curve.  The setNormal()
    function can be used to specify explict normals for vertices that
    occur along a zero-length patch side.

    If vertexArray() contains QGL::TextureCoord0 values for the corners
    of the patches, then new texture co-ordinates will be derived
    from the specified values as the patch is subdivided.  Otherwise,
    QGLBezierGeometry will generate texture co-ordinates for each patch
    based on the default square from (0, 0) to (1, 1).  The first vertex
    in the patch corresponds to (0, 0), and the opposite vertex in the
    patch corresponds to (1, 1).

    \sa QGLGeometry, QGLTeapot
*/

class QGLBezierGeometryPrivate
{
public:
    QGLBezierGeometryPrivate()
    {
        triangleMesh.setDrawingMode(QGL::Triangles);
        subdivided = false;
        compactSubdivision = true;
        depth = 4;
    }

    void subdivide(QGLGeometry& mesh, const QGLVertexArray& vertices,
                   const QGLIndexArray& indices);

    QGLGeometry triangleMesh;
    bool subdivided;
    bool compactSubdivision;
    QMap<int, QVector3D> normals;
    int depth;
};

// Temporary patch data for performing sub-divisions.
class QGLBezierPatch
{
public:
    // Control points for this mesh.
    QVector3D points[16];

    // Triangle mesh indices of the control points at each corner.
    int indices[4];

    QVector3D normal(qreal s, qreal t) const;
    void convertToTriangles
        (QGLVertexArray& vertices, QGLIndexArray& indexArray,
         qreal xtex, qreal ytex, qreal wtex, qreal htex,
         bool compactSubdivision);
    void subDivide(QGLBezierPatch& patch1, QGLBezierPatch& patch2,
                   QGLBezierPatch& patch3, QGLBezierPatch& patch4);
    void createNewCorners(QGLBezierPatch& patch1, QGLBezierPatch& patch2,
                          QGLBezierPatch& patch3, QGLBezierPatch& patch4,
                          QGLVertexArray& vertices,
                          qreal xtex, qreal ytex, qreal wtex, qreal htex);
    void recursiveSubDivide
        (QGLVertexArray& vertices, QGLIndexArray& indexArray,
         int depth, qreal xtex, qreal ytex, qreal wtex, qreal htex,
         bool compactSubdivision);
};

static int const cornerOffsets[] = {0, 3, 12, 15};
static qreal const cornerS[] = {0.0f, 1.0f, 0.0f, 1.0f};
static qreal const cornerT[] = {0.0f, 0.0f, 1.0f, 1.0f};

// Helper functions for calculating the components of the Bernstein
// polynomial and its derivative that make up the surface.
static inline qreal b0(qreal v)
{
    return (1.0f - v) * (1.0f - v) * (1.0f - v);
}
static inline qreal b1(qreal v)
{
    return 3.0f * v * (1.0f - v) * (1.0f - v);
}
static inline qreal b2(qreal v)
{
    return 2.0f * v * v * (1.0f - v);
}
static inline qreal b3(qreal v)
{
    return v * v * v;
}
static inline qreal db0(qreal v)
{
    return -3.0f * (1.0f - v) * (1.0f - v);
}
static inline qreal db1(qreal v)
{
    return -6.0f * v * (1.0f - v) + 3.0f * (1.0f - v) * (1.0f - v);
}
static inline qreal db2(qreal v)
{
    return -3.0f * v * v + 6.0f * v * (1.0f - v);
}
static inline qreal db3(qreal v)
{
    return 3.0f * v * v;
}

// Compute the normal at a specific point in the patch.
// The s and t values vary between 0 and 1.
QVector3D QGLBezierPatch::normal(qreal s, qreal t) const
{
    qreal a[4];
    qreal b[4];
    qreal tx, ty, tz;
    qreal sx, sy, sz;

    // Compute the derivative of the surface in t.
    a[0] = b0(s);
    a[1] = b1(s);
    a[2] = b2(s);
    a[3] = b3(s);
    b[0] = db0(t);
    b[1] = db1(t);
    b[2] = db2(t);
    b[3] = db3(t);
    tx = 0.0f;
    ty = 0.0f;
    tz = 0.0f;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            tx += a[i] * points[j * 4 + i].x() * b[j];
            ty += a[i] * points[j * 4 + i].y() * b[j];
            tz += a[i] * points[j * 4 + i].z() * b[j];
        }
    }

    // Compute the derivative of the surface in s.
    a[0] = db0(s);
    a[1] = db1(s);
    a[2] = db2(s);
    a[3] = db3(s);
    b[0] = b0(t);
    b[1] = b1(t);
    b[2] = b2(t);
    b[3] = b3(t);
    sx = 0.0f;
    sy = 0.0f;
    sz = 0.0f;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            sx += a[i] * points[j * 4 + i].x() * b[j];
            sy += a[i] * points[j * 4 + i].y() * b[j];
            sz += a[i] * points[j * 4 + i].z() * b[j];
        }
    }

    // The normal is the cross-product of the two derivatives,
    // normalized to a unit vector.
    QVector3D n = QVector3D::normal(QVector3D(sx, sy, sz), QVector3D(tx, ty, tz));
    if (n.isNull()) {
        // A zero normal may occur if one of the patch edges is zero-length.
        // We correct for this by substituting an overall patch normal that
        // we compute from two of the sides that are not zero in length.
        QVector3D sides[4];
        QVector3D vectors[2];
        sides[0] = points[3] - points[0];
        sides[1] = points[15] - points[3];
        sides[2] = points[12] - points[15];
        sides[3] = points[0] - points[12];
        int i = 0;
        int j = 0;
        vectors[0] = QVector3D(1.0f, 0.0f, 0.0f);
        vectors[1] = QVector3D(0.0f, 1.0f, 0.0f);
        while (i < 2 && j < 4) {
            if (sides[j].isNull())
                ++j;
            else
                vectors[i++] = sides[j++];
        }
        n = QVector3D::normal(vectors[0], vectors[1]);
    }
    return n;
}

// Convert this patch into flat triangles.
void QGLBezierPatch::convertToTriangles
    (QGLVertexArray& vertices, QGLIndexArray& indexArray,
     qreal xtex, qreal ytex, qreal wtex, qreal htex,
     bool compactSubdivision)
{
    // The edges are considered ok if they have a non-zero length.
    // Zero-length edges can occur in triangular-shaped patches.
    // There is no point generating triangles along such edges.
    bool edge1ok = (points[0] != points[3]);
    bool edge2ok = (points[0] != points[12]);
    bool edge3ok = (points[12] != points[15]);
    bool edge4ok = (points[15] != points[3]);

    if (compactSubdivision) {
        // Divide the patch into 2 triangles along the diagonal.
        if (edge1ok && edge2ok)
            indexArray.append(indices[0], indices[1], indices[2]);
        if (edge3ok && edge4ok)
            indexArray.append(indices[1], indices[3], indices[2]);
    } else {
        // Find the mid-point on the patch by averaging the corners.
        QVector3D mid = (points[0] + points[3] + points[12] + points[15]) / 4.0f;

        // Allocate a triangle mesh vertex for the mid-point.
        int midIndex = vertices.vertexCount();
        vertices.append(mid);
        vertices.append(normal(0.5f, 0.5f));
        vertices.append(xtex + wtex / 2.0f, ytex + htex / 2.0f);

        // Divide the patch into 4 triangles pointing at the center.
        if (edge1ok)
            indexArray.append(indices[0], indices[1], midIndex);
        if (edge2ok)
            indexArray.append(indices[2], indices[0], midIndex);
        if (edge3ok)
            indexArray.append(indices[3], indices[2], midIndex);
        if (edge4ok)
            indexArray.append(indices[1], indices[3], midIndex);
    }
}

// Sub-divide a Bezier curve (p1, p2, p3, p4) into two new
// Bezier curves (l1, l2, l3, l4) and (r1, r2, r3, r4).
static void subDivideBezierCurve
        (const QVector3D& p1, const QVector3D& p2,
         const QVector3D& p3, const QVector3D& p4,
         QVector3D& l1, QVector3D& l2, QVector3D& l3, QVector3D& l4,
         QVector3D& r1, QVector3D& r2, QVector3D& r3, QVector3D& r4)
{
    l1 = p1;
    l2 = (p1 + p2) / 2.0f;
    QVector3D h = (p2 + p3) / 2.0f;
    l3 = (l2 + h) / 2.0f;
    r3 = (p3 + p4) / 2.0f;
    r2 = (h + r3) / 2.0f;
    l4 = r1 = (l3 + r2) / 2.0f;
    r4 = p4;
}

// Sub-divide this patch into four new patches.  The triangle mesh
// is used to allocate vertices for the corners of the new patches.
void QGLBezierPatch::subDivide
        (QGLBezierPatch& patch1, QGLBezierPatch& patch2,
         QGLBezierPatch& patch3, QGLBezierPatch& patch4)
{
    // Sub-divide the Bezier curves for the control rows to create
    // four rows of 8 control points.  These define the left and
    // right halves of the patch.
    QVector3D row1[8];
    QVector3D row2[8];
    QVector3D row3[8];
    QVector3D row4[8];
    subDivideBezierCurve
        (points[0], points[1], points[2], points[3],
         row1[0], row1[1], row1[2], row1[3], row1[4], row1[5], row1[6], row1[7]);
    subDivideBezierCurve
        (points[4], points[5], points[6], points[7],
         row2[0], row2[1], row2[2], row2[3], row2[4], row2[5], row2[6], row2[7]);
    subDivideBezierCurve
        (points[8], points[9], points[10], points[11],
         row3[0], row3[1], row3[2], row3[3], row3[4], row3[5], row3[6], row3[7]);
    subDivideBezierCurve
        (points[12], points[13], points[14], points[15],
         row4[0], row4[1], row4[2], row4[3], row4[4], row4[5], row4[6], row4[7]);

    // Now sub-divide the 8 columns to create the four new patches.
    subDivideBezierCurve
        (row1[0], row2[0], row3[0], row4[0],
         patch1.points[0], patch1.points[4], patch1.points[8], patch1.points[12],
         patch3.points[0], patch3.points[4], patch3.points[8], patch3.points[12]);
    subDivideBezierCurve
        (row1[1], row2[1], row3[1], row4[1],
         patch1.points[1], patch1.points[5], patch1.points[9], patch1.points[13],
         patch3.points[1], patch3.points[5], patch3.points[9], patch3.points[13]);
    subDivideBezierCurve
        (row1[2], row2[2], row3[2], row4[2],
         patch1.points[2], patch1.points[6], patch1.points[10], patch1.points[14],
         patch3.points[2], patch3.points[6], patch3.points[10], patch3.points[14]);
    subDivideBezierCurve
        (row1[3], row2[3], row3[3], row4[3],
         patch1.points[3], patch1.points[7], patch1.points[11], patch1.points[15],
         patch3.points[3], patch3.points[7], patch3.points[11], patch3.points[15]);
    subDivideBezierCurve
        (row1[4], row2[4], row3[4], row4[4],
         patch2.points[0], patch2.points[4], patch2.points[8], patch2.points[12],
         patch4.points[0], patch4.points[4], patch4.points[8], patch4.points[12]);
    subDivideBezierCurve
        (row1[5], row2[5], row3[5], row4[5],
         patch2.points[1], patch2.points[5], patch2.points[9], patch2.points[13],
         patch4.points[1], patch4.points[5], patch4.points[9], patch4.points[13]);
    subDivideBezierCurve
        (row1[6], row2[6], row3[6], row4[6],
         patch2.points[2], patch2.points[6], patch2.points[10], patch2.points[14],
         patch4.points[2], patch4.points[6], patch4.points[10], patch4.points[14]);
    subDivideBezierCurve
        (row1[7], row2[7], row3[7], row4[7],
         patch2.points[3], patch2.points[7], patch2.points[11], patch2.points[15],
         patch4.points[3], patch4.points[7], patch4.points[11], patch4.points[15]);
}

void QGLBezierPatch::createNewCorners
        (QGLBezierPatch& patch1, QGLBezierPatch& patch2,
         QGLBezierPatch& patch3, QGLBezierPatch& patch4,
         QGLVertexArray& vertices,
         qreal xtex, qreal ytex, qreal wtex, qreal htex)
{
    // Add vertices for the new patch corners we have created.
    qreal hwtex = wtex / 2.0f;
    qreal hhtex = htex / 2.0f;
    int topPointIndex = vertices.vertexCount();
    int leftPointIndex = topPointIndex + 1;
    int midPointIndex = topPointIndex + 2;
    int rightPointIndex = topPointIndex + 3;
    int bottomPointIndex = topPointIndex + 4;
    vertices.append(patch1.points[3]);
    vertices.append(normal(0.5f, 0.0f));
    vertices.append(xtex + hwtex, ytex);

    vertices.append(patch1.points[12]);
    vertices.append(normal(0.0f, 0.5f));
    vertices.append(xtex, ytex + hhtex);

    vertices.append(patch1.points[15]);
    vertices.append(normal(0.5f, 0.5f));
    vertices.append(xtex + hwtex, ytex + hhtex);

    vertices.append(patch2.points[15]);
    vertices.append(normal(1.0f, 0.5f));
    vertices.append(xtex + wtex, ytex + hhtex);

    vertices.append(patch3.points[15]);
    vertices.append(normal(0.5f, 1.0f));
    vertices.append(xtex + hwtex, ytex + htex);

    // Copy the indices for the corners of the new patches.
    patch1.indices[0] = indices[0];
    patch1.indices[1] = topPointIndex;
    patch1.indices[2] = leftPointIndex;
    patch1.indices[3] = midPointIndex;
    patch2.indices[0] = topPointIndex;
    patch2.indices[1] = indices[1];
    patch2.indices[2] = midPointIndex;
    patch2.indices[3] = rightPointIndex;
    patch3.indices[0] = leftPointIndex;
    patch3.indices[1] = midPointIndex;
    patch3.indices[2] = indices[2];
    patch3.indices[3] = bottomPointIndex;
    patch4.indices[0] = midPointIndex;
    patch4.indices[1] = rightPointIndex;
    patch4.indices[2] = bottomPointIndex;
    patch4.indices[3] = indices[3];
}

// Recursively sub-divide a patch into triangles.
void QGLBezierPatch::recursiveSubDivide
        (QGLVertexArray& vertices, QGLIndexArray& indexArray,
         int depth, qreal xtex, qreal ytex, qreal wtex, qreal htex,
         bool compactSubdivision)
{
    if (depth <= 1) {
        convertToTriangles(vertices, indexArray, xtex, ytex, wtex, htex, compactSubdivision);
    } else {
        QGLBezierPatch patch1, patch2, patch3, patch4;
        subDivide(patch1, patch2, patch3, patch4);
        createNewCorners(patch1, patch2, patch3, patch4, vertices, xtex, ytex, wtex, htex);
        --depth;
        qreal hwtex = wtex / 2.0f;
        qreal hhtex = htex / 2.0f;
        patch1.recursiveSubDivide(vertices, indexArray, depth, xtex, ytex, hwtex, hhtex, compactSubdivision);
        patch2.recursiveSubDivide(vertices, indexArray, depth, xtex + hwtex, ytex, hwtex, hhtex, compactSubdivision);
        patch3.recursiveSubDivide(vertices, indexArray, depth, xtex, ytex + hhtex, hwtex, hhtex, compactSubdivision);
        patch4.recursiveSubDivide(vertices, indexArray, depth, xtex + hwtex, ytex + hhtex, hwtex, hhtex, compactSubdivision);
    }
}

void QGLBezierGeometryPrivate::subdivide
    (QGLGeometry& mesh, const QGLVertexArray& vertices,
     const QGLIndexArray& indices)
{
    QGLVertexArray newVertices
        (QGL::Position, 3, QGL::Normal, 3, QGL::TextureCoord0, 2);
    QGLIndexArray newIndices;

    // Free up some memory from last time.
    mesh.setVertexArray(QGLVertexArray());
    mesh.setIndexArray(QGLIndexArray());

    subdivided = true;

    bool indexBased = !indices.isEmpty();
    int count;
    if (indexBased)
        count = indices.size();
    else
        count = vertices.vertexCount();

    int positionField = vertices.fieldIndex(QGL::Position);
    int texCoordField = vertices.fieldIndex(QGL::TextureCoord0);
    if (positionField == -1) {
        // If there are no positions, then subdividing is pointless.
        return;
    }

    for (int posn = 0; (posn + 15) < count; posn += 16) {
        // Construct a QGLBezierPatch object from the next high-level patch.
        QGLBezierPatch patch;
        int vertex;
        if (indexBased) {
            for (vertex = 0; vertex < 16; ++vertex) {
                patch.points[vertex] = vertices.vector3DAt
                    (indices[posn + vertex], positionField);
            }
        } else {
            for (int vertex = 0; vertex < 16; ++vertex) {
                patch.points[vertex] = vertices.vector3DAt
                    (posn + vertex, positionField);
            }
        }
        QVector2D tex1, tex2;
        if (texCoordField != -1) {
            if (indexBased) {
                vertex = indices[posn + cornerOffsets[0]];
                tex1 = vertices.vector2DAt(vertex, texCoordField);
                vertex = indices[posn + cornerOffsets[3]];
                tex2 = vertices.vector2DAt(vertex, texCoordField);
            } else {
                vertex = posn + cornerOffsets[0];
                tex1 = vertices.vector2DAt(vertex, texCoordField);
                vertex = posn + cornerOffsets[3];
                tex2 = vertices.vector2DAt(vertex, texCoordField);
            }
        } else {
            tex1 = QVector2D(0.0f, 0.0f);
            tex2 = QVector2D(1.0f, 1.0f);
        }
        qreal xtex = tex1.x();
        qreal ytex = tex1.y();
        qreal wtex = tex2.x() - xtex;
        qreal htex = tex2.y() - ytex;
        for (int corner = 0; corner < 4; ++corner) {
            if (indexBased)
                vertex = indices[posn + cornerOffsets[corner]];
            else
                vertex = posn + cornerOffsets[corner];
            QVector3D n; // = normals.value(vertex, QVector3D());
            //if (n.isNull())
                n = patch.normal(cornerS[corner], cornerT[corner]);
            //else
                //n = n.normalized();
            patch.indices[corner] = newVertices.vertexCount();
            newVertices.append(patch.points[cornerOffsets[corner]]);
            newVertices.append(n);
            newVertices.append(xtex + wtex * cornerS[corner],
                               ytex + htex * cornerT[corner]);
        }

        // Subdivide the patch and generate the final triangles.
        patch.recursiveSubDivide(newVertices, newIndices,
                                 depth, xtex, ytex, wtex, htex,
                                 compactSubdivision);
    }

    mesh.setVertexArray(newVertices);
    mesh.setIndexArray(newIndices);
}

/*!
    Constructs a new Bezier geometry object and attaches it to \a parent.
*/
QGLBezierGeometry::QGLBezierGeometry(QObject *parent)
    : QGLGeometry(parent)
{
    d = new QGLBezierGeometryPrivate();
}

/*!
    Destroys this Bezier geometry object.
*/
QGLBezierGeometry::~QGLBezierGeometry()
{
    delete d;
}

/*!
    Returns the explicit normal associated with the vertex at \a index
    in vertexArray().  Returns a null QVector3D value if there is
    no explicit normal for \a index.

    \sa setNormal()
*/
QVector3D QGLBezierGeometry::normal(int index) const
{
    return d->normals.value(index, QVector3D());
}

/*!
    Sets the explicit normal associated with the vertex at \a index
    in vertexArray() to \a value.  If \a value is null, then the
    explicit normal will be removed.

    \sa normal()
*/
void QGLBezierGeometry::setNormal(int index, const QVector3D& value)
{
    if (value.isNull())
        d->normals.remove(index);
    else
        d->normals[index] = value;
}

/*!
    \reimp
*/
void QGLBezierGeometry::draw(QGLPainter *painter)
{
    if (isModified() || !d->subdivided) {
        d->triangleMesh.setBufferThreshold(bufferThreshold());
        d->subdivide(d->triangleMesh, vertexArray(), indexArray());
    }
    d->triangleMesh.draw(painter);
}

/*!
    \reimp
*/
bool QGLBezierGeometry::upload()
{
    if (isModified() || !d->subdivided) {
        d->triangleMesh.setBufferThreshold(bufferThreshold());
        d->subdivide(d->triangleMesh, vertexArray(), indexArray());
    }
    return d->triangleMesh.upload();
}

/*!
    Returns the depth of subdivision to use when converting the
    Bezier geometry into triangles.  The default value is 4.

    \sa setSubdivisionDepth(), subdivide()
*/
int QGLBezierGeometry::subdivisionDepth() const
{
    return d->depth;
}

/*!
    Sets the depth of subdivision to use when converting the
    Bezier geometry into triangles to \a value.

    \sa subdivisionDepth(), subdivide()
*/
void QGLBezierGeometry::setSubdivisionDepth(int value)
{
    d->depth = value;
}

/*!
    Returns true if the subdivision algorithm will generate a more
    compact set of triangles; false otherwise.

    The default value is true, which indicates that the patches
    will be decomposed into two triangles at the lowest level
    of subdivision.  A value of false indicates that the patches
    will be decomposed into four triangles at the lowest level
    of subdivision, which may give better visual results at
    the cost of extra memory.

    \sa setCompactSubdivision()
*/
bool QGLBezierGeometry::compactSubdivision() const
{
    return d->compactSubdivision;
}

/*!
    Sets the compact property of the subdivision algorithm to \a value.

    If \a value is true, then the patches will be decomposed into two
    triangles at the lowest level of subdivision.  A \a value of false
    indicates that the patches will be decomposed into four triangles
    at the lowest level of subdivision, which may give better visual
    results at the cost of extra memory.

    \sa compactSubdivision()
*/
void QGLBezierGeometry::setCompactSubdivision(bool value)
{
    d->compactSubdivision = value;
}

/*!
    Subdivides this Bezier geometry object into a set of flat
    triangles and returns the QGLGeometry object containing the
    triangles.  It is the caller's responsibility to destroy
    the returned object when it is no longer required.

    \sa subdivisionDepth()
*/
QGLGeometry *QGLBezierGeometry::subdivide() const
{
    QGLGeometry *geom = new QGLGeometry();
    geom->setDrawingMode(QGL::Triangles);
    geom->setBufferThreshold(bufferThreshold());
    d->subdivide(*geom, vertexArray(), indexArray());
    return geom;
}

QT_END_NAMESPACE
