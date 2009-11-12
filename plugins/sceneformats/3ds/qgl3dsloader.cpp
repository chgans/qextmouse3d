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

#include "qgl3dsloader.h"
#include "qglmaterialparameters.h"
#include "qglmaterialcollection.h"
#include "qglpainter.h"
#include "qgl3dsmesh.h"
#include "qgltexture2d.h"
#include "qgl3dsscene.h"
#include "qglscenenode.h"

#include <lib3ds/mesh.h>
#include <lib3ds/file.h>
#include <lib3ds/node.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>

#include <QtCore/qvarlengtharray.h>
#include <QtCore/qdir.h>
#include <QtCore/qobject.h>
#include <QtCore/qfileinfo.h>

QGL3dsLoader::QGL3dsLoader(Lib3dsFile *file)
     : mRootNode(new QGLSceneNode())
     , mFile(file)
     , mHasTextures(false)
{
    QGLGeometry *topGeometry = new QGLGeometry(mRootNode);
    mRootNode->setGeometry(topGeometry);
    mRootNode->setObjectName(file->name);
    topGeometry->setObjectName(mRootNode->objectName() + "Geometry");
    topGeometry->setPalette(new QGLMaterialCollection(topGeometry));
}

QGL3dsLoader::~QGL3dsLoader()
{
    // nothing to do here - mRootNode is taken ownership of by caller of
    // rootNode() method
}

/*!
    Sets the location for resolving resource paths to \a url.

    Only URL's with a "file" scheme are supported.
*/
void QGL3dsLoader::setUrl(const QUrl &url)
{
    // TODO
    if (url.scheme() == QLatin1String("file"))
        mUrl = url;
    else
        qWarning("QGL3dsLoader::setUrl(%s): ignored, only file urls supported",
                 url.encodedPath().constData());
}

void QGL3dsLoader::loadMesh(Lib3dsMesh *mesh)
{
#ifndef QT_NO_DEBUG_STREAM
    if (mesh->points == 0)
        qDebug() << "Mesh" << mesh->name << "has zero vertex count - ignoring";
    else if (mesh->faces == 0)
        qDebug() << "Mesh" << mesh->name << "has zero face count - ignoring";
#endif
    if (mesh->faces == 0 || mesh->points == 0)
        return;
    QGL3dsMesh *m = new QGL3dsMesh(mesh, mRootNode);
    m->setPalette(mRootNode->geometry()->palette());
    m->setObjectName(QString(mesh->name) + "Mesh");
    m->initialize();
    mMeshes.insert(mesh->name, m);
    mRefMap.insert(m, false);
    if (!mHasTextures)
        mHasTextures = m->hasTexture();
    if (!mHasLitMaterials)
        mHasLitMaterials = !m->hasTexture();
    QBox3D bounds = m->boundingBox();
    QBox3D currentBounds = mRootNode->geometry()->boundingBox();
    mRootNode->geometry()->setBoundingBox(currentBounds.expanded(bounds));
}

#ifndef QT_NO_DEBUG_STREAM
static const char *node_type_names[] = {
    "LIB3DS_UNKNOWN_NODE",
    "LIB3DS_AMBIENT_NODE",
    "LIB3DS_OBJECT_NODE",
    "LIB3DS_CAMERA_NODE",
    "LIB3DS_TARGET_NODE",
    "LIB3DS_LIGHT_NODE",
    "LIB3DS_SPOT_NODE"
};
#endif

void QGL3dsLoader::loadNodes(Lib3dsNode *nodeList, QGLSceneNode *parentNode)
{
    Lib3dsNode *node;
    for (node = nodeList; node != NULL; node = node->next)
    {
        if (node->type == LIB3DS_OBJECT_NODE)
        {
            QMatrix4x4 nodeMatrix;
            for (int col = 0; col < 4; ++col)
                for (int row = 0; row < 4; ++row)
                    nodeMatrix(row, col) = node->matrix[col][row];
            nodeMatrix.optimize();
            Lib3dsObjectData *d;
            d=&node->data.object;
            if (!qFuzzyIsNull(d->pivot[0]) || !qFuzzyIsNull(d->pivot[1]) || !qFuzzyIsNull(d->pivot[2]))
                nodeMatrix.translate(-d->pivot[0], -d->pivot[1], -d->pivot[2]);
            QGLSceneNode *sceneNode = new QGLSceneNode(parentNode);
            if (!nodeMatrix.isIdentity())
                sceneNode->setLocalTransform(nodeMatrix);
            QString meshName = node->data.object.morph;
            if (meshName.isEmpty())
                meshName = node->name;
            if (!meshName.isEmpty() && mMeshes.contains(meshName))
            {
                QGL3dsMesh *mesh = mMeshes[meshName];
                sceneNode->setGeometry(mesh);
                sceneNode->setObjectName(meshName + "Node");
                if (mesh->hasTexture())
                    sceneNode->setEffect(QGL::LitModulateTexture2D);
                else
                    sceneNode->setEffect(QGL::LitMaterial);
                Q_ASSERT(mRefMap.contains(mesh));
                mRefMap[mesh] = true;
            }
            else
            {
                qWarning("Object node named \'%s\':"
                         "cannot find a target mesh \'%s\'",
                         node->name,
                         meshName.toLocal8Bit().constData());
                sceneNode->setObjectName("No target");
            }
        }
#ifndef QT_NO_DEBUG_STREAM
        else
        {
            qDebug() << "Node" << node->name << "of type" << node_type_names[node->type] << "not currently supported";
        }
#endif
    }
}

/*!
    \internal
    Loads all the geometry, materials, and texture associations from the assigned
    file, and returns the root node of the resulting scene graph.

    The caller must take ownership of the root node returned, and delete it
    when its no longer required.
*/
QGLSceneNode *QGL3dsLoader::loadMeshes()
{
    Q_CHECK_PTR(mFile);
    Lib3dsMaterial *mat;
    for (mat = mFile->materials; mat != NULL; mat = mat->next)
        loadMaterial(mat);
    Lib3dsMesh * mesh;
    for (mesh = mFile->meshes; mesh != NULL; mesh = mesh->next)
        loadMesh(mesh);
    mRootNode->geometry()->palette()->removeUnusedMaterials();
    loadNodes(mFile->nodes, mRootNode);
    if (mHasTextures && !mHasLitMaterials)
    {
        mRootNode->setEffect(QGL::LitModulateTexture2D);
    }
    else if (!mHasTextures && mHasLitMaterials)
    {
        mRootNode->setEffect(QGL::LitMaterial);
    }
    // else leave the submeshes to duke it out

    return mRootNode;
}

/*!
    \internal
    Search for a resource file based on the given \a fileName.
    First a case-sensitive search is done of all of the current directory,
    the root directory of the resource tree, and any directories found in
    the top QGL3dsSceneNode's resourcePaths().  If the file is not found
    in any of those locations then they are searched again case-
    insensitively.  If the file is found, then the absolute file path
    of the matching file is returned.  Otherwise an empty string is
    returned.
*/
QString QGL3dsLoader::ensureResourceFile(const QString &fileName)
{
    QStringList paths;
    paths << "." << ":/";    // current directory and aliased/root resource file
    if (!mUrl.isEmpty())
    {
        QFileInfo fi(mUrl.encodedPath());
        paths.prepend(fi.absoluteDir().absolutePath());
    }
    bool caseInsensitive = false;
    do {
        QStringList::const_iterator it(paths.begin());
        for ( ; it != paths.end(); ++it)
        {
            QDir resDir(*it);
            QStringList fileList = resDir.entryList(QDir::Files);
            if (caseInsensitive)
            {
                QStringList::const_iterator fit(fileList.begin());
                for ( ; fit != fileList.end(); ++fit)
                {
                    if (fit->toLower() == fileName.toLower())
                        return resDir.absoluteFilePath(*fit);
                }
            }
            else
            {
                if (fileList.contains(fileName))
                    return resDir.absoluteFilePath(fileName);
            }
        }
        if (caseInsensitive)
            break;
        caseInsensitive = true;
    } while(true);
    return QString();
}

/*!
  Load a material
*/
void QGL3dsLoader::loadMaterial(Lib3dsMaterial *mat3ds)
{
    QGLMaterialCollection *palette = mRootNode->geometry()->palette();
    QGLMaterialParameters *mat = new QGLMaterialParameters();
    Lib3dsRgba &amb = mat3ds->ambient;
    Lib3dsRgba &dif = mat3ds->diffuse;
    Lib3dsRgba &spc = mat3ds->specular;
    mat->setAmbientColor(QColor::fromRgbF(amb[0], amb[1], amb[2], amb[3]));
    mat->setDiffuseColor(QColor::fromRgbF(dif[0], dif[1], dif[2], dif[3]));
    mat->setSpecularColor(QColor::fromRgbF(spc[0], spc[1], spc[2], spc[3]));
    mat->setShininess(128 * mat3ds->shininess);
    mat->setObjectName(mat3ds->name);
    int index = palette->addMaterial(mat);
    if (mat3ds->texture1_map.name[0])
    {
        QString txName(mat3ds->texture1_map.name);
        txName = ensureResourceFile(txName);
        if (txName.isEmpty())
        {
            qWarning("Could not load texture: %s", mat3ds->texture1_map.name);
        }
        else
        {
            QGLTexture2D *texture = new QGLTexture2D();
            if (txName.toLower().endsWith(".dds"))
                texture->setDdsImage(txName);
            else
                texture->setImage(QImage(txName));
            palette->setTexture(index, texture);
        }
    }
}
