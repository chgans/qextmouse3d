/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qgl3dsmesh.h"

#include <lib3ds/mesh.h>
#include <lib3ds/material.h>
#include <lib3ds/types.h>
#include <lib3ds/vector.h>

#include "qglpainter.h"
#include "qglvertexarray.h"
#include "qglindexarray.h"
#include "qglgeometry.h"
#include "qglmaterialcollection.h"
#include "qglscenenode.h"

#include <QtCore/qvector.h>

/*!
    \class QGL3dsMesh
    \since 4.6
    \internal
    The QGL3dsMesh class models a mesh object in a Lib3ds scene.

    It holds the data which has been retrieved from a Lib3dsMesh structure
    contained in a Lib3dsFile, and includes methods to draw the mesh onto
    a QGLPainter.
*/

/*!
    \internal
    If a faceted model (one with no smoothing groups) has more than this
    many faces, then smoothing will be forced on.  Smooth geometry is more
    efficient to display since normals can be stored in line with vertices.
    With more than this many faces smooth rendering should produce a more
    desirable result also.
*/
#define FACETED_THRESHOLD 1000

/*!
    \internal
    Construct a new QGL3dsMesh object, based on the Lib3dsMesh \a mesh and
    parented onto \a parent.
*/
QGL3dsMesh::QGL3dsMesh(Lib3dsMesh *mesh, QObject *parent)
    : QGLGeometry(parent)
    , mMesh(mesh)
    , mSmoothingGroups(0)
    , mHasTexture(false)
    , mTexFlip(false)
    , mDebugged(false)
{
    setDrawingMode(QGL::Triangles);
}

/*!
    Destroy this mesh recovering any resources.
*/
QGL3dsMesh::~QGL3dsMesh()
{
}

/*!
    \internal
    Sets up this mesh by determining the materials and smoothing, then
    generating the structures to hold the mesh data.  In the simple case
    with one or zero materials the mesh is a single object.  Sub-meshes
    are used for each material in multiple-material meshes.
*/
void QGL3dsMesh::initialize()
{
    QList<int> matList = determineMaterials();
    QString baseName = objectName();
    QGLMaterialCollection *pal = palette();
    QGL3dsMesh *m = this;
    while (true)
    {
        Q_ASSERT(matList.count() > 0);
        int matIx = matList.takeLast();
        m->setMaterial(matIx);
#ifndef QT_NO_DEBUG_STREAM
        if (matIx == -1)
            m->setObjectName(baseName + QString("XX_No_Material_XX"));
        else
            m->setObjectName(baseName + pal->materialName(matIx));
#endif
        m->setPalette(pal);
        m->generateStructure();
        pal->markMaterialAsUsed(matIx);
        if (matList.count() == 0)
            break;
        m = new QGL3dsMesh(mMesh, this);
    }
}

/*!
    \internal
    Returns a list of the material indexes used in this mesh.  If any
    face has no material assigned then a -1 index will be listed.
*/
QList<int> QGL3dsMesh::determineMaterials() const
{
    QList<int> materialIndexes;
    Lib3dsFace *face;
    for (Lib3dsDword f = 0; f < mMesh->faces; ++f)
    {
        face = &mMesh->faceL[f];
        int matIx = palette()->materialIndexByName(face->material);
#ifndef QT_NO_DEBUG_STREAM
        if (matIx == -1 && strlen(face->material) != 0)
            qDebug("Bad .3ds file: no material %s! (Referenced in mesh %s)\n",
                     face->material, mMesh->name);
#endif
        if (!materialIndexes.contains(matIx))
            materialIndexes.append(matIx);
    }
    return materialIndexes;
}

/*!
    \internal
    Calculates the smoothing group keys for this mesh, or if no smoothing is
    set returns a zero keys resulting in faceted rendering.

    If a material is set for this mesh, then only the faces matching this
    material are considered.  If no material is set for this mesh, then only
    faces without a material are considered.

    If the mesh is faceted (zero smoothing keys), but contains more than a
    threshold number of faces (currently set at 1000) then smoothing will be
    forced on.
*/
void QGL3dsMesh::determineSmoothing()
{
    Lib3dsFace *face;
    Lib3dsDword keys = 0;
    for (Lib3dsDword f = 0; f < mMesh->faces; ++f)
    {
        face = &mMesh->faceL[f];
        if (palette()->materialIndexByName(face->material) != material())
            continue;
        if (face->smoothing)
            keys |= face->smoothing;
    }
    if (keys == 0 && mMesh->faces > FACETED_THRESHOLD)
    {
#ifndef QT_NO_DEBUG_STREAM
        qDebug("Mesh %s has %d faces (threshold is %d): forcing smooth render",
               mMesh->name, mMesh->faces, FACETED_THRESHOLD);
#endif
        for (Lib3dsDword f = 0; f < mMesh->faces; ++f)
        {
            face = &mMesh->faceL[f];
            if (palette()->materialIndexByName(face->material) != material())
                continue;
            face->smoothing = 1;
        }
        keys = 1;
    }
    mSmoothingGroups = keys;
}

/*!
    Generates the structures of this mesh including index and vertex
    arrays, along with lighting normals.  The structure may be either a
    faceted mesh (with no smoothing) or a smooth mesh (which may have a
    number of smoothing groups).  The faceted mesh is optimised for cases
    where a sequence of faces share the same normal, and suits smaller meshes.
    The smooth mesh stores all data in a single large vertex array combining
    one normal for each vertex.
*/
void QGL3dsMesh::generateStructure()
{
    mMatrix = meshMatrix();
    checkTextures();
    determineSmoothing();
    if (mSmoothingGroups)
        generateVertices();
    else
        generateFaces();
}

/*!
    Check the meshes textures and update the record of whether this mesh
    has textures or not.  In debug mode issue a warning if the textures are
    corrupt (number of texels and vertices not equal).
*/
void QGL3dsMesh::checkTextures()
{
    QGLTexture2D *tex = palette()->texture(material());
    if (tex)
    {
        mHasTexture = true;
#ifndef QT_NO_DEBUG_STREAM
        if (mMesh->points != mMesh->texels)
            qWarning("Mesh %s has unequal number of texels (%d) and vertices (%d)",
                     mMesh->name, mMesh->texels, mMesh->points);
#endif
        mTexFlip = tex->flipVertically();
    }
}

/*!
    Helper method to accumulate the vertex at the given \a meshIndex into the
    \a vertices.  Also updates the \a boundingBox by expanding it
    to include the new vertex.  If the \a normal vector is not null then it
    will be appended to the vertices.  Texture coordinates are added from the
    current mesh if required.
*/
void QGL3dsMesh::addVertex(QGLVertexArray &vertices, QBox3D &boundingBox,
                           Lib3dsDword meshIndex, const QVector3D& normal)
{
    Lib3dsVector &v = mMesh->pointL[meshIndex].pos;
    QVector3D vec(v[0], v[1], v[2]);
    QVector3D n = normal;
    if (!mMatrix.isIdentity())
    {
        vec = mMatrix * vec;
        if (!n.isNull())
        {
            // Since the normal is a vector (not a point) it is not modified by
            // an affine transform matrix where the last column contains values
            // eg by a translate - but scales and rotates will effect it.
            // Acheive this by multiplying by the top-left 3x3 sub-matrix.
            n = QVector3D(
                    n.x() * mMatrix(0, 0) + n.y() * mMatrix(0, 1) + n.z() * mMatrix(0, 2),
                    n.x() * mMatrix(1, 0) + n.y() * mMatrix(1, 1) + n.z() * mMatrix(1, 2),
                    n.x() * mMatrix(2, 0) + n.y() * mMatrix(2, 1) + n.z() * mMatrix(2, 2));
            if (!(n == normal)) // matrix did something, ie was a rotation or similar
                n.normalize();
        }
    }
    Q_ASSERT(n.isNull() || vertices.fieldIndex(QGL::Normal) != -1);
    vertices.append(vec);
    if (!n.isNull())
        vertices.append(n);
    boundingBox.expand(vec);
    if (mHasTexture)
    {
        Q_ASSERT(vertices.fieldIndex(QGL::TextureCoord0) != -1);
        if (meshIndex < mMesh->texels)
        {
            Lib3dsTexel &tx = mMesh->texelL[meshIndex];
            vertices.append(QVector2D(tx[0], mTexFlip ? 1.0f - tx[1] : tx[1]));
        }
        else
        {
            // pad dodgy tex map with nulls, checkTextures() has already warned
            vertices.append(QVector2D());
        }
    }
}

/*!
    \internal
    Conditionally adds the face specified by index \a f to the geometry
    in \a verts and \a indices.  It is added if
    \list
    \i the material of the faces matches the geometry's material AND
    \i the smoothing value \a key either
        \list
        \i matches the smoothing key of the face OR
        \i is the special key 1, and the face has no smoothing key
        \endlist
    \endlist
    If it is added the \a indexMap and \a lastVertex is used to track the
    re-mapped index values of the vertices.
*/
void QGL3dsMesh::processFace(int f, Lib3dsDword key,
                             QMap<int, int> &indexMap,
                             QGLVertexArray &verts,
                             QGLIndexArray &indices,
                             QList<int> &counts,
                             QBox3D &bb,
                             int &lastVertex)
{
    Lib3dsFace *face = &mMesh->faceL[f];
    if (palette()->materialIndexByName(face->material) == material() &&
        ((key & face->smoothing) || (key == 1 && face->smoothing == 0)))
    {
        for (int j = 0; j < 3; ++j)
        {
            int ix = face->points[j];
            QVector3D norm = normalFromFace(face);
            if (indexMap.contains(ix))
            {
                ix = indexMap[ix];
                QVector3D exNorm = verts.vector3DAt(ix, QGL::Normal);
                verts.setAt(ix, QGL::Normal, exNorm + norm);
                counts[ix] += 1;
            }
            else
            {
                addVertex(verts, bb, ix, norm);
                indexMap.insert(ix, lastVertex);
                ix = lastVertex++;
                counts.append(1);
            }
            indices.append(ix);
        }
    }
    else
    {
        //qDebug() << "rejected face" << f << "key" << key << "material" << face->material << "face key" << face->smoothing;
    }
}

/*!
    \internal
    Generate the vertices for the faces with the given \a smoothingKeys.
    This method is optimized for the case of a smooth mesh with continuingly
    varied normals over the face of the mesh.  Normals are stored per vertex.
    Creates a set of vertices and lighting normals.  Updates the bounding box
    for the mesh.  Only considers vertices which are in faces that have a
    material matching the material() of this mesh.
*/
void QGL3dsMesh::generateVertices()
{
    Q_ASSERT(mSmoothingGroups);
    QBox3D bb;
    QMap<int, int> indexMap;
    int lastVertex = 0;
    QList<int> counts;
    QGLIndexArray indices;
    QGLVertexArray verts(QGL::Position, 3, QGL::Normal, 3);
    if (mHasTexture)
        verts.addField(QGL::TextureCoord0, 2);
    for (Lib3dsDword key = 1; key; key <<= 1)
    {
        if (key & mSmoothingGroups || key == 1)
        {
            for (Lib3dsDword f = 0; f < mMesh->faces; ++f)
                processFace(f, key, indexMap, verts, indices, counts, bb, lastVertex);
            indexMap.clear();
        }
    }
    if (verts.vertexCount())
    {
        renormalize(verts, counts);
        setBoundingBox(bb);
        setVertexArray(verts);
        setIndexArray(indices);
    }
#if 0
    if ( mMesh->faces != (indices.size() / 3))
        qDebug() << "generateVertices(" << this << ")"
                << "- mesh points:" << mMesh->points
                << ";    mesh faces:" << mMesh->faces
                << ";    verts size:" << verts.vertexCount()
                << ";    face count:" << (indices.size() / 3);
#endif
}

/*!
    \internal
    Compares two vectors for approximate equality.  This level of equality
    is good enough for lighting calculations.
*/
static inline bool qVectorCompare(const QVector3D &a, const QVector3D &b)
{
    if (qAbs((float)a.x() - (float)b.x()) > 0.00001f)
        return false;
    if (qAbs((float)a.y() - (float)b.y()) > 0.00001f)
        return false;
    if (qAbs((float)a.z() - (float)b.z()) > 0.00001f)
        return false;
    return true;
}

/*!
    \internal
    Returns a mapping of normals to faces.  The keys are indexes into
    this objects list of normal vectors, and the values are indexes into
    the lib3ds mesh faces.  Considers all faces which match this meshes
    material().
*/
QMap<int, Lib3dsDword> QGL3dsMesh::mapFaces()
{
    QMap<int, Lib3dsDword> faceMap;
    int cnt = 0;
    for (Lib3dsDword i = 0; i < mMesh->faces; ++i)
    {
        Lib3dsFace *face = &mMesh->faceL[i];
        // only process flat faces
        if (face->smoothing)
            continue;
        if (palette()->materialIndexByName(face->material) != material())
            continue;
        int n = 0;
        QVector3D norm = normalFromFace(face);
        cnt++;
        // don't store same normal many times
        for ( ; n < mNormals.count(); ++n)
            if (qVectorCompare(mNormals[n], norm))
                break;
        if (n == mNormals.count())
            mNormals.append(norm);
        faceMap.insertMulti(n, i);
    }
    return faceMap;
}

/*!
    \internal
    Generate geometry with normals set per each face, or runs of faces.
    This gives a faceted appearance in the case of zero smoothing.
    In cases where large non-smoothed planes are broken into many faces
    this method is very efficient on the graphics hardware.
*/
void QGL3dsMesh::generateFaces()
{
    Lib3dsFace *face;
    QGLIndexArray indices;
    QGLVertexArray verts(QGL::Position, 3);
    if (mHasTexture)
        verts.addField(QGL::TextureCoord0, 2);
    QMap<int, int> indexMap;
    int lastVertex = 0;
    QMap<int, Lib3dsDword> faceMap = mapFaces();
    QBox3D bb;
    // basically an RLE compression of normals
    NormalMap entry;
    entry.normal = 0;  // first one in map will be number 0
    entry.start = 0;   // start at first
    int count = 0;
    QMap<int, Lib3dsDword>::iterator it(faceMap.begin());
    for ( ; it != faceMap.end(); ++it)
    {
        if (it.key() != entry.normal)
        {
            entry.count = count;   // update the count and add it
            mNormalMappings.append(entry);
            entry.start = entry.start + count;
            count = 0;
            entry.normal = it.key();
        }
        face = &mMesh->faceL[it.value()];
        for (int i = 0; i < 3; ++i)
        {
            int ix = face->points[i];
            if (indexMap.contains(ix))
            {
                ix = indexMap[ix];
            }
            else
            {
                addVertex(verts, bb, ix);
                indexMap.insert(ix, lastVertex);
                ix = lastVertex++;
            }
            indices.append(ix);
            ++count;
        }
    }
    entry.count = count;   // update count and add last entry
    if (count > 0)
    {
        mNormalMappings.append(entry);
        setVertexArray(verts);
        setIndexArray(indices);
    }
#if 0
    qDebug() << "generateFaces(" << this << ")"
            << "- mesh points:" << mMesh->points
            << ";   mesh faces:" << mMesh->faces
            << ";   verts size:" << verts.vertexCount()
            << ";   face count:" << (indices.size() / 3);
#endif
}

/*!
    \internal
    Returns any local transformation matrix for the mesh.
*/
QMatrix4x4 QGL3dsMesh::meshMatrix() const
{
    Lib3dsMatrix &m = mMesh->matrix;  // typedef for float[4][4]
    QMatrix4x4 mat;
    for (int col = 0; col < 4; ++col)
        for (int row = 0; row < 4; ++row) {
            float e = m[col][row];
            if (qFuzzyIsNull(e))
                mat(row, col) = 0.0f;
            else
                mat(row, col) = e;
        }
    mat.optimize();  // setup to use optimizations
    if (mat.isIdentity())
        return mat;
    // The reverse transform is what we apply to model-view in order
    // to draw the underlying geometry
    bool invertible = true;
    mat = mat.inverted(&invertible);
    if (invertible)
         return mat;
#ifndef QT_NO_DEBUG_STREAM
    qWarning("Could not invert matrix for mesh %s", mMesh->name);
    qDebug() << mat;
#endif
    return QMatrix4x4();
}

/*!
    \internal
    Returns a normal to the given Lib3dFace \a face.  Accesses the Lib3dsMesh
    for this object.  If the face has a normal defined then that is returned.
    If the normal has to be calculated then counter-clockwise winding is
    assumed and the returned normal is normalized to unit length.  Any normal
    defined for the face is used as is.
*/
QVector3D QGL3dsMesh::normalFromFace(Lib3dsFace *face) const
{
    Lib3dsVector &vec = face->normal;
    if (qFuzzyIsNull(vec[0]) && qFuzzyIsNull(vec[1]) && qFuzzyIsNull(vec[2]))
    {
        Lib3dsVector &va = mMesh->pointL[face->points[0]].pos;
        Lib3dsVector &vb = mMesh->pointL[face->points[1]].pos;
        Lib3dsVector &vc = mMesh->pointL[face->points[2]].pos;
        QVector3D a(va[0], va[1], va[2]);
        QVector3D b(vb[0], vb[1], vb[2]);
        QVector3D c(vc[0], vc[1], vc[2]);
        return QVector3D::normal(a, b, c);
    }
    else
    {
        float x = qFuzzyIsNull(vec[0]) ? 0.0f : vec[0];
        float y = qFuzzyIsNull(vec[1]) ? 0.0f : vec[1];
        float z = qFuzzyIsNull(vec[2]) ? 0.0f : vec[2];
        return QVector3D(x, y, z);
    }
}

/*!
    \internal
    Rescales a collection of normals stored in the QGL::Normal index of
    \a vertices by dividing each normal by \a counts of how many normals
    were accumulated at that vertex.  Note that this gives the average of
    the normals, which is the desired result.  If the model specifies
    non-standard normals for some special effect, then simply normalizing
    (dividing by the total length) will result in a standard unit-length
    normal which may not be what the author of the model desired.
*/
void QGL3dsMesh::renormalize(QGLVertexArray &vertices, const QList<int> &counts)
{
    for (int i = 0; i < vertices.vertexCount(); ++i)
    {
        int n = vertices.fieldIndex(QGL::Normal);
        Q_ASSERT(n != -1);
        QVector3D norm = vertices.vector3DAt(i, n);
        norm = norm / qreal(counts[i]);
        vertices.setAt(i, n, norm);
    }
}

/*!
    \internal
    \reimp
*/
void QGL3dsMesh::draw(QGLPainter *painter)
{
    QObjectList subMeshes = children();
    QObjectList::iterator it(subMeshes.begin());
    for ( ; it != subMeshes.end(); ++it)
    {
        QGL3dsMesh *m = qobject_cast<QGL3dsMesh *>(*it);
        if (m)
            m->draw(painter);
    }
    if (mNormals.count())
    {
#if 0
        if (!mDebugged)
            qDebug() << "QGL3dsMesh::draw( FLAT " << this << ") - normals:" << mNormals.count()
            << ";    map entries:" << mNormalMappings.count()
            << ";    faces:" << (indexArray().size() / 3)
            << ";    verts:" << vertexArray().vertexCount();
#endif
        QList<NormalMap>::iterator it(mNormalMappings.begin());
        for ( ; it != mNormalMappings.end(); ++it)
        {
            painter->setCommonNormal(mNormals[it->normal]);
            QGLGeometry::draw(painter, it->start, it->count);
        }
    }
    else
    {
#if 0
        if (!mDebugged)
            qDebug() << "QGL3dsMesh::draw( SMOOTH " << this << ") - normals:" << mNormals.count()
            << ";    map entries:" << mNormalMappings.count()
            << ";    faces:" << (indexArray().size() / 3)
            << ";    verts:" << vertexArray().vertexCount();
#endif
        QGLGeometry::draw(painter);
    }
    mDebugged = true;
}

/*!
    \internal
    \reimp
*/
bool QGL3dsMesh::upload()
{
    QObjectList subMeshes = children();
    QObjectList::iterator it(subMeshes.begin());
    for ( ; it != subMeshes.end(); ++it)
    {
        QGL3dsMesh *m = qobject_cast<QGL3dsMesh *>(*it);
        if (!m->upload())
            return false;
    }
    return QGLGeometry::upload();
}
