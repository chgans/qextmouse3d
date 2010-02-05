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

#include "qglscenenode.h"
#include "qglscenenode_p.h"
#include "qglpainter.h"
#include <QtGui/qmatrix4x4.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLSceneNode
    \brief The QGLSceneNode class defines a geometry node in a QGLAbstractScene.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::scene

    QGLSceneNode is a convenience class that makes it easier for
    applications to extract the interesting properties of a mesh or
    other geometry and manipulate them as part of a scene.

    Also multiple QGLSceneNodes can reference the same geometry, whilst
    applying different transformations and treatments to it.  Since
    QGLSceneNode is a QObject sub class it cannot be copied directly, so
    instead use the clone() function for this purpose.

    A scene node allows referencing into sections of geometry, via the start
    and count properties.

    The start index is an offset into the geometry at which drawing will start.
    The default start index is 0, so that drawing will start from the beginning
    of the geometry.  The count dictates how many vertices will be drawn.  The
    default count is 0, which instructs the underlying geometry to draw all
    vertices.

    Also a node may have a local material.  This allows drawing the same geometry
    with different materials (which includes different textures).

    As a QGLSceneObject, QGLSceneNode provides a draw() implementation.  This
    default implementation does the following:
    \list
    \o ensures the effect specified by effect() is current on the painter
    \o applies any local transformation that may be set for this node
    \o sets the nodes material onto the geometry, if the material is valid
    \o calls draw() for all the child nodes
    \o calls draw(start, count) on this nodes QGLGeometry object (if any)
    \o restores the geometry's original material if it was changed
    \o restores the model-view matrix if any local transform was applied
    \endlist

    This means that this nodes effects, materials and transformations will
    apply by default to its child nodes.  Transformations are cumulative,
    but effects and materials override those of any parent node.

    Use childNodes() to obtain the list of child nodes, and add and remove
    child nodes by the addNode() and removeNode() methods.

    A child may be a child multiple times, that is multiple copies of the
    node may exist; a child may be under more than one parent, and several
    parents may reference the same child.

    A child node for the purposes of rendering means a child added via the
    addNode() method.  The default QGLSceneNode constructor will check to
    see if its parent is a QGLSceneNode and add itself via the addNode()
    function if it is.

    Note that the draw() method does \bold not restore the effect.  If the first
    step above results in a change to the current QGL::Standard effect then it
    will remain set to that effect.  In general any painting method should
    ensure the effect it requires is current.

    \sa QGLAbstractScene
*/


/*!
    Constructs a new scene node and attaches it to \a parent.  If parent is
    a QGLSceneNode then this node is added to it as a child.
*/
QGLSceneNode::QGLSceneNode(QObject *parent)
    : QGLSceneObject(*new QGLSceneNodePrivate(QGLSceneObject::Mesh), parent)
{
    QGLSceneNode *sceneParent = qobject_cast<QGLSceneNode*>(parent);
    if (sceneParent)
        sceneParent->addNode(this);
}

/*!
    Constructs a new scene node referencing \a geometry and attaches it to
    \a parent.    If parent is a QGLSceneNode then this node is added to it
    as a child.
*/
QGLSceneNode::QGLSceneNode(QGLGeometry *geometry, QObject *parent)
    : QGLSceneObject(*new QGLSceneNodePrivate(QGLSceneObject::Mesh), parent)
{
    Q_D(QGLSceneNode);
    d->geometry = geometry;
    QGLSceneNode *sceneParent = qobject_cast<QGLSceneNode*>(parent);
    if (sceneParent)
        sceneParent->addNode(this);
}

/*!
    \internal
    Constructor for use by QObjectPrivate-using subclasses of QGLSceneObject.
    If parent is a QGLSceneNode then this node is added to it
    as a child.
*/
QGLSceneNode::QGLSceneNode(QGLSceneNodePrivate &dd, QObject *parent)
    : QGLSceneObject(dd, parent)
{
    QGLSceneNode *sceneParent = qobject_cast<QGLSceneNode*>(parent);
    if (sceneParent)
        sceneParent->addNode(this);
}


/*!
    Destroys this scene node.
*/
QGLSceneNode::~QGLSceneNode()
{
}

/*!
    Returns the geometry associated with this node, or NULL if no
    geometry has been associated with it.

    \sa setGeometry()
*/
QGLGeometry *QGLSceneNode::geometry() const
{
    Q_D(const QGLSceneNode);
    return d->geometry;
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
    Q_D(QGLSceneNode);
    d->geometry = geometry;
}

/*!
    Returns the local transform associated with this node.  If no
    local transform has been explicitly set, this method returns a
    QMatrix4x4 set to the identity matrix.

    \sa setLocalTransform()
*/
QMatrix4x4 QGLSceneNode::localTransform() const
{
    Q_D(const QGLSceneNode);
    return d->localTransform;
}

/*!
    Sets the local transform associated with this node to be \a transform.
    The default implementation of the QGLSceneNode::draw() method will
    apply this transform to the QGLPainter before drawing any geometry.

    \sa localTransform()
*/
void QGLSceneNode::setLocalTransform(const QMatrix4x4 &transform)
{
    Q_D(QGLSceneNode);
    d->localTransform = transform;
}

/*!
    Returns the local effect associated with this node.  The default value
    is QGL::LitMaterial.  If the value of hasEffect() is false, then this
    the value of effect() is ignored.

    \sa setEffect(), hasEffect()
*/
QGL::StandardEffect QGLSceneNode::effect() const
{
    Q_D(const QGLSceneNode);
    return d->localEffect;
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
    Q_D(QGLSceneNode);
    d->localEffect = effect;
    d->hasEffect = true;
}

/*!
    Returns the user effect associated with this node, or NULL if one is not
    set.  The default value is NULL.  If the value of hasEffect() is false,
    then this effect is ignored.

    \sa setUserEffect(), hasEffect()
*/
QGLAbstractEffect *QGLSceneNode::userEffect() const
{
    Q_D(const QGLSceneNode);
    return d->customEffect;
}

/*!
    Sets the local effect associated with this node to be the custom
    \a effect.  hasEffect() will return true after calling this method.

    This custom effect will supersede any standard effect.

    The default implementation of QGLSceneNode::apply() will set this effect
    during initialization of the model.

    The default implementation of the QGLSceneNode::draw() method will
    ensure that \a effect is applied to the QGLPainter before drawing
    any geometry.

    \sa userEffect(), hasEffect()
*/
void QGLSceneNode::setUserEffect(QGLAbstractEffect *effect)
{
    Q_D(QGLSceneNode);
    d->customEffect = effect;
    d->hasEffect = true;
}


 /*!
     Returns true if the local effect on this node is enabled, otherwise
     returns false.

     \sa setEffectEnabled(), setEffect()
 */
bool QGLSceneNode::hasEffect() const
{
    Q_D(const QGLSceneNode);
    return d->hasEffect;
}

/*!
    Sets whether the current value of effect() or userEffect() will be
    applied to the QGLPainter prior to drawing.  If \a enabled is true,
    then the effect is applied, otherwise it is not.

     \sa setEffect(), effect(), hasEffect()
*/
void QGLSceneNode::setEffectEnabled(bool enabled)
{
    Q_D(QGLSceneNode);
    d->hasEffect = enabled;
}

/*!
    Returns the start index for this scene node.

    \sa setStart()
*/
int QGLSceneNode::start() const
{
    Q_D(const QGLSceneNode);
    return d->start;
}

/*!
    Sets the start index for this scene node to \a start.

    \sa start()
*/
void QGLSceneNode::setStart(int start)
{
    Q_D(QGLSceneNode);
    d->start = start;
}

/*!
    Returns the count of vertices referenced for this scene node.

    \sa setCount()
*/
int QGLSceneNode::count() const
{
    Q_D(const QGLSceneNode);
    return d->count;
}

/*!
    Sets the count of vertices referenced to \a count for this scene node.

    \sa count()
*/
void QGLSceneNode::setCount(int count)
{
    Q_D(QGLSceneNode);
    d->count = count;
}

/*!
    Returns the material index for this scene node.

    \sa setMaterial()
*/
int QGLSceneNode::material() const
{
    Q_D(const QGLSceneNode);
    return d->material;
}

/*!
    Sets the material index for this scene node to \a material.

    \sa material()
*/
void QGLSceneNode::setMaterial(int material)
{
    Q_D(QGLSceneNode);
    d->material = material;
}

/*!
    Returns a list of the child nodes for this node.
*/
QList<QGLSceneNode*> QGLSceneNode::childNodes() const
{
    Q_D(const QGLSceneNode);
    return d->childNodes;
}

/*!
    Sets the list of child nodes for this node to be \a children.
*/
void QGLSceneNode::setChildNodes(const QList<QGLSceneNode*> &children)
{
    Q_D(QGLSceneNode);
    d->childNodes = children;
}

/*!
    Adds the \a node to the list of child nodes for this node.
*/
void QGLSceneNode::addNode(QGLSceneNode *node)
{
    Q_D(QGLSceneNode);
    d->childNodes.append(node);
    connect(node, SIGNAL(destroyed(QObject*)), this, SLOT(deleteChild(QObject*)));
}

/*!
    Removes the first child node matching \a node.
*/
void QGLSceneNode::removeNode(QGLSceneNode *node)
{
    Q_D(QGLSceneNode);
    d->childNodes.removeOne(node);
}

void QGLSceneNode::deleteChild(QObject *object)
{
    removeNode(reinterpret_cast<QGLSceneNode*>(object));
}

/*!
    Sets the \a parent to be the parent of this object.  If \a parent is
    a QGLSceneNode then this node is added to it as a child.

    \sa addNode()
*/
void QGLSceneNode::setParent(QObject *parent)
{
    QGLSceneNode *sceneParent = qobject_cast<QGLSceneNode*>(parent);
    if (sceneParent)
        sceneParent->addNode(this);
    QObject::setParent(parent);
}

/*!
    \reimp
*/
void QGLSceneNode::draw(QGLPainter *painter)
{
    Q_D(QGLSceneNode);
    bool wasTransformed = false;
    if (!d->localTransform.isIdentity())
    {
         painter->modelViewMatrix().push();
         painter->modelViewMatrix() *= d->localTransform;
         wasTransformed = true;
    }

    if (d->geometry)
    {
        // If this node only references a small section of the geometry, then
        // this bounding-box test may draw something that didn't need to be.
        QBox3D bb = d->geometry->boundingBox();
        if (bb.isFinite() && !painter->isVisible(bb))
        {
            if (wasTransformed)
                painter->modelViewMatrix().pop();
            return;
        }
    }
    if (d->hasEffect)
    {
        if (d->customEffect)
        {
            if (painter->userEffect() != d->customEffect)
                painter->setUserEffect(d->customEffect);
        }
        else
        {
            if (painter->standardEffect() != d->localEffect)
                painter->setStandardEffect(d->localEffect);
        }
    }

    int saveMat = -1;
    bool matSaved = false;
    if (d->material != -1)
    {
        saveMat = d->geometry->material();
        matSaved = true;
        d->geometry->setMaterial(d->material);
	}
		
    QList<QGLSceneNode*>::iterator cit = d->childNodes.begin();
    for ( ; cit != d->childNodes.end(); ++cit)
        (*cit)->draw(painter);

    if (d->geometry && d->geometry->drawingMode() != QGL::NoDrawingMode)
    {
        if (d->start == 0 && (d->count == 0 || d->count == d->geometry->indexArray().size()))
            d->geometry->draw(painter);
        else
            d->geometry->draw(painter, d->start, d->count);
    }

    if (matSaved)
        d->geometry->setMaterial(saveMat);

    if (wasTransformed)
        painter->modelViewMatrix().pop();
}

/*!
    \reimp
*/
void QGLSceneNode::apply(QGLPainter *painter)
{
    Q_D(QGLSceneNode);
    if (d->hasEffect && painter->standardEffect() != d->localEffect)
        painter->setStandardEffect(d->localEffect);

    QList<QGLSceneNode*>::iterator cit = d->childNodes.begin();
    for ( ; cit != d->childNodes.end(); ++cit)
        (*cit)->apply(painter);
}

/*!
    Creates a new QGLSceneNode that is a copy of this scene node, with
    \a parent as the parent of the new copy.  If parent is NULL then parent
    is set to this nodes parent.

    The copy will reference the same underlying geometry, child nodes, and
    have all effects, transforms and other properties copied from this node.
*/
QGLSceneNode *QGLSceneNode::clone(QObject *parent) const
{
    Q_D(const QGLSceneNode);
    QGLSceneNode *node = new QGLSceneNode(parent ? parent : this->parent());
    node->setGeometry(d->geometry);
    node->setLocalTransform(d->localTransform);
    node->setEffect(d->localEffect);
    node->setUserEffect(d->customEffect);
    node->setEffectEnabled(d->hasEffect);
    node->setMaterial(d->material);
    node->setStart(d->start);
    node->setCount(d->count);
    node->setChildNodes(d->childNodes);
    return node;
}

#ifndef QT_NO_DEBUG_STREAM
#include "qglmaterialcollection.h"

/*!
    \relates QGLSceneNode
    Print a description of \a node, and all its descendants, to stderr.  Only
    available when compiled in debug mode (without QT_NO_DEBUG defined).
    The \a indent and \a loop parameters are used internally.
*/
void qDumpScene(QGLSceneNode *node, int indent, const QSet<QGLSceneNode *> &loop)
{
    QSet<QGLSceneNode *> lp = loop;
    lp.insert(node);
    QString ind;
    ind.fill(' ', indent * 4);
    fprintf(stderr, "\n%s ======== Node: %p - %s =========\n", qPrintable(ind), node,
            qPrintable(node->objectName()));
    fprintf(stderr, "%s start: %d   count: %d   children:", qPrintable(ind), node->start(), node->count());
    {
        QList<QGLSceneNode*> children = node->childNodes();
        QList<QGLSceneNode*>::const_iterator it = children.constBegin();
        for (int i = 0; it != children.constEnd(); ++it, ++i)
            fprintf(stderr, "%d: %p  ", i, *it);
    }
    fprintf(stderr, "\n");
    if (node->localTransform().isIdentity())
    {
        fprintf(stderr, "%s local transform: identity\n", qPrintable(ind));
    }
    else
    {
        fprintf(stderr, "%s local transform:\n", qPrintable(ind));
        QMatrix4x4 m = node->localTransform();
        for (int i = 0; i < 4; ++i)
            fprintf(stderr, "%s     %0.4f   %0.4f   %0.4f   %0.4f\n",
                    qPrintable(ind), m(i, 0), m(i, 1), m(i, 2), m(i, 3));
    }
    if (node->geometry())
    {
        fprintf(stderr, "%s geometry: %p\n", qPrintable(ind), node->geometry());
        fprintf(stderr, "%s material: %d == %s\n", qPrintable(ind), node->material(),
                qPrintable(node->geometry()->palette()->materialName(node->material())));
    }
    else
    {
        fprintf(stderr, "%s geometry: NULL\n", qPrintable(ind));
        fprintf(stderr, "%s material: %d\n", qPrintable(ind), node->material());
    }

    if (node->hasEffect())
    {
        switch (node->effect())
        {
        case QGL::FlatColor:
            fprintf(stderr, "%s flat color effect\n", qPrintable(ind)); break;
        case QGL::FlatPerVertexColor:
            fprintf(stderr, "%s flat per vertex color effect\n", qPrintable(ind)); break;
        case QGL::FlatReplaceTexture2D:
            fprintf(stderr, "%s flat replace texture 2D effect\n", qPrintable(ind)); break;
        case QGL::FlatDecalTexture2D:
            fprintf(stderr, "%s flat decal texture 2D effect\n", qPrintable(ind)); break;
        case QGL::LitMaterial:
            fprintf(stderr, "%s lit material effect\n", qPrintable(ind)); break;
        case QGL::LitDecalTexture2D:
            fprintf(stderr, "%s lit decal texture 2D effect\n", qPrintable(ind)); break;
        case QGL::LitModulateTexture2D:
            fprintf(stderr, "%s lit modulate texture 2D effect\n", qPrintable(ind)); break;
        }
    }
    else
    {
        fprintf(stderr, "%s no effect set\n", qPrintable(ind));
    }
    QList<QGLSceneNode*> children = node->childNodes();
    QList<QGLSceneNode*>::const_iterator it = children.constBegin();
    for ( ; it != children.constEnd(); ++it)
        if (!lp.contains(*it))
            qDumpScene(*it, indent + 1);
}

QDebug operator<<(QDebug dbg, const QGLSceneNode &node)
{
    dbg << &node << "\n    start:" << node.start() << " count:" << node.count();
    QList<QGLSceneNode*> children = node.childNodes();
    QList<QGLSceneNode*>::const_iterator it = children.constBegin();
    for ( ; it != children.constEnd(); ++it)
        dbg << "\n        child:" << *it;

    if (node.localTransform().isIdentity())
        dbg << "\n    local transform: identity";
    else
        dbg << "\n    local transform:\n" << node.localTransform();

    if (node.geometry())
    {
        dbg << "\n    geometry:" << node.geometry();
        dbg << "\n    material:" << QString("#%1 ==").arg(QString::number(node.material()))
                << node.geometry()->palette()->materialName(node.material());
    }
    else
    {
        dbg << "\n    geometry: NULL";
        dbg << "\n    material:" << QString("#%1").arg(QString::number(node.material()));
    }

    if (node.hasEffect())
    {
        switch (node.effect())
        {
        case QGL::FlatColor:
            dbg << "\n    flat color effect"; break;
        case QGL::FlatPerVertexColor:
            dbg << "\n    flat per vertex color effect"; break;
        case QGL::FlatReplaceTexture2D:
            dbg << "\n    flat replace texture 2D effect"; break;
        case QGL::FlatDecalTexture2D:
            dbg << "\n    flat decal texture 2D effect"; break;
        case QGL::LitMaterial:
            dbg << "\n    lit material effect"; break;
        case QGL::LitDecalTexture2D:
            dbg << "\n    lit decal texture 2D effect"; break;
        case QGL::LitModulateTexture2D:
            dbg << "\n    lit modulate texture 2D effect"; break;
        }
    }
    else
    {
        dbg << "\n    no effect set";
    }
    return dbg;
}

#endif

QT_END_NAMESPACE
