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

#include "qaiscene.h"
#include "qaimesh.h"
#include "qailoader.h"

#include "qglscenenode.h"

QT_BEGIN_NAMESPACE

/*!
    \internal
    \class QAiScene
    \brief The QAiScene class manages and contains an asset importer scene.
    The scene consists of a QGLSceneNode object which is the top-level node
    and references the geometry imported from a file.
*/

/*!
    \internal
    Construct a new QAiScene object using the data in the \a scene,
    and setting the given \a parent.

    The QAiScene object takes ownership of the \a file.
*/
QAiScene::QAiScene(const aiScene *scene, QAiSceneHandler *handler)
    : QGLAbstractScene(0)
{
    Q_ASSERT(handler);
    Q_ASSERT(scene);
    QAiLoader loader(scene, handler);
    m_root = loader.loadMeshes();
}

/*!
    \reimp
    Destroy this QAiScene, recovering all resources.

    This method destroys the Lib3dsFile object the scene is based
    on by calling the appropriate lib3ds function.
*/
QAiScene::~QAiScene()
{
    // nothing to do here
}

/*!
    \internal
    \reimp
*/
QList<QObject *> QAiScene::objects() const
{
    QList<QObject *> objs;
    if (!m_root)
        return objs;
    objs.append(m_root);
    QList<QGLSceneNode*> children = m_root->allChildren();
    QList<QGLSceneNode*>::const_iterator it = children.constBegin();
    for ( ; it != children.constEnd(); ++it)
        objs.append(*it);
    return objs;
}

/*!
    \internal
    \reimp
*/
QGLSceneNode *QAiScene::mainNode() const
{
    return m_root;
}

QT_END_NAMESPACE
