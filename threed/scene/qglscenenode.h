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

#ifndef QGLSCENENODE_H
#define QGLSCENENODE_H

#include "qglsceneobject.h"
#include "qglgeometry.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLSceneNodePrivate;
class QGLGeometry;
class QGLAbstractEffect;

class Q_QT3D_EXPORT QGLSceneNode : public QGLSceneObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGLSceneNode);
public:
    explicit QGLSceneNode(QObject *parent = 0);
    explicit QGLSceneNode(QGLGeometry *geometry, QObject *parent = 0);
    virtual ~QGLSceneNode();

    QGLGeometry *geometry() const;
    void setGeometry(QGLGeometry *);

    QMatrix4x4 localTransform() const;
    void setLocalTransform(const QMatrix4x4 &);

    QGL::StandardEffect effect() const;
    void setEffect(QGL::StandardEffect);

    QGLAbstractEffect *userEffect() const;
    void setUserEffect(QGLAbstractEffect *effect);

    bool hasEffect() const;
    void setEffectEnabled(bool);

    int start() const;
    void setStart(int start);

    int count() const;
    void setCount(int count);

    int material() const;
    void setMaterial(int material);

    QList<QGLSceneNode*> childNodes() const;
    void setChildNodes(const QList<QGLSceneNode*> &children);
    void addNode(QGLSceneNode *node);
    void removeNode(QGLSceneNode *node);
    void setParent(QObject *);

    virtual void draw(QGLPainter *painter);
    virtual void apply(QGLPainter *painter);
    virtual QGLSceneNode *clone(QObject *parent = 0) const;
protected:
    QGLSceneNode(QGLSceneNodePrivate &dd, QObject *parent = 0);

private:
    Q_DISABLE_COPY(QGLSceneNode)
};

#ifndef QT_NO_DEBUG_STREAM
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QGLSceneNode &node);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif
