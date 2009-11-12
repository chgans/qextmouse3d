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

#ifndef QGLOBJSCENE_H
#define QGLOBJSCENE_H

#include "qglabstractscene.h"
#include "qglobjgeometry.h"

QT_BEGIN_NAMESPACE

class QGLObjObject : public QGLSceneObject
{
    Q_OBJECT
public:
    explicit QGLObjObject(QGLObjGeometry *geometry, QObject *parent = 0);
    QGLObjObject(QGLObjGeometry *geometry, const QString& name,
                 int first, int count, QObject *parent = 0);
    ~QGLObjObject();

    void draw(QGLPainter *painter);

private:
    QGLObjGeometry *geometry;
    int first, count;
};

class QGLObjScene : public QGLAbstractScene
{
    Q_OBJECT
public:
    explicit QGLObjScene(QGLObjGeometry *geometry, QObject *parent = 0);
    virtual ~QGLObjScene();

    QList<QGLSceneObject *> objects(QGLSceneObject::Type type) const;

private:
    QGLObjObject *mainObject;
    QList<QGLSceneObject *> meshes;
};

QT_END_NAMESPACE

#endif
