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

#include "qglsceneobject.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLSceneObject
    \brief The QGLSceneObject class represents a 3D object within a scene.
    \ingroup qt3d
    \ingroup qt3d::scene

    \sa QGLAbstractScene
*/

/*!
    \enum QGLSceneObject::Type
    This enum defines the type of scene object represented by a QGLSceneObject instance.

    \value Camera Defines a projection for the scene.
    \value Effect Defines a specific material, shader, or texture.
    \value Light Defines a light in the scene.
    \value Main Defines a main object of interest in the scene.
           There may be multiple main objects.
    \value Mesh Defines a geometry mesh.
    \value World Defines an entire scene world.  World objects are usually
           the top-level, or "root", objects in a scene.
*/

class QGLSceneObjectPrivate
{
public:
    QGLSceneObjectPrivate(QGLSceneObject::Type t)
    {
        type = t;
    }

    QGLSceneObject::Type type;
    QString name;
};

/*!
    Constructs a scene object of the specified \a type and attaches
    it to \a parent.
*/
QGLSceneObject::QGLSceneObject
        (QGLSceneObject::Type type, QObject *parent)
    : QObject(parent)
{
    d_ptr = new QGLSceneObjectPrivate(type);
}

/*!
    Destroys this scene object.
*/
QGLSceneObject::~QGLSceneObject()
{
    delete d_ptr;
}

/*!
    Returns the type of this object.
*/
QGLSceneObject::Type QGLSceneObject::type() const
{
    return d_ptr->type;
}

/*!
    Applies the settings to \a painter that are needed to draw() this object.
    The default implementation does nothing.

    The action that is taken depends upon the type of object.  Cameras and
    lights are typically applied once per scene and then deactivated with
    cleanup() after the entire scene is drawn.  Effects are applied just
    before a geometry mesh is drawn, and then cleaned up afterwards.

    Mesh objects will typically call QGLMatrixStack::push() to save
    the QGLPainter::modelViewMatrix() and then adjust the modelview for
    the object's scale, orientation, and position.  In cleanup(), the
    original modelview is restored with QGLMatrixStack::pop().

    \sa draw(), cleanup()
*/
void QGLSceneObject::apply(QGLPainter *painter)
{
    Q_UNUSED(painter);
}

/*!
    Draws this object to \a painter.  The default implementation does nothing.

    Usually only geometry meshes actually draw anything.  Other object types
    will typically ignore overriding this function and instead override
    apply() and cleanup().

    In the process of drawing a geometry mesh, other objects may called
    upon to apply() effect parameters.  These settings must be restored
    by the time draw() returns, or in cleanup().

    \sa apply(), cleanup()
*/
void QGLSceneObject::draw(QGLPainter *painter)
{
    Q_UNUSED(painter);
}	

/*!
    Cleans up the settings in \a painter that were changed by apply().
    The default implementation does nothing.

    \sa apply(), draw()
*/
void QGLSceneObject::cleanup(QGLPainter *painter)
{
    Q_UNUSED(painter);
}

/*!
    \fn void QGLSceneObject::changed()

    Signal that is emitted when this object changes in a manner that
    requires the scene to be repainted.
*/

QT_END_NAMESPACE
