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

#include "qglabstractscene.h"
#include "qglscenenode.h"
#include "qglscenenode_p.h"
#include "qglpicknode.h"
#include "qglpainter.h"
#include "qgeometrydata.h"
#include "qglmaterialcollection.h"
#include "qmatrix4x4.h"

#include <QtGui/qmatrix4x4.h>
#include <QtCore/qthread.h>
#include <QtGui/qapplication.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLSceneNode
    \brief The QGLSceneNode class defines a node in a 3D scene.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::scene
    \ingroup qt3d::qml3d

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
    \o moves the node to the x, y, z position
    \o rotates the node by the rotX, rotY and rotZ rotations
    \o scales the node by the scale factor, in x, y and z directions
    \o applies any local transformation that may be set for this node
    \o sets the nodes material onto the painter, if the material is valid
    \o calls draw() for all the child nodes
    \o calls draw(start, count) on this nodes geometry object (if any)
    \o restores the geometry's original material if it was changed
    \o restores the model-view matrix if any local transform was applied
    \endlist

    Note that the draw() method does \bold not restore the effect.  If the first
    step above results in a change to the current QGL::Standard effect then it
    will remain set to that effect.  In general any painting method should
    ensure the effect it requires is current.

    The way draw is implemented ensures that this nodes effects, materials and
    transformations will apply by default to its child nodes.  Transformations
    are cumulative, but effects and materials override those of any parent node.

    Typically the local transformation matrix is set by the process that
    constructed the node:  in the case of an imported model, it is likely
    to have been specified by the model file.  To make individual changes
    to the location or orientation of this node, use the position and
    rotation properties - modifying the local transformation is an
    advanced usage and undesirable results may be obtained.

    Note that modifying scale can effect lighting calculations due to normals
    so usage of the scale attribute is also advanced.

    Use childNodes() to obtain the list of child nodes, and add and remove
    child nodes by the addNode() and removeNode() methods.  Also if the normal
    QObject child-parent connection is made (via the setParent() function, or by
    passing the scene node to the childs constructor), and the child is itself
    a QGLSceneNode, then addNode() will be called on it.

    A child may be a child multiple times, a child may be under more than one
    parent, and several parents may reference the same child.  There is however
    no protection against cycles, so a child must not be a parent of itself,
    even if indirectly.

    A child node for the purposes of rendering means a child added via the
    addNode() method.  The default QGLSceneNode constructor will check to
    see if its parent is a QGLSceneNode and add itself via the addNode()
    function if it is.

    \bold{Advanced feature:} if there are suspected problems with lighting
    normals, recompile Qt3D with \c{config+=Q_DEBUG_NORMALS}.  This will cause
    QGLSceneNode to display the lighting normals as short lines protruding
    from each vertex, pointing in the direction of the lighting normal.
    This can be very helpful when a few normals are incorrect for some reason.

    \sa QGLAbstractScene
*/

#if 0

void QGLSceneNodePrivate::appendFunc(QDeclarativeListProperty<QGLSceneNode> *list,
                                             QGLSceneNode *node)
{
    QGLSceneNodePrivate *q = reinterpret_cast<QGLSceneNodePrivate*>(list->data);
    q->childNodes.append(node);
    QGLSceneNode *parent = qobject_cast<QGLSceneNode*>(list->object);
    Q_ASSERT(parent);
    parent->childNodesChanged();
}

int QGLSceneNodePrivate::countFunc(QDeclarativeListProperty<QGLSceneNode> *list)
{
    QGLSceneNodePrivate *q = reinterpret_cast<QGLSceneNodePrivate*>(list->data);
    return q->childNodes.count();
}

QGLSceneNode *QGLSceneNodePrivate::atFunc(QDeclarativeListProperty<QGLSceneNode> *list,
                                                       int index)
{
    QGLSceneNodePrivate *q = reinterpret_cast<QGLSceneNodePrivate*>(list->data);
    QGLSceneNode *node = q->childNodes.at(index);
    return node;
}

void QGLSceneNodePrivate::clearFunc(QDeclarativeListProperty<QGLSceneNode> *list)
{
    QGLSceneNodePrivate *q = reinterpret_cast<QGLSceneNodePrivate*>(list->data);
    q->childNodes.clear();
    QGLSceneNode *parent = qobject_cast<QGLSceneNode*>(list->object);
    Q_ASSERT(parent);
    parent->childNodesChanged();
}

#endif

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
QGLSceneNode::QGLSceneNode(const QGeometryData &geometry, QObject *parent)
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
    Returns the geometry associated with this node, or a null QGeometryData
    if no geometry has been associated with it.

    \sa setGeometry()
*/
QGeometryData QGLSceneNode::geometry() const
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
void QGLSceneNode::setGeometry(QGeometryData geometry)
{
    Q_D(QGLSceneNode);
    d->geometry = geometry;
}

/*!
    Returns a bounding box for the portion of the geometry referenced by
    this scene node.  If the value of start() is 0, and count() is the same
    as geometry()->size() then the bounding box will be the same as
    geometry()->boundingBox().  However if the scene node only references
    some part of the geometry, a bounding box for this section is calculated.

    If this scene node has child nodes then the bounding box will be the
    calculated union of the bounding box for this nodes geometry (if any) and
    the bounding boxes of the children.

    The calculated value is cached and returned on subsequent calls, but
    could be expensive to calculate initially.
*/
QBox3D QGLSceneNode::boundingBox() const
{
    Q_D(const QGLSceneNode);
    if (d->boxValid)
        return d->bb;
    d->bb = QBox3D();
    if (d->geometry.count() > 0)
    {
        if (d->start == 0 && (d->count == d->geometry.count() || d->count == 0))
        {
            d->bb = d->geometry.boundingBox();
        }
        else
        {
            QGL::IndexArray indices = d->geometry.indices();
            for (int i = d->start; i < (d->start + d->count); ++i)
            {
                int ix = indices.at(i);
                d->bb.unite(d->geometry.vertex(ix));
            }
        }
    }
    QList<QGLSceneNode*>::const_iterator it = d->childNodes.constBegin();
    for ( ; it != d->childNodes.constEnd(); ++it)
    {
        QGLSceneNode *n = *it;
        QBox3D b = n->boundingBox();
        b.transform(n->transform());
        d->bb.unite(b);
    }
    d->boxValid = true;
    return d->bb;
}

/*!
    \property QGLSceneNode::center
    \brief Returns the coordinates of the center of the portion of the geometry
    referenced by this scene node.

    The center is calculated as the centroid or geometric barycenter
    of the vertices (the average of the vertices).  For a convex hull this
    is guaranteed to be inside the figure.

    If the value of start() is 0, and count() is the same
    as geometry()->size() then the center will be the same as
    geometry()->center().  However if the scene node only references
    some part of the geometry, a center for this part is calculated.

    Note also that the center is relative to the transformed frame of reference
    for this node.  To get the absolute value, add the center and the position.
*/
QVector3D QGLSceneNode::center() const
{
    Q_D(const QGLSceneNode);
    QVector3D center;
    if (d->geometry.count() > 0)
    {
        if (d->start == 0 && (d->count == d->geometry.count() || d->count == 0))
        {
            center = d->geometry.center();
        }
        else
        {
            QGL::IndexArray ix = d->geometry.indices();
            for (int i = d->start; i < (d->start + d->count); ++i)
            {
                center += d->geometry.vertex(ix[i]);
            }
            center /= (float)d->geometry.count();
        }
    }
    return center;
}

// Calculate the resulting matrix from the position, rotation, scale and
// local transform.  Cache the result for future calls.
QMatrix4x4 QGLSceneNode::transform() const
{
    Q_D(const QGLSceneNode);
    if (d->transformValid)
        return d->transform;
    d->transform = QMatrix4x4();
    if (!d->translate.isNull())
        d->transform.translate(d->translate);
    if (!d->rotate.isNull())
    {
        QQuaternion rx = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, d->rotate.x());
        QQuaternion ry = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, d->rotate.y());
        QQuaternion rz = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, d->rotate.z());
        QQuaternion res = rz * ry * rx;
        d->transform.rotate(res);
    }
    if (!d->scale.isNull())
        d->transform.scale(d->scale);
    if (!d->localTransform.isIdentity())
        d->transform *= d->localTransform;
    d->transformValid = true;
    return d->transform;
}

/*!
    Returns the local transform associated with this node.  If no
    local transform has been explicitly set, this method returns a
    QMatrix4x4 set to the identity matrix.

    The local transform is typically set during model loading or
    geometry construction, and is a feature of the geometry.

    In general to change the \l {QGLSceneNode::position}{location}
    or \l {QGLSceneNode::rotation}{orientation} of the node
    use the position() or rotation() properties instead.

    \sa setLocalTransform(), position(), rotation()
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
    if (d->localTransform != transform)
    {
        d->localTransform = transform;
        invalidateTransform();
        d->invalidateParentBoundingBox();
    }
}

/*!
    \property QGLSceneNode::rotation
    \brief The amounts of x, y and z axis rotation for this node.

    The x, y and z axis rotations can also be specified individually as
    seperate properties \l rotX, \l rotY, and \l rotZ

    These values are calculated as seperate axial rotations and then applied
    in the order x, y and then z: this means that a rotation like
    \c{(90.0f, 45.0f, 0.0f)} may not do what is intended, since the 90.0f around
    x will map the y axis onto the z axis.

    \sa rotX(), rotY(), rotZ()
*/
QVector3D QGLSceneNode::rotation() const
{
    Q_D(const QGLSceneNode);
    return d->rotate;
}

void QGLSceneNode::setRotation(const QVector3D &r)
{
    Q_D(QGLSceneNode);
    if (r != d->rotate)
    {
        d->rotate = r;
        emit rotationChanged();
        invalidateTransform();
    }
}

/*!
    \property QGLSceneNode::rotX
    \brief The amount of x axis rotation for this node.

    \sa rotation()
*/
qreal QGLSceneNode::rotX() const
{
    Q_D(const QGLSceneNode);
    return d->rotate.x();
}

void QGLSceneNode::setRotX(qreal rx)
{
    Q_D(QGLSceneNode);
    if (rx != d->rotate.x())
    {
        d->rotate.setX(rx);
        emit rotationChanged();
        invalidateTransform();
    }
}

/*!
    \property QGLSceneNode::rotY
    \brief The amount of y axis rotation for this node.

    \sa rotation()
*/
qreal QGLSceneNode::rotY() const
{
    Q_D(const QGLSceneNode);
    return d->rotate.y();
}

void QGLSceneNode::setRotY(qreal ry)
{
    Q_D(QGLSceneNode);
    if (d->rotate.y() != ry)
    {
        d->rotate.setY(ry);
        emit rotationChanged();
        invalidateTransform();
    }
}

/*!
    \property QGLSceneNode::rotZ
    \brief The amount of z axis rotation for this node.

    \sa rotation()
*/
qreal QGLSceneNode::rotZ() const
{
    Q_D(const QGLSceneNode);
    return d->rotate.z();
}

void QGLSceneNode::setRotZ(qreal rz)
{
    Q_D(QGLSceneNode);
    if (d->rotate.z() != rz)
    {
        d->rotate.setZ(rz);
        emit rotationChanged();
        invalidateTransform();
    }
}

/*!
    \property QGLSceneNode::position
    \brief The amounts of x, y and z axis translation for this node.

    Since most nodes are situated relative to \c{(0, 0, 0)} when imported as
    part of a model or constructed programatically, the translation is
    effectively the position of the model in the scene.

    The x, y and z axis translations can also be specified individually as
    seperate properties \l x, \l y, and \l z

    \sa rotX(), rotY(), rotZ()
*/
QVector3D QGLSceneNode::position() const
{
    Q_D(const QGLSceneNode);
    return d->translate;
}

void QGLSceneNode::setPosition(const QVector3D &p)
{
    Q_D(QGLSceneNode);
    if (p != d->translate)
    {
        d->translate = p;
        emit positionChanged();
        invalidateTransform();
    }
}

/*!
    \property QGLSceneNode::x
    \brief The amount of x axis translation for this node.

    \sa position()
*/
qreal QGLSceneNode::x() const
{
    Q_D(const QGLSceneNode);
    return d->translate.x();
}

void QGLSceneNode::setX(qreal x)
{
    Q_D(QGLSceneNode);
    if (x != d->translate.x())
    {
        d->translate.setX(x);
        emit positionChanged();
        invalidateTransform();
    }
}

/*!
    \property QGLSceneNode::y
    \brief The amount of y axis translation for this node.

    \sa position()
*/
qreal QGLSceneNode::y() const
{
    Q_D(const QGLSceneNode);
    return d->translate.y();
}

void QGLSceneNode::setY(qreal y)
{
    Q_D(QGLSceneNode);
    if (y != d->translate.y())
    {
        d->translate.setY(y);
        emit positionChanged();
        invalidateTransform();
    }
}

/*!
    \property QGLSceneNode::z
    \brief The amount of z axis translation for this node.

    \sa position()
*/
qreal QGLSceneNode::z() const
{
    Q_D(const QGLSceneNode);
    return d->translate.y();
}

void QGLSceneNode::setZ(qreal z)
{
    Q_D(QGLSceneNode);
    if (z != d->translate.z())
    {
        d->translate.setZ(z);
        emit positionChanged();
        invalidateTransform();
    }
}

/*!
    \property QGLSceneNode::scale
    \brief The amounts of x, y and z axis scaling for this node.

    Use of the scale property is more advanced than both the position()
    and rotation() properties, since scale changes effect lighting normals
    and change the magnitude of other transformations.

    \sa position(), rotation()
*/
QVector3D QGLSceneNode::scale() const
{
    Q_D(const QGLSceneNode);
    return d->scale;
}

void QGLSceneNode::setScale(const QVector3D &scale)
{
    Q_D(QGLSceneNode);
    if (scale != d->scale)
    {
        d->scale = scale;
        emit scaleChanged();
        invalidateTransform();
    }
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
    \property QGLSceneNode::start
    \brief The start index for this scene node.

    By default this value is zero meaning that this node references all
    vertices from the 0'th logical vertex in the underlying geometry.

    \sa count()
*/
int QGLSceneNode::start() const
{
    Q_D(const QGLSceneNode);
    return d->start;
}

void QGLSceneNode::setStart(int start)
{
    Q_D(QGLSceneNode);
    if (start != d->start)
    {
        d->start = start;
        emit startChanged();
        invalidateBoundingBox();
    }
}

/*!
    \property QGLSceneNode::count
    \brief The count of vertices referenced for this scene node.

    By default this value is zero meaning that this node references all
    vertices up to the last logical vertex in the underlying geometry.

    \sa start()
*/
int QGLSceneNode::count() const
{
    Q_D(const QGLSceneNode);
    return d->count;
}

void QGLSceneNode::setCount(int count)
{
    Q_D(QGLSceneNode);
    if (count != d->count)
    {
        d->count = count;
        emit countChanged();
        invalidateBoundingBox();
    }
}

/*!
    Returns the material index for this scene node.

    \sa setMaterial()
*/
int QGLSceneNode::materialIndex() const
{
    Q_D(const QGLSceneNode);
    return d->material;
}

/*!
    Sets the material index for this scene node to \a material.

    \sa material()
*/
void QGLSceneNode::setMaterialIndex(int material)
{
    Q_D(QGLSceneNode);
    d->material = material;
    emit materialChanged();
}

/*!
    \property QGLSceneNode::material
    \brief This property is a pointer to the QGLMaterial instance for this scene node.

    Getting this property is exactly equivalent to \c{palette()->material(materialIndex())}.

    Setting this property causes the material if not already in this nodes palette to be
    added, and then the corresponding index to be set for this scene node.

    Setting this property is exactly equivalent to:
    \code
    int index = d->palette->indexOf(material);
    if (index == -1)
        index = d->palette->addMaterial(material);
    setMaterialIndex(ix);
    \endcode

    \sa materialIndex(), setMaterialIndex()
*/
QGLMaterial *QGLSceneNode::material() const
{
    Q_D(const QGLSceneNode);
    return d->palette->material(d->material);
}

/*!
    Sets the \a material instance for this scene node.

*/
void QGLSceneNode::setMaterial(QGLMaterial *material)
{
    Q_D(QGLSceneNode);
    int ix = d->palette->indexOf(material);
    if (ix == -1)
        ix = d->palette->addMaterial(material);
    setMaterialIndex(ix);
}

/*!
    Returns the palette of materials used by this scene node, or NULL
    if no palette has been set.

    \sa setPalette()
*/
QGLMaterialCollection *QGLSceneNode::palette() const
{
    Q_D(const QGLSceneNode);
    return d->palette;
}

/*!
    Sets the palette of materials for this scene node to \a palette.

    \sa palette()
*/
void QGLSceneNode::setPalette(QGLMaterialCollection *palette)
{
    Q_D(QGLSceneNode);
    d->palette = palette;
}

/*!
    Returns a list of the child nodes for this node.  This list is not
    recursively generated, it includes only the nodes which are
    immediate children of this node.

    \sa allChildren()
*/
QList<QGLSceneNode*> QGLSceneNode::childNodeList() const
{
    Q_D(const QGLSceneNode);
    return d->childNodes;
}

/*!
    Returns a list including recursively all child nodes under
    this node.  Each child node only appears once, even if it is included
    multiple times in the scene graph.
*/
QList<QGLSceneNode*> QGLSceneNode::allChildren() const
{
    Q_D(const QGLSceneNode);
    QList<QGLSceneNode*> allSceneNodes;
    QList<QGLSceneNode*> gather;
    QList<QGLSceneNode*>::const_iterator it = d->childNodes.constBegin();
    for ( ; it != d->childNodes.constEnd(); ++it)
        if (!gather.contains(*it))
            gather.append(*it);
    while (gather.count() > 0)
    {
        QGLSceneNode *node = gather.takeFirst();
        if (!allSceneNodes.contains(node))
        {
            allSceneNodes.append(node);
            gather.append(node->childNodeList());
        }
    }
    return allSceneNodes;
}

/*!
    Sets the list of child nodes for this node to be \a children.
*/
void QGLSceneNode::setChildNodeList(const QList<QGLSceneNode*> &children)
{
    Q_D(QGLSceneNode);
    invalidateBoundingBox();
    QList<QGLSceneNode*>::iterator it = d->childNodes.begin();
    for ( ; it != d->childNodes.end(); ++it)
    {
        QGLSceneNode *node = *it;
        node->d_func()->parentNodes.removeOne(this);
    }
    d->childNodes = children;
    emit childNodesChanged();
}

#if 0

/*!
    \property QGLSceneNode::childNodes
    \brief A list of all child nodes directly under this node.

    Note that this is not a recursively generated list, just the first level
    children under this node.
*/
QDeclarativeListProperty<QGLSceneNode> QGLSceneNode::childNodes()
{
    return QDeclarativeListProperty<QGLSceneNode>(this, 0,
                                                  QGLSceneNodePrivate::appendFunc,
                                                  QGLSceneNodePrivate::countFunc,
                                                  QGLSceneNodePrivate::atFunc,
                                                  QGLSceneNodePrivate::clearFunc);
}

#endif

/*!
    Adds the \a node to the list of child nodes for this node.

    Adding a the same child node more than once is not supported, and will
    lead to undefined results.

    It usually makes no sense to add a node as a child to another node
    more than once, since it would appear in the same place and overdraw
    with no perceptible result.

    If the aim is to have the same geometry displayed several times under a
    given node, each time with different transformations, use the clone()
    call to create copies of the node and then apply the transformations to
    the copies.

    \sa removeNode(), clone()
*/
void QGLSceneNode::addNode(QGLSceneNode *node)
{
    Q_D(QGLSceneNode);
    invalidateBoundingBox();
    d->childNodes.append(node);
    node->d_func()->parentNodes.append(this);
    connect(node, SIGNAL(destroyed(QObject*)), this, SLOT(deleteChild(QObject*)));
    emit childNodesChanged();
}

/*!
    Removes the child node matching \a node.

    \sa addNode()
*/
void QGLSceneNode::removeNode(QGLSceneNode *node)
{
    Q_D(QGLSceneNode);
    // we always remove the correct node here because each node is unique
    // under a given parent due to the requirement that no node be addNode()'ed
    // more than once.
    d->childNodes.removeOne(node);
    node->d_func()->parentNodes.removeOne(this);
    emit childNodesChanged();
    invalidateBoundingBox();
}

void QGLSceneNode::deleteChild(QObject *object)
{
    removeNode(reinterpret_cast<QGLSceneNode*>(object));
}

void QGLSceneNode::invalidateBoundingBox() const
{
    Q_D(const QGLSceneNode);
    d->boxValid = false;
    d->invalidateParentBoundingBox();
}

void QGLSceneNode::invalidateTransform() const
{
    Q_D(const QGLSceneNode);
    d->transformValid = false;
    d->invalidateParentBoundingBox();
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
    else 
    {
        //If the parent wasn't a scene node, then usually we would expect it to be an 
        //abstract scene - failing this we should probably warn the user, as in some
        //instances it may indicate an incorrect assignment (there is a slight overhead
        //to this, so this is usually useful for debugging, and may be removed later).
        QGLAbstractScene *abstractScene = qobject_cast<QGLAbstractScene*>(parent);
        if (!abstractScene)
            qWarning("Warning: QGLSceneNode::setParent was unable to find a valid parent Scene Node or Scene to add the new node to.");
    }
        
    //In all cases perform a normal QObject parent assignment.
    QObject::setParent(parent);
}

/*!
    \reimp
*/
void QGLSceneNode::draw(QGLPainter *painter)
{
    Q_D(QGLSceneNode);
    bool wasTransformed = false;

    QMatrix4x4 m = transform();

    if (!m.isIdentity())
    {
         painter->modelViewMatrix().push();
         painter->modelViewMatrix() *= m;
         wasTransformed = true;
    }

    // If this node only references a small section of the geometry, then
    // this bounding-box test may draw something that didn't need to be.
    QBox3D bb = boundingBox();
    if (bb.isFinite() && !bb.isNull() && painter->isCullable(bb))
    {
        if (wasTransformed)
            painter->modelViewMatrix().pop();
        return;
    }

    if (d->hasEffect && !painter->isPicking())
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

    const QGLMaterial *saveMat = 0;
    bool changedTex = false;
    if (d->palette && d->material != -1 && !painter->isPicking())
    {
        QGLMaterial *mat = d->palette->material(d->material);
        if (painter->faceMaterial(QGL::FrontFaces) != mat)
        {
            saveMat = painter->faceMaterial(QGL::FrontFaces);
            painter->setFaceMaterial(QGL::FrontFaces, mat);
            int texUnit = 0;
            for (int i = 0; i < mat->textureLayerCount(); ++i)
            {
                QGLTexture2D *tex = mat->texture(i);
                if (tex)
                {
                    painter->setTexture(texUnit, tex);
                    changedTex = true;
                    ++texUnit;
                }
            }
        }
    }

    QList<QGLSceneNode*>::iterator cit = d->childNodes.begin();
    for ( ; cit != d->childNodes.end(); ++cit)
        (*cit)->draw(painter);

    if (d->count && d->geometry.count() > 0)
    {
        bool idSaved = false;
        int id = -1;
        if (d->pickNode && painter->isPicking())
        {
            idSaved = true;
            id = painter->objectPickId();
            painter->setObjectPickId(d->pickNode->id());
        }

        d->geometry.draw(painter, d->start, d->count);

        if (idSaved)
            painter->setObjectPickId(id);

        if (d->viewNormals)
        {
            QVector3DArray verts;
            QGL::IndexArray indices = d->geometry.indices();
            for (int i = d->start; i < (d->start + d->count); ++i)
            {
                int ix = indices[i];
                QVector3D a = d->geometry.vertex(ix);
                QVector3D b = a + d->geometry.normal(ix);
                verts.append(a, b);
            }
            painter->setVertexAttribute(QGL::Position, QGLAttributeValue(verts));
            glLineWidth(2.0f);
            painter->draw(QGL::Lines, verts.size());
        }
    }

    if (saveMat)
    {
        painter->setFaceMaterial(QGL::FrontFaces, saveMat);
        if (changedTex)
            painter->setTexture((QGLTexture2D*)0);
    }

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
    \reimp
*/
QGLPickNode *QGLSceneNode::pickNode() const
{
    Q_D(const QGLSceneNode);
    return d->pickNode;
}

/*!
    \reimp
*/
void QGLSceneNode::setPickNode(QGLPickNode *node)
{
    Q_D(QGLSceneNode);
    // TODO - resolve recursive picking - not supported by
    // color based pick AFAICT
    d->pickNode = node;
    node->setTarget(this);
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
    node->setPalette(d->palette);
    node->setLocalTransform(d->localTransform);
    node->setPosition(d->translate);
    node->setRotation(d->rotate);
    node->setScale(d->scale);
    node->setEffect(d->localEffect);
    node->setUserEffect(d->customEffect);
    node->setEffectEnabled(d->hasEffect);
    node->setMaterialIndex(d->material);
    node->setStart(d->start);
    node->setCount(d->count);
    node->setChildNodeList(d->childNodes);
    node->d_func()->bb = d->bb;
    node->d_func()->boxValid = d->boxValid;
    return node;
}

/*!
    Creates a new QGLSceneNode that is a copy of this scene node, with
    \a parent as the parent of the new copy.  If parent is NULL then parent
    is set to this nodes parent.

    The copy will reference the same underlying geometry and
    have all effects, transforms and other properties copied from this node.

    The copy returned will have the same child nodes, except all child nodes
    whose objectName() is equal to \a name are removed.

    \sa clone(), only()
*/
QGLSceneNode *QGLSceneNode::allExcept(const QString &name, QObject *parent) const
{
    QGLSceneNode *n = clone(parent);
    QList<QGLSceneNode*> ch = n->childNodeList();
    for (int i = 0; i < ch.count(); ++i)
    {
        QGLSceneNode *kn = ch.at(i);
        if (kn->objectName() == name)
            n->removeNode(kn);
    }
    return n;
}

/*!
    Creates a new QGLSceneNode that is a copy of this scene node, with
    \a parent as the parent of the new copy.  If parent is NULL then parent
    is set to this nodes parent.

    The copy will reference the same underlying geometry and
    have all effects, transforms and other properties copied from this node.

    The copy returned will have only one child node.  This child node will be
    the first child node of this one which has its objectName() equal to \a name.

    \sa clone(), allExcept()
*/
QGLSceneNode *QGLSceneNode::only(const QString &name, QObject *parent) const
{
    QGLSceneNode *n = clone(parent);
    QList<QGLSceneNode*> ch = n->childNodeList();
    QList<QGLSceneNode*> result;
    for (int i = 0; i < ch.count(); ++i)
    {
        QGLSceneNode *kn = ch.at(i);
        if (kn->objectName() == name)
        {
            result.append(kn);
            break;
        }
    }
    n->setChildNodeList(result);
    return n;
}

/*!
    Creates a new QGLSceneNode that is a copy of this scene node, with
    \a parent as the parent of the new copy.  If parent is NULL then parent
    is set to this nodes parent.

    The copy will reference the same underlying geometry and
    have all effects, transforms and other properties copied from this node.

    The copy returned will have the same child nodes, except all child nodes
    whose objectName() is in the list of \a names are removed.

    \sa clone(), only()
*/
QGLSceneNode *QGLSceneNode::allExcept(const QStringList &names, QObject *parent) const
{
    QGLSceneNode *n = clone(parent);
    QList<QGLSceneNode*> ch = n->childNodeList();
    QSet<QString> chk = QSet<QString>::fromList(names);
    for (int i = 0; i < ch.count(); ++i)
    {
        QGLSceneNode *kn = ch.at(i);
        if (chk.contains(kn->objectName()))
            n->removeNode(kn);
    }
    return n;
}

/*!
    Creates a new QGLSceneNode that is a copy of this scene node, with
    \a parent as the parent of the new copy.  If parent is NULL then parent
    is set to this nodes parent.

    The copy will reference the same underlying geometry and
    have all effects, transforms and other properties copied from this node.

    The copy returned will have only the child nodes from this
    whose objectName() is in the list of \a names are removed.

    \sa clone(), allExcept()
*/
QGLSceneNode *QGLSceneNode::only(const QStringList &names, QObject *parent) const
{
    QGLSceneNode *n = clone(parent);
    QList<QGLSceneNode*> ch = n->childNodeList();
    QList<QGLSceneNode*> result;
    QSet<QString> chk = QSet<QString>::fromList(names);
    for (int i = 0; i < ch.count(); ++i)
    {
        QGLSceneNode *kn = ch.at(i);
        if (chk.contains(kn->objectName()))
            result.append(kn);
    }
    n->setChildNodeList(result);
    return n;
}

/*!
    If \a enabled is true, the viewing of normals on this scene
    nodes geometry is turned on; otherwise it is turned off.  The
    normal view is an advanced feature for use when inspecting and
    debugging models or geometry in a scene.  The lighting normals
    are displayed as a straight line from the vertex pointing in
    the direction of the lighting normal.  This is useful for
    example to show where normals are inverted or wrongly
    calculated.

    The setting of this flag is not propagated to child nodes,
    instead set the flag to true for the node or nodes where its
    needed.

    To set the flag on all child nodes use code like:
    \code
    foreach(QGLSceneNode *node, scene.allChildren())
        node->setNormalViewEnabled(true);
    \endcode

    \image spiky-teapot.png

    \sa normalViewEnabled()
*/
void QGLSceneNode::setNormalViewEnabled(bool enabled)
{
    Q_D(QGLSceneNode);
    d->viewNormals = enabled;
}

/*!
    Returns true if the viewing of normals on this scene nodes geometry
    is turned on; false otherwise.  See setNormalViewEnabled() for more
    details.

    \sa setNormalViewEnabled()
*/
bool QGLSceneNode::normalViewEnabled() const
{
    Q_D(const QGLSceneNode);
    return d->viewNormals;
}

/*!
    \fn QGLSceneNode::childNodesChanged()
    Signals that the childNodes() property for this scene node has changed.
*/

/*!
    \fn QGLSceneNode::positionChanged()
    Signals that the position() property for this scene node has changed.
*/

/*!
    \fn QGLSceneNode::rotationChanged()
    Signals that the rotation() property for this scene node has changed.
*/

/*!
    \fn QGLSceneNode::scaleChanged()
    Signals that the count() property for this scene node has changed.
*/

/*!
    \fn QGLSceneNode::materialChanged()
    Signals that the material() property for this scene node has changed.
*/

/*!
    \fn QGLSceneNode::countChanged()
    Signals that the count() property for this scene node has changed.
*/

/*!
    \fn QGLSceneNode::startChanged()
    Signals that the count() property for this scene node has changed.
*/

#ifndef QT_NO_DEBUG_STREAM
#include "qglmaterialcollection.h"
#include "qgltexture2d.h"
/*!
    \relates QGLSceneNode
    Print a description of \a node, and all its descendants, to stderr.  Only
    available when compiled in debug mode (without QT_NO_DEBUG defined).
    The \a indent and \a loop parameters are used internally.
*/
void qDumpScene(QGLSceneNode *node, int indent, const QSet<QGLSceneNode *> &loop)
{
    QThread *appThread = QApplication::instance()->thread();
    QSet<QGLSceneNode *> lp = loop;
    lp.insert(node);
    QString ind;
    ind.fill(' ', indent * 4);
    fprintf(stderr, "\n%s ======== Node: %p - %s =========\n", qPrintable(ind), node,
            qPrintable(node->objectName()));
    if (appThread != node->thread())
        fprintf(stderr, "\n%s        from thread: %p\n", qPrintable(ind), node->thread());
    fprintf(stderr, "%s start: %d   count: %d   children:", qPrintable(ind), node->start(), node->count());
    {
        QList<QGLSceneNode*> children = node->childNodeList();
        QList<QGLSceneNode*>::const_iterator it = children.constBegin();
        for (int i = 0; it != children.constEnd(); ++it, ++i)
            fprintf(stderr, "%d: %p  ", i, *it);
    }
    fprintf(stderr, "\n");
    if (!node->position().isNull())
    {
        QVector3D p = node->position();
        fprintf(stderr, "%s position: (%0.4f, %0.4f, %0.4f)\n", qPrintable(ind),
                p.x(), p.y(), p.z());
    }
    if (!node->rotation().isNull())
    {
        QVector3D r = node->rotation();
        fprintf(stderr, "%s rotation: (%0.4f, %0.4f, %0.4f)\n", qPrintable(ind),
                r.x(), r.y(), r.z());
    }
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
    if (!node->geometry().isNull())
    {
        fprintf(stderr, "%s geometry: %d verts\n", qPrintable(ind), node->geometry().count());
    }
    else
    {
        fprintf(stderr, "%s geometry: NULL\n", qPrintable(ind));
    }
    if (node->materialIndex() != -1)
    {
        fprintf(stderr, "%s material: %d", qPrintable(ind), node->materialIndex());
        QGLMaterial *mat = node->material();
        if (mat)
        {
            if (mat->objectName().isEmpty())
                fprintf(stderr, " -- %p:", mat);
            else
                fprintf(stderr, " -- \"%s\":",
                        qPrintable(mat->objectName()));
            fprintf(stderr, " Amb: %s - Diff: %s - Spec: %s - Shin: %0.2f\n",
                    qPrintable(mat->ambientColor().name()),
                    qPrintable(mat->diffuseColor().name()),
                    qPrintable(mat->specularColor().name()),
                    mat->shininess());
            for (int i = 0; i < mat->textureLayerCount(); ++i)
            {
                if (mat->texture(i) != 0)
                {
                    QGLTexture2D *tex = mat->texture(i);
                    if (tex->objectName().isEmpty())
                        fprintf(stderr, "%s         texture %p", qPrintable(ind), tex);
                    else
                        fprintf(stderr, "%s         texture %s", qPrintable(ind),
                                qPrintable(tex->objectName()));
                    QSize sz = tex->size();
                    fprintf(stderr, " - size: %d (w) x %d (h)\n", sz.width(), sz.height());
                }
            }
        }
    }
    else
    {
        fprintf(stderr, "%s material: NONE\n", qPrintable(ind));
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
    QList<QGLSceneNode*> children = node->childNodeList();
    QList<QGLSceneNode*>::const_iterator it = children.constBegin();
    for ( ; it != children.constEnd(); ++it)
        if (!lp.contains(*it))
            qDumpScene(*it, indent + 1);
}

QDebug operator<<(QDebug dbg, const QGLSceneNode &node)
{
    dbg << &node << "\n    start:" << node.start() << " count:" << node.count();
    QList<QGLSceneNode*> children = node.childNodeList();
    QList<QGLSceneNode*>::const_iterator it = children.constBegin();
    for ( ; it != children.constEnd(); ++it)
        dbg << "\n        child:" << *it;

    if (node.localTransform().isIdentity())
        dbg << "\n    local transform: identity";
    else
        dbg << "\n    local transform:\n" << node.localTransform();

    if (node.geometry().count() > 0)
    {
        QGLMaterial *mat = node.material();
        QString mdesc;
        if (mat)
            mdesc = mat->objectName();
        dbg << "\n    geometry:" << node.geometry();
        dbg << "\n    material" << node.material() << ": " << mat << mdesc;
    }
    else
    {
        dbg << "\n    geometry: NULL";
        dbg << "\n    material" << node.material();
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
