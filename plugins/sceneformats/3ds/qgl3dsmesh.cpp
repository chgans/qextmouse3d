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

#include "qgl3dsmesh.h"
#include "qglmaterialcollection.h"
#include "qgloperation.h"
#include "qglsection_p.h"
#include "qarray.h"

#include <lib3ds/mesh.h>
#include <lib3ds/material.h>
#include <lib3ds/vector.h>
#include <lib3ds/types.h>

#include <QtGui/qmatrix4x4.h>

// Faceted meshes look terrible when they have more than a small number
// of faces.  Usually if large meshes are faceted its some kind of error
// in the model, or in importing of the model by lib3ds.  Force on
// smoothing when greater than this many faces are detected.
#define FACETED_THRESHOLD 1000



// Modulate smoothed vertices to prevent attempts to smooth acute angles
// comment this out to disable
#define DO_MODULATE 1

// Two faces that have an angle between their plane vectors with a cosine
// less than this are judged to form a sharp (acute) angle.
// -ve cosine (less than 0.0f) means 90 degrees or sharper like the sides
// of a rectangular prism so this is a good value.
#define ACUTE -0.0001f



// Correct flipped normals where possible - comment this out to disable
#define DO_NORMAL_CORRECT 1

// Two vectors that have an angle between them with a cosine less than this
// value are judged to be approximately the inverse of each other, for the
// purposes of determining whether a normal has been inverted or not
#define INVERSE -0.20f

// Only try to correct normals if this many neighbour faces contribute to
// the assessment.  2 or 3 are good values.  4 or greater is the same as
// defining DO_NORMAL_CORRECT false
#define AVG_CNT 2



// make a QVector3D from a Lib3dsVector - by a hard cast.  Its required
// that QVector3D behave like a POD type - float[3] - for all of the
// OpenGL stuff
static inline QVector3D &l2v(Lib3dsVector &vec)
{
    return reinterpret_cast<QVector3D&>(vec);
}

// is this a null 3ds vector - same as QVector3D::isNull()
static inline bool qIsNull(const Lib3dsVector &vec)
{
    return (qIsNull(vec[0]) && qIsNull(vec[1]) && qIsNull(vec[2]));
}

// find the normalized plane vector, that is a unit vector perpendicular
// to the plane of the face.  this is the same thing as the default normal
static inline void planeVec(Lib3dsFace *face, Lib3dsPoint *pointList, Lib3dsVector result)
{
    Lib3dsVector &l3a = pointList[face->points[0]].pos;
    Lib3dsVector &l3b = pointList[face->points[1]].pos;
    Lib3dsVector &l3c = pointList[face->points[2]].pos;
    lib3ds_vector_normal(result, l3a, l3b, l3c);
}

struct AdjListNode
{
    Lib3dsFace *face;
    AdjListNode *next;
};

struct AdjListHead
{
    Lib3dsFace *face;
    AdjListNode *next;
    Lib3dsVector planeVector;
};



QGL3dsMesh::QGL3dsMesh(Lib3dsMesh *mesh, QObject *parent,
                       QGLMaterialCollection *materials)
    : QGLDisplayList(parent, materials)
    , m_mesh(mesh)
    , m_texFlip(false)
    , m_hasZeroSmoothing(false)
    , m_faceMap(0)
{
    setObjectName(QString(mesh->name));
}

QGL3dsMesh::~QGL3dsMesh()
{
    delete[] m_faceMap;
}

void QGL3dsMesh::processNodeForMaterial(int matIx, QGLSceneNode *node)
{
    QString baseName = objectName();
    node->setMaterial(matIx);
    node->setObjectName(baseName + QLatin1String("::") +
                        ((matIx == -1)
                         ? QString("No_Material")
                             : palette()->materialName(matIx)));
    checkTextures(matIx);
    generateVertices();
    palette()->markMaterialAsUsed(matIx);
}

void QGL3dsMesh::initialize()
{
    modulateMesh();
    analyzeMesh();
    if (m_smoothingGroups == 0 && m_mesh->faces > FACETED_THRESHOLD)
    {
#ifndef QT_NO_DEBUG_STREAM
        qDebug("Mesh %s has %d faces (threshold is %d): forcing smooth render",
               m_mesh->name, m_mesh->faces, FACETED_THRESHOLD);
#endif
        m_hasZeroSmoothing = false;
        for (Lib3dsDword f = 0; f < m_mesh->faces; ++f)
            m_mesh->faceL[f].smoothing = 1;
        analyzeMesh();
    }
    bool mixedTexturedAndPlain = m_plainMaterials.count() > 0 &&
                                 m_textureMaterials.count() > 0;

    setLocalTransform(meshMatrix());

    // start a new section and node
    newSection(m_smoothingGroups ? QGL::Smooth : QGL::Faceted);
    QGLSceneNode *node = currentNode();

    // process all the plain materials first, then textured to avoid effect swapping
    QList<int> matList = m_plainMaterials.toList();
    if (mixedTexturedAndPlain)
    {
        node->setEffect(QGL::LitMaterial);
        node->setObjectName(objectName() + "::Materials");
        pushNode();
    }
    else
    {
        setEffect(m_textureMaterials.count() > 0 ? QGL::LitModulateTexture2D : QGL::LitMaterial);
    }
    while (matList.count() > 0)
    {
        int matIx = matList.takeFirst();
        processNodeForMaterial(matIx, node);
        if (matList.count() > 0)
            node = newNode();
    }
    matList = m_textureMaterials.toList();
    if (mixedTexturedAndPlain)
    {
        popNode();
        newNode();
        node->setEffect(QGL::LitModulateTexture2D);
        node->setObjectName(objectName() + "::Textures");
    }
    while (matList.count() > 0)
    {
        int matIx = matList.takeFirst();
        processNodeForMaterial(matIx, node);
        if (matList.count() > 0)
            node = newNode();
    }
    finalize();
}

// Build a linked list.  Use a QArray: the first N*2 entries correspond
// to the N vertices: n*2 is the face number, n*2+1 is the index of
// the next entry for that vertex, or -1 if there is no next entry.
// While on the job find the smoothing keys.
QArray<int> QGL3dsMesh::mapFacesToVerts(Lib3dsDword *allKeys)
{
    Lib3dsFace *face;
    QArray<int> vlist(m_mesh->faces * 2, -1);
    int nx = 2 * m_mesh->faces;
    m_smoothingGroupCount = 0;
    for (Lib3dsDword f = 0; f < m_mesh->faces; ++f)
    {
        face = &m_mesh->faceL[f];
        // saveData[f] = face->user;
        face->user.p = 0;
        if (face->smoothing)
        {
            if (!(face->smoothing & *allKeys))
            {
                *allKeys |= face->smoothing;
                m_smoothingGroupCount += 1;
            }
        }
        for (int i = 0; i < 3; ++i)
        {
            int v = face->points[i];
            int ptr = v*2;
            int prv = -1;
            if (vlist[ptr] != -1)
            {
                prv = ptr;
                while (vlist[ptr+1] != -1)
                {
                    ptr = vlist[ptr+1];
                    prv = ptr;
                }
                ptr = nx;
                if (vlist.size() <= ptr+1)
                    vlist.extend(ptr+1);
                nx += 2;
            }
            vlist[ptr] = f;
            vlist[ptr+1] = -1;
            if (prv != -1)
                vlist[prv+1] = ptr;
        }
    }
    return vlist;
}

void QGL3dsMesh::addToAdjacencyMap(Lib3dsFace *face, int *mptr, int *hptr, Lib3dsFace *neighbour)
{
    AdjListNode *n;
    if (face->user.p == 0)
    {
        AdjListHead *h = &m_faceMapHeads[*hptr++];
        face->user.p = h;
        planeVec(face, m_mesh->pointL, h->planeVector);
        n = reinterpret_cast<AdjListNode*>(h);
    }
    else
    {
        n = reinterpret_cast<AdjListNode*>(face->user.p);
        while (n->next != 0)
            n = n->next;
        n->next = &m_faceMap[*mptr++];
    }
    n->face = neighbour;
}

// add to each face a linked list of its <= 3 neighbouring faces
// and also ensure each face knows is own normal.  Rather than doing
// lots of small allocs, just assume every face has 3 neighbours
// and allocate the list nodes in one big lump - the m_faceMap.
// use the user data pointer in the lib3ds face struct to point
// to the first node in the list.
void QGL3dsMesh::buildAdjacencyMap(const QArray<int> &vlist)
{
    Lib3dsFace *face;
    Lib3dsFace *nbr;
    int nxm = 0;
    int nxh = 0;
    m_faceMap = new AdjListNode[m_mesh->faces * 2];
    m_faceMapHeads = new AdjListHead[m_mesh->faces];
    qMemSet(m_faceMap, 0, sizeof(m_faceMap));
    qDebug() << vlist;
    return;
    for (Lib3dsDword f = 0; f < m_mesh->faces; ++f)
    {
        // for each edge (where an edge is a pair of verts) find ones
        // adjacent, ie both verts in edge have same face (which is
        // not this face).
        face = &m_mesh->faceL[f];
        if (qIsNull(face->normal))
            planeVec(face, m_mesh->pointL, face->normal);
        for (int i = 0; i < 3; ++i)
        {
            int vi = face->points[i];
            int neighbouri = -1;
            while (true)
            {
                neighbouri = vlist[vi];
                if (neighbouri != (int)f)
                {
                    int vj = face->points[i + 1 % 3];
                    int neighbourj = -1;
                    while (true)
                    {
                        neighbourj = vlist[vj];
                        if (neighbourj == neighbouri)
                        {
                            nbr = &m_mesh->faceL[neighbouri];
                            if (nbr->smoothing & face->smoothing)
                                addToAdjacencyMap(face, &nxm, &nxh, nbr);
                            break;
                        }
                        if (vlist[vj+1] == -1)
                            break;
                        else
                            vj = vlist[vj+1];
                    }
                }
                if (vlist[vi+1] == -1)
                    break;
                else
                    vi = vlist[vi+1];
            }
        }
    }
}

static inline Lib3dsDword nextUnusedKey(Lib3dsDword keys, Lib3dsDword key)
{
    if (key != 0)
        ++key;
    else
        key = 1;
    while (key & keys)
        key <<= 1;
    return key;
}

void QGL3dsMesh::modulateMesh()
{
#if !defined(DO_MODULATE) && !defined(DO_NORMAL_CORRECT)
    return;
#endif
    // If model turns up that relies on the user data, will need to save and
    // reinstantiate it.  If that happens uncomment this, and the others lines below.
    /*
    Lib3dsUserData saveData[] = new Lib3dsUserData[m_mesh->faces];
    for (Lib3dsDword f = 0; f < m_mesh->faces; ++f)
    {
        face = &m_mesh->faceL[f];
        saveData[f] = face->user;
    }
    */
    Lib3dsFace *face;
    Lib3dsDword allKeys = 0;
    QArray<int> vlist = mapFacesToVerts(&allKeys);
    buildAdjacencyMap(vlist);

    // Go thru each smoothing group - we don't care about the zero group since
    // they're already faceted.  In each group process all connected faces
    // starting the queue off with a seed face (the first face in the group).
    // Set the alternate group to be the first unused smoothing group key.
    //
    // To process a face: add the face to the processed set; for each adjacent
    // face if its in this same smoothing group, add it to the queue; if the
    // adjacent face is at an acute angle to this face, move it to the
    // alternate group.  Note that the alternate groups are not further iterated
    // since they are not in "each smoothing group" (the allKeys variable).
    //
    // When the queue is empty, go to the next smoothing group in the mesh.

    // Normal repair mode:  if a face normal is flipped in error, ie a mistake
    // in model building (typically points specified in the wrong order)
    // this can be detected and repaired.  The normal is flipped if the inverse
    // of the normal is at an acute angle to the average of neighbour normals.
    // If detected the normals is corrected and the winding is also checked to
    // see if it needs to be corrected also.
    QSet<Lib3dsFace*> processed;
    bool keyUsed = true;
    Lib3dsDword altKey = 0;
    for (Lib3dsDword key = 1; key; key <<= 1)
    {
        if (!(key & allKeys))
            continue;
        if (keyUsed)
            altKey = nextUnusedKey(allKeys, altKey);
        keyUsed = false;
        QArray<Lib3dsFace*> queue;
        for (Lib3dsDword f = 0; f < m_mesh->faces; ++f)
        {
            face = &m_mesh->faceL[f];
            if (face->smoothing)
            {
                queue.append(face);
                break;
            }
        }
        int head = 0;
        while (head < queue.size())
        {
            face = queue.at(head++);
            if (processed.contains(face))
                continue;
            processed.insert(face);
            AdjListHead *header = reinterpret_cast<AdjListHead*>(face->user.p);
            AdjListNode *neighbours = reinterpret_cast<AdjListNode*>(face->user.p);
            AdjListNode *n;
            for (n = neighbours; n; n = n->next)
                if ((n->face->smoothing & face->smoothing) && !processed.contains(n->face))
                    queue.append(n->face);
#ifdef DO_NORMAL_CORRECT
            int acnt = 0;
            Lib3dsVector avgn = { 0 };
            for (n = neighbours; n; n = n->next)
            {
                if (n->face->smoothing & face->smoothing)
                {
                    lib3ds_vector_add(avgn, avgn, n->face->normal);
                    ++acnt;
                }
            }
            if (acnt >= AVG_CNT)
            {
                lib3ds_vector_normalize(avgn);
                float na = lib3ds_vector_dot(face->normal, avgn);
                if (na < INVERSE)
                {
                    lib3ds_vector_neg(face->normal);
                    na = lib3ds_vector_dot(header->planeVector, avgn);
                    if (na < INVERSE)
                    {
                        lib3ds_vector_neg(header->planeVector);
                        qSwap(face->points[1], face->points[2]);
                    }
                }
            }
#endif
#if DO_MODULATE
            for (n = neighbours; n; n = n->next)
            {
                if (n->face->smoothing & face->smoothing)
                {
                    AdjListHead *hn = reinterpret_cast<AdjListHead*>(n->face->user.p);
                    float ca = lib3ds_vector_dot(header->planeVector, hn->planeVector);
                    if (ca < ACUTE)
                    {
                        n->face->smoothing &= ~key;    // take it out of this group
                        n->face->smoothing |= altKey;  // move it to the alt group
                        keyUsed = true;
                    }
                }
            }
#endif
        }
    }

    /*
    for (Lib3dsDword f = 0; f < m_mesh->faces; ++f)
    {
        face = &m_mesh->faceL[f];
        face->user = saveData[f];
    }
    delete[] saveData;
    */
}

/*!
    \internal
    Find material indexes and smoothing groups used in this mesh.  If any
    face has no material assigned then a -1 index will be listed.
    Also figures out how complex the mesh is, by finding the count of
    smoothing groups for the material with the greatest number of groups.
    If the mesh is faceted (no smoothing) but has greater than FACETED_THRESHOLD
    faces then smoothing is forced on and the mesh rescanned.

*/
void QGL3dsMesh::analyzeMesh()
{
    QGLMaterialCollection *pal = palette();
    Lib3dsFace *face;
    Lib3dsDword allKeys = 0;
    m_smoothingGroupCount = 0;
    m_keys.clear();
    m_groupCounts.clear();
    m_plainMaterials.clear();
    m_textureMaterials.clear();
    for (Lib3dsDword f = 0; f < m_mesh->faces; ++f)
    {
        face = &m_mesh->faceL[f];
        int matIx = pal->indexOf(face->material);
#ifndef QT_NO_DEBUG_STREAM
        if (matIx == -1 && strlen(face->material) != 0)
            qDebug("Bad .3ds file: no material %s! (Referenced in mesh %s)\n",
                     face->material, m_mesh->name);
#endif
        if (face->smoothing)
        {
            if (!(face->smoothing & allKeys))
            {
                allKeys |= face->smoothing;
                m_smoothingGroupCount += 1;
                if (!m_keys.contains(matIx))
                {
                    m_keys.insert(matIx, 0);
                    m_groupCounts.insert(matIx, 0);
                }
                else
                {
                    if (!(face->smoothing & m_keys[matIx]))
                    {
                        m_keys[matIx] |= face->smoothing;
                        m_groupCounts[matIx] += 1;
                    }
                }
            }
        }
        else
        {
            m_hasZeroSmoothing = true;
        }
        if (!m_plainMaterials.contains(matIx) && !m_textureMaterials.contains(matIx))
        {
            QGLMaterial *mat = pal->material(matIx);
            if (mat && mat->texture())
                m_textureMaterials.insert(matIx);
            else
                m_plainMaterials.insert(matIx);
        }
    }
    m_smoothingGroups = allKeys;
}

/*!
    \internal
    Check the meshes textures and update the record of whether this mesh
    has textures or not.  In debug mode issue a warning if the textures are
    corrupt (number of texels and vertices not equal).
*/
void QGL3dsMesh::checkTextures(int material)
{
    QGLMaterial *mat = palette()->material(material);
    QGLTexture2D *tex = (mat ? mat->texture() : 0);
    m_hasTextures = false;
    if (tex)
    {
        m_hasTextures = true;
#ifndef QT_NO_DEBUG_STREAM
        if (m_mesh->points != m_mesh->texels)
            qWarning("Mesh %s has unequal number of texels (%d) and vertices (%d)",
                     m_mesh->name, m_mesh->texels, m_mesh->points);
#endif
        // all texture coordinates from 3ds have to be flipped because
        // 3ds uses the correct coordinate system, whilst qt uses
        // upside-down coordinates
        m_texFlip = (tex->bindOptions() & QGLContext::InvertedYBindOption) == 0;
    }
}

/*!
    \internal
    Returns any local transformation matrix for the mesh.
*/
QMatrix4x4 QGL3dsMesh::meshMatrix() const
{
    Lib3dsMatrix &m = m_mesh->matrix;  // typedef for float[4][4]
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
    qWarning("Could not invert matrix for mesh %s", m_mesh->name);
    qDebug() << mat;
#endif
    return QMatrix4x4();
}

/*!
    \internal
    Generate the vertices for the faces based on their smoothing keys and
    the current nodes material.
*/
void QGL3dsMesh::generateVertices()
{
    int matIx = currentNode()->material();
    int keyCount = m_smoothingGroupCount;
    if (m_hasZeroSmoothing)
        ++keyCount;
    QString baseName = currentNode()->objectName();
    Lib3dsDword key = m_hasZeroSmoothing ? 0 : 1;
    int lastLookup = -1;
    char lastName[512];
    ::memset(lastName, 0, 512);
    while(true)
    {
        if ((key & m_smoothingGroups) || (key == 0))
        {
            if (key == 0)
                currentSection()->setSmoothing(QGL::Faceted);
            keyCount -= 1;
            currentNode()->setMaterial(matIx);
            currentNode()->setObjectName(baseName + "::" + QString::number(key));
            QGLPrimitive tri;
            for (Lib3dsDword f = 0; f < m_mesh->faces; ++f)
            {
                Lib3dsFace *face = &m_mesh->faceL[f];
                int faceMat = 0;
                if (qstrncmp(lastName, face->material, 510) != 0)
                {
                    lastLookup = palette()->indexOf(face->material);
                    qstrncpy(lastName, face->material, 510);
                }
                faceMat = lastLookup;
                if (faceMat == matIx &&
                    ((key & face->smoothing) || (key == 0 && face->smoothing == 0)))
                {
                    Lib3dsVector &l3a = m_mesh->pointL[face->points[0]].pos;
                    Lib3dsVector &l3b = m_mesh->pointL[face->points[1]].pos;
                    Lib3dsVector &l3c = m_mesh->pointL[face->points[2]].pos;
                    Lib3dsVector &n = face->normal;
                    QVector3D &vn = l2v(n);
                    if (vn.isNull())
                        lib3ds_vector_normal(n, l3a, l3b, l3c);
                    tri.appendVertex(l2v(l3a), l2v(l3b), l2v(l3c));
                    tri.appendNormal(l2v(n), l2v(n), l2v(n));
                    if (m_hasTextures)
                    {
                        Lib3dsTexel &t0 = m_mesh->texelL[face->points[0]];
                        Lib3dsTexel &t1 = m_mesh->texelL[face->points[1]];
                        Lib3dsTexel &t2 = m_mesh->texelL[face->points[2]];
                        tri.appendTexCoord(QVector2D(t0[0], m_texFlip ? 1.0f - t0[1] : t0[1]));
                        tri.appendTexCoord(QVector2D(t1[0], m_texFlip ? 1.0f - t1[1] : t1[1]));
                        tri.appendTexCoord(QVector2D(t2[0], m_texFlip ? 1.0f - t2[1] : t2[1]));
                    }
                }
            }
            addTriangle(tri);
            if (keyCount > 0)
                newSection(QGL::Smooth);
            else
                break;
        }
        if (key == 0)
            key = 1;
        else
            key <<= 1;
    }
}
