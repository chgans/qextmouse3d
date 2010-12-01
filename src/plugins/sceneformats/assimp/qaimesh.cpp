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

#include "qaimesh.h"
#include "qglmaterialcollection.h"

#include <QtGui/qmatrix4x4.h>
#include <QtCore/qmath.h>

#include "aiMesh.h"
#include "DefaultLogger.h"

QAiMesh::QAiMesh(aiMesh *mesh)
    : m_mesh(mesh)
{
}

QAiMesh::~QAiMesh()
{
}

static inline QVector2D qv2d(const aiVector3D &v)
{
    return QVector2D(v.x, v.y);
}

static inline QVector3D qv3d(const aiVector3D &v)
{
    return QVector3D(v.x, v.y, v.z);
}

QGLSceneNode *QAiMesh::loadTriangles()
{
    QGLBuilder builder;
    QGeometryData data;
    for (unsigned int i = 0; i < m_mesh->mNumVertices; ++i)
        data.appendVertex(qv3d(m_mesh->mVertices[i]));
    if (m_mesh->HasNormals())
        for (unsigned int i = 0; i < m_mesh->mNumVertices; ++i)
            data.appendNormal(qv3d(m_mesh->mNormals[i]));
    int k = m_mesh->GetNumUVChannels();
    for (int t = 0; t < k; ++t)
    {
        if (m_mesh->mNumUVComponents[t] != 2)
            Assimp::DefaultLogger::get()->warn("Tex co-ords only supports U & V");
        for (unsigned int i = 0; i < m_mesh->mNumVertices; ++i)
            data.appendTexCoord(qv2d(m_mesh->mTextureCoords[t][i]), static_cast<QGL::VertexAttribute>(QGL::TextureCoord0 + t));
    }
    for (unsigned int i = 0; i < m_mesh->mNumFaces; ++i)
    {
        aiFace *face = &m_mesh->mFaces[i];
        data.appendIndices(face->mIndices[0], face->mIndices[1], face->mIndices[2]);
    }
    // invoke raw mode (with indexes)
    builder.addTriangles(data);
    return builder.finalizedSceneNode();
}

static inline void assertOnePrimitiveType(aiMesh *mesh)
{
#ifndef QT_NO_DEBUG
    int k = 0;  // count the number of bits set in the primitives
    unsigned int msk = 0x01;
    for (unsigned int p = mesh->mPrimitiveTypes; p; p >>= 1)
        if (p & msk)
            ++k;
    Q_ASSERT(k == 1);  // Assimp SortByPType promises this
#endif
}

QGLSceneNode *QAiMesh::build()
{
    assertOnePrimitiveType(m_mesh);
    QGLSceneNode *s = 0;
    if (m_mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE)
    {
        s = loadTriangles();
    }
    else if (m_mesh->mPrimitiveTypes & aiPrimitiveType_POINT)
    {
        // TODO: not implemented yet
        Assimp::DefaultLogger::get()->warn("Point primitive not implemented yet");
        return 0;
    }
    else if (m_mesh->mPrimitiveTypes & aiPrimitiveType_LINE)
    {
        // TODO: not implemented yet
        Assimp::DefaultLogger::get()->warn("Line primitive not implemented yet");
        return 0;
    }
    else
    {
        // TODO: not implemented yet
        Assimp::DefaultLogger::get()->warn("Unsupported primitive type");
        return 0;
    }
    return s;
}
