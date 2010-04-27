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

#include "qgl3dsloader.h"
#include "qglmaterial.h"
#include "qglmaterialcollection.h"
#include "qglpainter.h"
#include "qgltexture2d.h"
#include "qgl3dsscene.h"
#include "qglscenenode.h"
#include "qgl3dsmesh.h"

#include <lib3ds/mesh.h>
#include <lib3ds/file.h>
#include <lib3ds/node.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>

#include <QtCore/qvarlengtharray.h>
#include <QtCore/qdir.h>
#include <QtCore/qobject.h>
#include <QtCore/qfileinfo.h>

QGL3dsLoader::QGL3dsLoader(Lib3dsFile *file, QGL3dsSceneHandler* sh)
     : mFile(file)
     , mRootNode(new QGLSceneNode())
     , mScene(sh)
     , mHasTextures(false)
{
    mRootNode->setPalette(new QGLMaterialCollection(mRootNode));
    mRootNode->setObjectName(file->name);
    setUrl(sh->url());
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
    if (mesh->points == 0 && (mScene->options() & QGL::ShowWarnings))
        qDebug() << "Mesh" << mesh->name << "has zero vertex count";
    else if (mesh->faces == 0 && (mScene->options() & QGL::ShowWarnings))
        qDebug() << "Mesh" << mesh->name << "has zero face count";
#endif
    QGL3dsMesh *m = new QGL3dsMesh(mesh, mRootNode, mRootNode->palette());
    QGL::ModelOptions o = mScene->meshOptions(mesh->name);
    if (o == 0)
        o = mScene->options();
    m->setOptions(o);
    mMeshes.insert(mesh->name, m);
    if (mesh->faces == 0 || mesh->points == 0)
        return;
    m->initialize();
    if (!mHasTextures)
        mHasTextures = m->hasTexture();
    if (!mHasLitMaterials)
        mHasLitMaterials = !m->hasTexture();
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

inline static QMatrix4x4 getNodeMatrix(Lib3dsNode *node)
{
    QMatrix4x4 nodeMatrix;
    for (int col = 0; col < 4; ++col)
        for (int row = 0; row < 4; ++row)
            nodeMatrix(row, col) = node->matrix[col][row];
    nodeMatrix.optimize();
    Lib3dsObjectData *d = &node->data.object;
    if (!qFuzzyIsNull(d->pivot[0]) || !qFuzzyIsNull(d->pivot[1]) || !qFuzzyIsNull(d->pivot[2]))
        nodeMatrix.translate(-d->pivot[0], -d->pivot[1], -d->pivot[2]);
    return nodeMatrix;
}

void QGL3dsLoader::loadNodes(Lib3dsNode *nodeList, QGLSceneNode *parentNode)
{
    Lib3dsNode *node;
    for (node = nodeList; node != NULL; node = node->next)
    {
        if (node->type == LIB3DS_OBJECT_NODE)
        {
            Lib3dsObjectData *d = &node->data.object;
            QString meshName = d->morph;
            if (meshName.isEmpty())
                meshName = d->instance;
            if (meshName.isEmpty())
                meshName = node->name;
            if (!meshName.isEmpty() && mMeshes.contains(meshName))
            {
                QGL3dsMesh *mesh = mMeshes[meshName];
                QMatrix4x4 mat = getNodeMatrix(node);
                mat = mat * mesh->localTransform();
                mesh->setLocalTransform(mat);
            }
            else
            {
                QGLSceneNode *sceneNode = new QGLSceneNode(parentNode);
                sceneNode->setPalette(parentNode->palette());
                sceneNode->setLocalTransform(getNodeMatrix(node));
                //sceneNode->userTransform().setToIdentity();		//DP: set matrix to identity so it is initialised in a useful way at least.
                QString nodeName(node->name);
                if (nodeName == QLatin1String("$$$DUMMY"))
                {
                    nodeName = node->data.object.instance;
                    sceneNode->setObjectName(nodeName);
                    loadNodes(node->childs, sceneNode);
                }
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
    QGL::MeshOptionMap optList = mScene->meshOptions();
    QStringList optionedMeshes = optList.keys();
    QStringList gotMeshes = mMeshes.keys();
    for (int i = 0; i < gotMeshes.size(); ++i)
        optionedMeshes.removeAll(gotMeshes.at(i));
    for (int i = 0; i < optionedMeshes.size(); ++i)
        qWarning("Option specified, but mesh %s not found",
                 qPrintable(optionedMeshes.at(i)));
    mRootNode->palette()->removeUnusedMaterials();
    loadNodes(mFile->nodes, mRootNode);
    mRootNode->setEffect(mHasTextures ? QGL::LitModulateTexture2D : QGL::LitMaterial);
    return mRootNode;
}

/*!
    \internal
    Search for a resource based on the given \a path.

    If the URL for the currently loading mesh has a scheme other than
    "file" then a URL with the path relative to that URL is returned.

    If the URL for the currently loading mesh has a "file" scheme, then
    first a case-sensitive search is done of all of the current directory,
    and the :/ resource directory, and the directory of the current mesh
    file.

    If the file is not found in any of those locations then they are
    searched again case-insensitively.  If the file is found, then a
    URL based on the absolute file path of the matching file is returned.

    Otherwise an empty string is returned.
*/
QUrl QGL3dsLoader::ensureResource(const QString &path)
{
    QUrl res;
    if (mUrl.scheme() == QLatin1String("file"))
    {
        res = mUrl.resolved(path);
        if (QFile::exists(res.path())) // shortcut common case
            return res;
        QStringList paths;
        paths << "." << ":/";    // current directory and aliased/root resource file
        if (!mUrl.isEmpty())
        {
            QFileInfo fi(mUrl.path());
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
                        if (fit->toLower() == path.toLower())
                        {
                            res.setPath(resDir.absoluteFilePath(*fit));
                            break;
                        }
                    }
                }
                else
                {
                    if (fileList.contains(path))
                        return resDir.absoluteFilePath(path);
                }
            }
            if (caseInsensitive)
                break;
            caseInsensitive = true;
        } while(true);
    }
    else
    {
        // non-file url
        res = mUrl.resolved(path);
    }
    return res;
}

/*!
  Load a material
*/
void QGL3dsLoader::loadMaterial(Lib3dsMaterial *mat3ds)
{
    QGLMaterialCollection *palette = mRootNode->palette();
    QGLMaterial *mat = new QGLMaterial();
    Lib3dsRgba &amb = mat3ds->ambient;
    Lib3dsRgba &dif = mat3ds->diffuse;
    Lib3dsRgba &spc = mat3ds->specular;
    mat->setAmbientColor(QColor::fromRgbF(amb[0], amb[1], amb[2], amb[3]));
    mat->setDiffuseColor(QColor::fromRgbF(dif[0], dif[1], dif[2], dif[3]));
    mat->setSpecularColor(QColor::fromRgbF(spc[0], spc[1], spc[2], spc[3]));
    mat->setShininess(128 * mat3ds->shininess);
    mat->setObjectName(mat3ds->name);
    palette->addMaterial(mat);
    if (mat3ds->texture1_map.name[0])
    {
        QString txName(mat3ds->texture1_map.name);
        QUrl url = ensureResource(txName);
        if (url.isEmpty())
        {
            if (mScene->options() & QGL::ShowWarnings)
                qWarning("Could not load texture: %s", mat3ds->texture1_map.name);
        }
        else
        {
            mat->setTextureUrl(url);
        }
    }
}
