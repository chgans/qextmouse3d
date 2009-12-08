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

#include "qglscenenode.h"
#include "qglpainter.h"
#include <QtGui/qmatrix4x4.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLSceneNode
    \brief The QGLSceneNode class defines a geometry node in a QGLAbstractScene.
    \ingroup qt3d
    \ingroup qt3d::scene

    QGLSceneNode is a convenience class that makes it easier for
    applications to extract the interesting properties of a mesh or
    other geometry and manipulate them as part of a scene.

    Also multiple QGLSceneNodes can reference the same geometry, whilst
    applying different transformations and treatments to it.

    As a QGLSceneObject, QGLSceneNode provides a draw() implementation.  This
    default implementation does the following:
    \list
    \i ensures the effect specified by effect() is current on the painter
    \i applies any local transformation that may be set for this node
    \i calls draw() for all the child nodes
    \i calls draw() on this nodes QGLGeometry object (if any)
    \i restores the model-view matrix if any local transform was applied
    \endlist

    Note that the draw() method does \bold not restore the effect.  If the first
    step above results in a change to the current QGL::Standard effect then it
    will remain set to that effect.  In general any painting method should
    ensure the effect it requires is current.

    \sa QGLAbstractScene
*/

class QGLSceneNodePrivate
{
public:
    QGLSceneNodePrivate()
        : geometry(0)
        , localEffect(QGL::LitMaterial)
        , hasEffect(false)
		, isVisible(true)
    {
		
    }

    QGLGeometry *geometry;
    QMatrix4x4 localTransform;
	QMatrix4x4 userTransform;
    QGL::StandardEffect localEffect;
    bool hasEffect;
	bool isVisible;
};

/*!
    Constructs a new scene node and attaches it to \a parent.
*/
QGLSceneNode::QGLSceneNode(QObject *parent)
    : QGLSceneObject(QGLSceneObject::Mesh, parent)
{
    d_ptr = new QGLSceneNodePrivate();
}

/*!
    Constructs a new scene node referencing \a geometry and attaches it to \a parent.
*/
QGLSceneNode::QGLSceneNode(QGLGeometry *geometry, QObject *parent)
    : QGLSceneObject(QGLSceneObject::Mesh, parent)
{
    d_ptr = new QGLSceneNodePrivate();
    d_ptr->geometry = geometry;
}

/*!
    Destroys this scene node.
*/
QGLSceneNode::~QGLSceneNode()
{
    delete d_ptr;
}

/*!
    Returns the geometry associated with this node, or NULL if no
    geometry has been associated with it.

    \sa setGeometry()
*/
QGLGeometry *QGLSceneNode::geometry() const
{
    return d_ptr->geometry;
}

/*!
    Sets the geometry associated with this node to be \a geometry.
    Typically the \a geometry will be some type of mesh object.  The
    default implementation of the QGLSceneNode::draw() method will call
    the geometry's draw() method.

    \sa geometry()
*/
void QGLSceneNode::setGeometry(QGLGeometry *geometry)
{
    d_ptr->geometry = geometry;
}

/*!
    Returns the local transform associated with this node.  If no
    local transform has been explicitly set, this method returns a
    QMatrix4x4 set to the identity matrix.

    \sa setLocalTransform()
*/
QMatrix4x4 QGLSceneNode::localTransform() const
{
    return d_ptr->localTransform;
}

/*!
    Sets the local transform associated with this node to be \a transform.
    The default implementation of the QGLSceneNode::draw() method will
    apply this transform to the QGLPainter before drawing any geometry.

    \sa localTransform()
*/
void QGLSceneNode::setLocalTransform(const QMatrix4x4 &transform) const
{
    d_ptr->localTransform = transform;
}

QMatrix4x4 QGLSceneNode::userTransform() const
{
	return d_ptr->userTransform;
}

void QGLSceneNode::setUserTransform(const QMatrix4x4 &transform)
{
        d_ptr->userTransform = transform;
}


/*!
    Returns the local effect associated with this node.  The default value
    is QGL::LitMaterial.  If the value of hasEffect() is false, then this
    the value of effect() is ignored.

    \sa setEffect(), hasEffect()
*/
QGL::StandardEffect QGLSceneNode::effect() const
{
    return d_ptr->localEffect;
}

/*!
    Sets the local effect associated with this node to be \a effect.  hasEffect()
    will return true after calling this method.

    The default implementation of QGLSceneNode::apply() will set this effect
    during initialization of the model.

    The default implementation of the QGLSceneNode::draw() method will
    ensure that \a effect is applied to the QGLPainter before drawing
    any geometry.

    \sa effect(), hasEffect()
*/
void QGLSceneNode::setEffect(QGL::StandardEffect effect)
{
    d_ptr->localEffect = effect;
    d_ptr->hasEffect = true;
}

 /*!
     Returns true if the local effect on this node is enabled, otherwise
     returns false.

     \sa setEffectEnabled(), setEffect()
 */
bool QGLSceneNode::hasEffect() const
{
    return d_ptr->hasEffect;
}

/*!
     Sets whether the current value of effect() will be applied to the GL
     painter prior to drawing to \a enabled.

     \sa setEffect(), effect(), hasEffect()
*/
void QGLSceneNode::setEffectEnabled(bool enabled)
{
    d_ptr->hasEffect = enabled;
}

/*!
    \reimp
*/
void QGLSceneNode::draw(QGLPainter *painter)
{
    if (d_ptr->geometry)
    {
        QBox3D bb = d_ptr->geometry->boundingBox();
        if (bb.isFinite() && !painter->isVisible(bb))
            return;
    }
    if (d_ptr->hasEffect && painter->standardEffect() != d_ptr->localEffect)
        painter->setStandardEffect(d_ptr->localEffect);

    if (!d_ptr->localTransform.isIdentity() || !d_ptr->userTransform.isIdentity())
	{
		 painter->modelViewMatrix().push();
		 if (!d_ptr->localTransform.isIdentity())  painter->modelViewMatrix() *= d_ptr->localTransform;
		 if (!d_ptr->userTransform.isIdentity()) painter->modelViewMatrix() *= d_ptr->userTransform;
	}
		

    QObjectList subNodes = children();
    QObjectList::iterator cit(subNodes.begin());
    for ( ; cit != subNodes.end(); ++cit)
    {
        QGLSceneNode *n = qobject_cast<QGLSceneNode *>(*cit);
        if (n)
            n->draw(painter);
    }
    if (d_ptr->geometry && d_ptr->geometry->drawingMode() != QGL::NoDrawingMode)
        d_ptr->geometry->draw(painter);

    if (!d_ptr->localTransform.isIdentity() || !d_ptr->userTransform.isIdentity())
        painter->modelViewMatrix().pop();
}

/*!
    \reimp
*/
void QGLSceneNode::apply(QGLPainter *painter)
{
    if (d_ptr->hasEffect && painter->standardEffect() != d_ptr->localEffect)
        painter->setStandardEffect(d_ptr->localEffect);
    QObjectList subNodes = children();
    QObjectList::iterator cit(subNodes.begin());
    for ( ; cit != subNodes.end(); ++cit)
    {
        QGLSceneNode *n = qobject_cast<QGLSceneNode *>(*cit);
        if (n)
            n->apply(painter);
    }
}


QT_END_NAMESPACE
