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

#include "item3d.h"
#include "viewport.h"
#include "mesh.h"
#include "effect.h"
#include "qgllightparameters.h"
#include "qml3dview.h"
#include "qglabstractscene.h"
#include "qglsceneobject.h"
#include <QtGui/qevent.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/private/qmlstategroup_p.h>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,Item3d,Item3d)

class Item3dPrivate
{
public:
    Item3dPrivate(Item3d *_item)
        : item(_item)
        , viewport(0)
        , position(0.0f, 0.0f, 0.0f)
        , scale(1.0f)
        , mesh(0)
        , effect(0)
        , objectPickId(-1)
        , cullFaces(Item3d::CullDisabled)
        , _stateGroup(0)
    {
    }

    Item3d *item;
    Viewport *viewport;
    QVector3D position;
    qreal scale;
    Mesh *mesh;
    Effect *effect;
    int objectPickId;
    Item3d::CullFaces cullFaces;

    // data property
    void data_removeAt(int);
    int data_count() const;
    void data_append(QObject *);
    void data_insert(int, QObject *);
    QObject *data_at(int) const;
    void data_clear();
    QML_DECLARE_LIST_PROXY(Item3dPrivate, QObject *, data)

    // resources property
    void resources_removeAt(int);
    int resources_count() const;
    void resources_append(QObject *);
    void resources_insert(int, QObject *);
    QObject *resources_at(int) const;
    void resources_clear();
    QML_DECLARE_LIST_PROXY(Item3dPrivate, QObject *, resources)

    // children property
    void children_removeAt(int);
    int children_count() const;
    void children_append(Item3d *);
    void children_insert(int, Item3d *);
    Item3d *children_at(int) const;
    void children_clear();
    QML_DECLARE_LIST_PROXY(Item3dPrivate, Item3d *, children)

    // transform property
    void transform_removeAt(int);
    int transform_count() const;
    void transform_append(QGraphicsTransform *);
    void transform_insert(int, QGraphicsTransform *);
    QGraphicsTransform *transform_at(int) const;
    void transform_clear();
    QML_DECLARE_LIST_PROXY(Item3dPrivate, QGraphicsTransform *, transform)
    QList<QGraphicsTransform *> transforms;

    QmlStateGroup *states();
    QmlStateGroup *_stateGroup;
};

Item3d::Item3d(QObject *parent)
    : QObject(parent)
{
    d = new Item3dPrivate(this);
}

Item3d::~Item3d()
{
    delete d;
}

QVector3D Item3d::position() const
{
    return d->position;
}

void Item3d::setPosition(const QVector3D& value)
{
    d->position = value;
    emit positionChanged();
    update();
}

qreal Item3d::x() const
{
    return d->position.x();
}

void Item3d::setX(qreal value)
{
    d->position.setX(value);
    emit positionChanged();
    update();
}

qreal Item3d::y() const
{
    return d->position.y();
}

void Item3d::setY(qreal value)
{
    d->position.setY(value);
    emit positionChanged();
    update();
}

qreal Item3d::z() const
{
    return d->position.z();
}

void Item3d::setZ(qreal value)
{
    d->position.setZ(value);
    emit positionChanged();
    update();
}

qreal Item3d::scale() const
{
    return d->scale;
}

void Item3d::setScale(qreal value)
{
    d->scale = value;
    emit scaleChanged();
    update();
}

void Item3dPrivate::transform_removeAt(int i)
{
    transforms.removeAt(i);
    item->update();
}

int Item3dPrivate::transform_count() const
{
    return transforms.count();
}

void Item3dPrivate::transform_append(QGraphicsTransform *item)
{
    if (!transforms.contains(item)) {
        transforms.append(item);

        // XXX - need a better way of doing this.
        if (qobject_cast<QGraphicsScale *>(item)) {
            QObject::connect(item, SIGNAL(originChanged()),
                             this->item, SLOT(update()));
            QObject::connect(item, SIGNAL(scaleChanged()),
                             this->item, SLOT(update()));
        } else if (qobject_cast<QGraphicsRotation *>(item)) {
            QObject::connect(item, SIGNAL(originChanged()),
                             this->item, SLOT(update()));
            QObject::connect(item, SIGNAL(angleChanged()),
                             this->item, SLOT(update()));
            QObject::connect(item, SIGNAL(axisChanged()),
                             this->item, SLOT(update()));
        }

        this->item->update();
    }
}

void Item3dPrivate::transform_insert(int, QGraphicsTransform *)
{
    // ###
}

QGraphicsTransform *Item3dPrivate::transform_at(int idx) const
{
    return transforms.at(idx);
}

void Item3dPrivate::transform_clear()
{
    transforms.clear();
    item->update();
}

QmlList<QGraphicsTransform *>* Item3d::transform()
{
    return &(d->transform);
}

Mesh *Item3d::mesh() const
{
    return d->mesh;
}

void Item3d::setMesh(Mesh *value)
{
    if (d->mesh) {
        if (!d->mesh->deref())
            delete d->mesh;
    }
    d->mesh = value;
    if (value) {
        d->mesh->ref();
        connect(value, SIGNAL(dataChanged()), this, SIGNAL(meshChanged()));
        connect(value, SIGNAL(dataChanged()), this, SLOT(update()));
    }
    emit meshChanged();
    update();
}

Effect *Item3d::effect() const
{
    return d->effect;
}

void Item3d::setEffect(Effect *value)
{
    if (d->effect == value)
        return;
    if (d->effect)
        disconnect(d->effect, SIGNAL(effectChanged()), this, SLOT(update()));
    d->effect = value;
    if (d->effect)
        connect(d->effect, SIGNAL(effectChanged()), this, SLOT(update()));
    emit effectChanged();
    update();
}

QmlList<Item3d *> *Item3d::children()
{
    return &(d->children);
}

QmlList<QObject *> *Item3d::resources()
{
    return &(d->resources);
}

QmlList<QObject *> *Item3d::data()
{
    return &(d->data);
}

QmlList<QmlState *>* Item3d::states()
{
    return d->states()->statesProperty();
}

QmlState *Item3d::findState(const QString &name) const
{
    if (!d->_stateGroup)
        return 0;
    else
        return d->_stateGroup->findState(name);
}

QmlList<QmlTransition *>* Item3d::transitions()
{
    return d->states()->transitionsProperty();
}

QString Item3d::state() const
{
    if (!d->_stateGroup)
        return QString();
    else
        return d->_stateGroup->state();
}

void Item3d::setState(const QString &state)
{
    d->states()->setState(state);
}

Item3d::CullFaces Item3d::cullFaces() const
{
    return d->cullFaces;
}

void Item3d::setCullFaces(Item3d::CullFaces value)
{
    if (d->cullFaces != value) {
        d->cullFaces = value;
        emit meshChanged();
    }
}

void Item3d::draw(QGLPainter *painter)
{
    int prevId = painter->objectPickId();
    painter->setObjectPickId(d->objectPickId);
    QObjectList list = QObject::children();
    bool haveLights = false;
    foreach (QObject *child, list) {
        QGLLightParameters *light = qobject_cast<QGLLightParameters *>(child);
        if (light) {
            painter->setLightParameters(0, light); // XXX - non-zero lights
            painter->setLightEnabled(0, true);
            haveLights = true;
        }
    }
    painter->setCullFaces((QGL::CullFaces)(int)(d->cullFaces));
    if (d->effect)
        d->effect->enableEffect(painter);
    painter->modelViewMatrix().push();
    painter->modelViewMatrix().translate(d->position);
    if (!d->transforms.isEmpty()) {
        // The transformations are applied in reverse order of their
        // lexical appearance in the QML file.
        QMatrix4x4 m = painter->modelViewMatrix();
        for (int index = d->transforms.size() - 1; index >= 0; --index)
            d->transforms[index]->applyTo(&m);
        painter->modelViewMatrix() = m;
    }
    if (d->scale != 1.0f)
        painter->modelViewMatrix().scale(d->scale);
    drawItem(painter);
    foreach (QObject *child, list) {
        Item3d *item = qobject_cast<Item3d *>(child);
        if (item)
            item->draw(painter);
    }
    painter->modelViewMatrix().pop();
    if (d->effect)
        d->effect->disableEffect(painter);
    if (d->cullFaces != CullDisabled)
        painter->setCullFaces(QGL::CullDisabled);
    if (haveLights) {
        foreach (QObject *child, list) {
            QGLLightParameters *light = qobject_cast<QGLLightParameters *>(child);
            if (light) {
                painter->setLightParameters(0, 0);  // XXX - non-zero lights
                painter->setLightEnabled(0, false);
            }
        }
    }
    painter->setObjectPickId(prevId);
}

void Item3d::initialize(Viewport *viewport, QGLPainter *painter)
{
    d->viewport = viewport;
    Qml3dView *view = viewport->view();
    if (view) {
        d->objectPickId = view->nextPickId();
        view->registerObject(d->objectPickId, this);
    }
    QObjectList list = QObject::children();
    foreach (QObject *child, list) {
        Item3d *item = qobject_cast<Item3d *>(child);
        if (item)
            item->initialize(viewport, painter);
    }
}

void Item3d::update()
{
    if (d->viewport)
        d->viewport->update3d();
}

void Item3d::componentComplete()
{
    d->states()->componentComplete();
}

void Item3d::drawItem(QGLPainter *painter)
{
    if (d->mesh)
        d->mesh->draw(painter);
}

bool Item3d::event(QEvent *e)
{
    // Convert the raw event into a signal representing the user's action.
    if (e->type() == QEvent::MouseButtonPress) {
        QMouseEvent *me = (QMouseEvent *)e;
        if (me->button() == Qt::LeftButton)
            emit pressed();
    } else if (e->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *me = (QMouseEvent *)e;
        if (me->button() == Qt::LeftButton) {
            emit released();
            if (me->x() >= 0)   // Positive: inside object, Negative: outside.
                emit clicked();
        }
    } else if (e->type() == QEvent::MouseButtonDblClick) {
        emit doubleClicked();
    } else if (e->type() == QEvent::Enter) {
        emit hoverEnter();
    } else if (e->type() == QEvent::Leave) {
        emit hoverLeave();
    }
    return QObject::event(e);
}

void Item3dPrivate::data_removeAt(int)
{
    // ###
}

int Item3dPrivate::data_count() const
{
    // ###
    return 0;
}

void Item3dPrivate::data_append(QObject *o)
{
    Item3d *i = qobject_cast<Item3d *>(o);
    if (i)
        item->children()->append(i);
    else
        resources_append(o);
}

void Item3dPrivate::data_insert(int, QObject *)
{
    // ###
}

QObject *Item3dPrivate::data_at(int) const
{
    // ###
    return 0;
}

void Item3dPrivate::data_clear()
{
    // ###
}

void Item3dPrivate::resources_removeAt(int)
{
    // ###
}

int Item3dPrivate::resources_count() const
{
    // ###
    return 0;
}

void Item3dPrivate::resources_append(QObject *o)
{
    o->setParent(item);
}

void Item3dPrivate::resources_insert(int, QObject *)
{
    // ###
}

QObject *Item3dPrivate::resources_at(int) const
{
    // ###
    return 0;
}

void Item3dPrivate::resources_clear()
{
    // ###
}

void Item3dPrivate::children_removeAt(int)
{
    // ###
}

int Item3dPrivate::children_count() const
{
    // ###
    return 0;
}

void Item3dPrivate::children_append(Item3d *i)
{
    i->setParent(item);

    // Proxy the mouse event signals to the parent so that
    // the parent can trap the signal for a group of children.
    // This isn't ideal: if the child traps the signal, the
    // parent probably shouldn't be notified.
    QObject::connect(i, SIGNAL(clicked()), item, SIGNAL(clicked()));
    QObject::connect(i, SIGNAL(doubleClicked()), item, SIGNAL(doubleClicked()));
    QObject::connect(i, SIGNAL(pressed()), item, SIGNAL(pressed()));
    QObject::connect(i, SIGNAL(released()), item, SIGNAL(released()));
    QObject::connect(i, SIGNAL(hoverEnter()), item, SIGNAL(hoverEnter()));
    QObject::connect(i, SIGNAL(hoverLeave()), item, SIGNAL(hoverLeave()));
}

void Item3dPrivate::children_insert(int, Item3d *)
{
    // ###
}

Item3d *Item3dPrivate::children_at(int) const
{
    // ###
    return 0;
}

void Item3dPrivate::children_clear()
{
    // ###
}

QmlStateGroup *Item3dPrivate::states()
{
    if (!_stateGroup) {
        _stateGroup = new QmlStateGroup(item);
        _stateGroup->classBegin();
        QObject::connect(_stateGroup, SIGNAL(stateChanged(QString)),
                         item, SIGNAL(stateChanged(QString)));
    }

    return _stateGroup;
}

QT_END_NAMESPACE
