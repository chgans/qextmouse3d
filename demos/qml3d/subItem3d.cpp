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

#include <time.h>

#include "subItem3d.h"
#include "item3d.h"
#include "viewport.h"
#include "mesh.h"
#include "effect.h"
#include "qgllightparameters.h"
#include "qml3dview.h"
#include "qglabstractscene.h"
#include "qglsceneobject.h"
#include "qglscenenode.h"
#include <QtGui/qevent.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/private/qmlstategroup_p.h>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,SubItem3d,SubItem3d)

class SubItem3dPrivate
{
public:
    SubItem3dPrivate(SubItem3d *_item)
        : item(_item)
        , position(0.0f, 0.0f, 0.0f)
        , scale(1.0f)
        , objectPickId(-1)
    {
		//meshNode init?
		subTransform.setToIdentity();
    }

    SubItem3d *item;
    Viewport *viewport;
    QVector3D position;
    qreal scale;
    Mesh *mesh;
    Effect *effect;
    int objectPickId;
    Item3d::CullFaces cullFaces;
    QString meshNode;
    QGLSceneNode * targetNode;
	QMatrix4x4 subTransform;
};

SubItem3d::SubItem3d(QObject *parent)
    : QObject(parent)
{
    d = new SubItem3dPrivate(this);
}

SubItem3d::~SubItem3d()
{
    delete d;
}

QVector3D SubItem3d::position() const
{
    return d->position;
}

void SubItem3d::setPosition(const QVector3D& value)
{
    d->position = value;
    emit positionChanged();
        transform();
		update();
}

qreal SubItem3d::x() const
{
    return d->position.x();
}

void SubItem3d::setX(qreal value)
{
    d->position.setX(value);
    emit positionChanged();
    transform();
	update();
}

qreal SubItem3d::y() const
{
    return d->position.y();
}

void SubItem3d::setY(qreal value)
{
    d->position.setY(value);
    emit positionChanged();
    transform();
	update();
}

qreal SubItem3d::z() const
{
    return d->position.z();
}

void SubItem3d::setZ(qreal value)
{
    d->position.setZ(value);
    emit positionChanged();
    transform();
	update();
}

qreal SubItem3d::scale() const
{
    return d->scale;
}

void SubItem3d::setScale(qreal value)
{
    d->scale = value;
    emit scaleChanged();
    transform();
	update();
}


QString SubItem3d::meshNode() const
{
	return d->meshNode;
}

void SubItem3d::setMeshNode(const QString &node)
{
	d->meshNode = node;
}

void SubItem3d::transform()
{
    d->subTransform.setToIdentity();
	d->subTransform.translate(x(),y(),z());
}

void SubItem3d::update()
{
	Item3d * parentItem = (Item3d *)this->parent();
	parentItem->update();
}

void SubItem3d::performTransform()
{
	if (d->targetNode)
		d->targetNode->setUserTransform(d->subTransform);
}

void SubItem3d::setMeshObject(QGLSceneNode *object)
{
	d->targetNode = object;
}

QGLSceneNode * SubItem3d::meshObject()
{
	return d->targetNode;
}

QT_END_NAMESPACE
