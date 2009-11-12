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

#include "qgl3dsscene.h"

#include "qglscenenode.h"
#include "qgl3dsmesh.h"
#include "qgl3dsloader.h"

#include <lib3ds/file.h>

QT_BEGIN_NAMESPACE

/*!
    \internal
    \class QGL3dsScene
    \brief The QGL3dsScene class manages and contains a 3ds scene.
    The scene consists of a QGLSceneNode object which is the top-level node
    and references the geometry in a 3D Studio Max ".3ds" file.  That top
    level node references sub-nodes
*/

/*!
    \internal
    Construct a new QGL3dsScene object using the data in the \a file,
    and setting the given \a parent.  Resources are searched for at the
    given \a url.

    The QGL3dsSceneObject takes ownership of the \a file.
*/
QGL3dsScene::QGL3dsScene(Lib3dsFile *file, QUrl url, QObject *parent)
    : QGLAbstractScene(parent)
    , mFile(file)
    , mUrl(url)
{
    QGL3dsLoader loader(file);
    loader.setUrl(mUrl);
    mRootNode = loader.loadMeshes();
}

/*!
    \reimp
    Destroy this QGL3dsScene, recovering all resources.

    This method destroys the Lib3dsFile object the scene is based
    on by calling the appropriate lib3ds function.
*/
QGL3dsScene::~QGL3dsScene()
{
    lib3ds_file_free(mFile);
}

/*!
    \internal
    \reimp
*/
QList<QGLSceneObject *> QGL3dsScene::objects(QGLSceneObject::Type type) const
{
    QList<QGLSceneObject *> objs;
    if (type == QGLSceneObject::Main)
    {
        objs.append(mRootNode);
        return objs;
    }
    QList<QGLSceneObject *> nodes;
    nodes.append(mRootNode);
    while (!nodes.isEmpty())
    {
        QGLSceneObject *o = nodes.takeFirst();
        if (o->type() == type)
            objs.append(o);
        QObjectList children = o->children();
        QObjectList::iterator it(children.begin());
        for ( ; it != children.end(); ++it)
        {
            QGLSceneObject *c = qobject_cast<QGLSceneObject *>(*it);
            if (c)
            {
                nodes.append(c);
                if (c->type() == type)
                    objs.append(c);
            }
        }
    }
    return objs;
}

QT_END_NAMESPACE
