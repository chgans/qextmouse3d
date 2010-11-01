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

#ifndef QGLSCENENODE_H
#define QGLSCENENODE_H

#include <QtCore/qobject.h>

#include "qgeometrydata.h"
#include "qglmaterialcollection.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLSceneNodePrivate;
class QGLAbstractEffect;
class QGLPickNode;

class Q_QT3D_EXPORT QGLSceneNode : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGLSceneNode)
    Q_PROPERTY(QVector3D rotation READ rotation WRITE setRotation NOTIFY updated)
    Q_PROPERTY(qreal rotX READ rotX WRITE setRotX NOTIFY updated)
    Q_PROPERTY(qreal rotY READ rotY WRITE setRotY NOTIFY updated)
    Q_PROPERTY(qreal rotZ READ rotZ WRITE setRotZ NOTIFY updated)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY updated)
    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY updated)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY updated)
    Q_PROPERTY(qreal z READ z WRITE setZ NOTIFY updated)
    Q_PROPERTY(QVector3D scale READ scale WRITE setScale NOTIFY updated)
    Q_PROPERTY(QGLMaterial *material READ material WRITE setMaterial NOTIFY updated)
    Q_PROPERTY(QGLMaterial *backMaterial READ backMaterial WRITE setBackMaterial NOTIFY updated)
public:
    explicit QGLSceneNode(QObject *parent = 0);
    explicit QGLSceneNode(const QGeometryData &geometry, QObject *parent = 0);
    virtual ~QGLSceneNode();

    enum Option
    {
        CullBoundingBox = 0x0001,
        ViewNormals     = 0x0002
    };
    Q_DECLARE_FLAGS(Options, Option)

    QGLSceneNode::Options options() const;
    void setOptions(QGLSceneNode::Options options);
    void setOption(QGLSceneNode::Option option, bool value);

    QGeometryData geometry() const;
    void setGeometry(QGeometryData);

    QBox3D boundingBox() const;

    QMatrix4x4 localTransform() const;
    void setLocalTransform(const QMatrix4x4 &);
    QVector3D rotation() const;
    void setRotation(const QVector3D &r);
    qreal rotX() const;
    void setRotX(qreal rx);
    qreal rotY() const;
    void setRotY(qreal ry);
    qreal rotZ() const;
    void setRotZ(qreal rz);
    QVector3D position() const;
    void setPosition(const QVector3D &p);
    qreal x() const;
    void setX(qreal x);
    qreal y() const;
    void setY(qreal y);
    qreal z() const;
    void setZ(qreal z);
    QVector3D scale() const;
    void setScale(const QVector3D &scale);

    QGL::DrawingMode drawingMode() const;
    void setDrawingMode(QGL::DrawingMode mode);

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

    int materialIndex() const;
    void setMaterialIndex(int material);
    int backMaterialIndex() const;
    void setBackMaterialIndex(int material);

    QGLMaterial *material() const;
    void setMaterial(QGLMaterial *material);
    QGLMaterial *backMaterial() const;
    void setBackMaterial(QGLMaterial *material);

    QGLMaterialCollection *palette() const;
    void setPalette(QGLMaterialCollection *palette);

    QList<QGLSceneNode*> allChildren() const;
    QList<QGLSceneNode *> childNodeList() const;
    void setChildNodeList(const QList<QGLSceneNode*> &children);
    void addNode(QGLSceneNode *node);
    void removeNode(QGLSceneNode *node);

    virtual void draw(QGLPainter *painter);

    QGLPickNode *pickNode() const;
    void setPickNode(QGLPickNode *node);

    Q_INVOKABLE QGLSceneNode *clone(QObject *parent = 0) const;
    Q_INVOKABLE QGLSceneNode *allExcept(const QString &name, QObject *parent = 0) const;
    Q_INVOKABLE QGLSceneNode *only(const QString &name, QObject *parent = 0) const;
    Q_INVOKABLE QGLSceneNode *allExcept(const QStringList &names, QObject *parent = 0) const;
    Q_INVOKABLE QGLSceneNode *only(const QStringList &names, QObject *parent = 0) const;

Q_SIGNALS:
    void updated();

protected:
    virtual void drawGeometry(QGLPainter *painter);

private:
    QMatrix4x4 transform() const;
    void invalidateBoundingBox() const;
    void invalidateTransform() const;
    void drawNormalIndicators(QGLPainter *painter);
    const QGLMaterial *setPainterMaterial(int material, QGLPainter *painter,
                                    QGL::Face faces, bool &changedTex);

    Q_DISABLE_COPY(QGLSceneNode)

    QScopedPointer<QGLSceneNodePrivate> d_ptr;

    QGLSceneNode(QGLSceneNodePrivate *d, QObject *parent);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGLSceneNode::Options)

#ifndef QT_NO_DEBUG_STREAM
Q_QT3D_EXPORT void qDumpScene(QGLSceneNode *, int indent = 0, const QSet<QGLSceneNode*> &loop = QSet<QGLSceneNode*>());

Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QGLSceneNode &node);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif
