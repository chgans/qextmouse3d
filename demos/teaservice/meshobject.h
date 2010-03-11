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

#ifndef MESHOBJECT_H
#define MESHOBJECT_H

#include <QtCore/qobject.h>
#include <QtGui/qevent.h>
#include "qglgeometry.h"
#include "qglpainter.h"
#include "qglsceneobject.h"
#include "qgldisplaylist.h"
#include "qglabstractscene.h"

class QGLView;

class MeshObject : public QObject
{
    Q_OBJECT
public:
    explicit MeshObject(QGLDisplayList *mesh, QObject *parent=0);
    explicit MeshObject(QGLSceneObject *meshObject, QObject *parent=0);
    explicit MeshObject(QGLAbstractScene *scene, QObject *parent=0);
    virtual ~MeshObject();

    QVector3D position() const { return m_position; }
    void setPosition(const QVector3D& value) { m_position = value; }

    qreal scale() const { return m_scale; }
    void setScale(qreal value) { m_scale = value; }

    qreal rotationAngle() const { return m_rotationAngle; }
    void setRotationAngle(qreal value) { m_rotationAngle = value; }

    QVector3D rotationVector() const { return m_rotationVector; }
    void setRotationVector(const QVector3D& value) { m_rotationVector = value; }

    QGLMaterial *material() const { return m_material; }
    void setMaterial(QGLMaterial *value)
        { m_material = value; m_hoverMaterial = value; }

    QGLMaterial *hoverMaterial() const { return m_hoverMaterial; }
    void setHoverMaterial(QGLMaterial *value) { m_hoverMaterial = value; }

    QGLAbstractEffect *effect() const { return m_effect; }
    void setEffect(QGLAbstractEffect *value) { m_effect = value; }

    int objectId() const { return m_objectId; }
    void setObjectId(int id) { m_objectId = id; }

    void initialize(QGLView *view, QGLPainter *painter);
    void draw(QGLPainter *painter);

signals:
    void pressed();
    void released();
    void clicked();
    void doubleClicked();
    void hoverChanged();

protected:
    bool event(QEvent *e);

private:
    QGLDisplayList *m_mesh;
    QGLSceneObject *m_meshObject;
    QGLAbstractScene *m_scene;
    QVector3D m_position;
    qreal m_scale;
    qreal m_rotationAngle;
    QVector3D m_rotationVector;
    QGLMaterial *m_material;
    QGLMaterial *m_hoverMaterial;
    QGLAbstractEffect *m_effect;
    int m_objectId;
    bool m_hovering;
};

#endif
