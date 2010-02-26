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

#include <lib3ds/mesh.h>
#include <lib3ds/material.h>
#include <lib3ds/vector.h>

#include <QtGui/qmatrix4x4.h>

// Faceted meshes look terrible when they have more than a small number
// of faces.  Usually if large meshes are faceted its some kind of error
// in the model, or in importing of the model by lib3ds.  Force on
// smoothing when greater than this many faces are detected.
#define FACETED_THRESHOLD 1000

QGL3dsMesh::QGL3dsMesh(Lib3dsMesh *mesh, QObject *parent,
                       QGLMaterialCollection *materials)
    : QGLDisplayList(parent, materials)
    , m_mesh(mesh)
    , m_texFlip(false)
{
    setObjectName(QString(mesh->name));
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
    analyzeMesh();
    if (m_smoothingGroups == 0 && m_mesh->faces > FACETED_THRESHOLD)
    {
#ifndef QT_NO_DEBUG_STREAM
        qDebug("Mesh %s has %d faces (threshold is %d): forcing smooth render",
               m_mesh->name, m_mesh->faces, FACETED_THRESHOLD);
#endif
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
        int matIx = pal->materialIndexByName(face->material);
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
        if (!m_plainMaterials.contains(matIx) && !m_textureMaterials.contains(matIx))
        {
            if (pal->texture(matIx))
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
    QGLTexture2D *tex = palette()->texture(material);
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
        m_texFlip = tex->flipVertically();
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
    QString baseName = currentNode()->objectName();
    for (Lib3dsDword key = 1; key; key <<= 1)
    {
        keyCount -= 1;
        if (key & m_smoothingGroups || key == 1)
        {
            if (key != 1)
                currentNode()->setObjectName(baseName + "::" + QString::number(key));
            for (Lib3dsDword f = 0; f < m_mesh->faces; ++f)
            {
                Lib3dsFace *face = &m_mesh->faceL[f];
                if (palette()->materialIndexByName(face->material) == matIx &&
                    ((key & face->smoothing) || (key == 1 && face->smoothing == 0)))
                {
                    Lib3dsVector &l3a = m_mesh->pointL[face->points[0]].pos;
                    Lib3dsVector &l3b = m_mesh->pointL[face->points[1]].pos;
                    Lib3dsVector &l3c = m_mesh->pointL[face->points[2]].pos;
                    QGLOperation op(this, QGL::TRIANGLE);
                    op << QVector3D(l3a[0], l3a[1], l3a[2]);
                    op << QVector3D(l3b[0], l3b[1], l3b[2]);
                    op << QVector3D(l3c[0], l3c[1], l3c[2]);
                    if (m_hasTextures)
                    {
                        Lib3dsTexel &t0 = m_mesh->texelL[face->points[0]];
                        Lib3dsTexel &t1 = m_mesh->texelL[face->points[1]];
                        Lib3dsTexel &t2 = m_mesh->texelL[face->points[2]];
                        op << QVector2D(t0[0], m_texFlip ? 1.0f - t0[1] : t0[1]);
                        op << QVector2D(t1[0], m_texFlip ? 1.0f - t1[1] : t1[1]);
                        op << QVector2D(t2[0], m_texFlip ? 1.0f - t2[1] : t2[1]);
                    }
                }
            }
        }
        if (keyCount > 0)
        {
            newSection(QGL::Smooth);
            currentNode()->setMaterial(matIx);
        }
    }
}
