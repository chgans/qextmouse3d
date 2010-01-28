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

#include "qglobjscene.h"
#include "qgldisplaylist.h"
#include "qglsceneobject.h"

QT_BEGIN_NAMESPACE

QGLObjScene::QGLObjScene(QGLDisplayList *list,
                         QGLSceneObject *defaultNode,
                         const QList<QGLSceneObject *>& otherNodes,
                         QObject *parent)
    : QGLAbstractScene(parent),
      displayList(list),
      mainObject(defaultNode),
      meshes(otherNodes)
{
    displayList->setParent(this);
}

QGLObjScene::~QGLObjScene()
{
}

QList<QGLSceneObject *> QGLObjScene::objects(QGLSceneObject::Type type) const
{
    QList<QGLSceneObject *> objs;
    if (type == QGLSceneObject::Mesh) {
        objs.append(mainObject);
        objs.append(meshes);
    } else if (type == QGLSceneObject::Main) {
        objs.append(mainObject);
    }
    return objs;
}

QT_END_NAMESPACE
