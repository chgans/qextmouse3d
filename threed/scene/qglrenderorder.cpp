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

#include "qglrenderorder.h"

QT_BEGIN_NAMESPACE

/*!
    \class RenderOrderKey
    \brief The RenderOrderKey class encapsulates a state and node to identify an order.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::scene

    The RenderOrderKey class is a simple value structure exposing a QGLSceneNode
    and a related QGLRenderState.  Together these two items uniquely identify a
    a particular rendering operation which must be ordered.

    Create instances of RenderOrderKey in order to specify to a QGLRenderOrderRepository
    which QGLRenderOrder to construct.
*/

/*!
    \fn RenderOrderKey::RenderOrderKey()
    Constructs a new RenderOrderKey with default values.
*/

/*!
    \fn RenderOrderKey::RenderOrderKey(const QGLSceneNode *node, const QGLRenderState &state)
    Constructs a new RenderOrderKey with the given \a node and \a state.
*/

/*!
    \variable RenderOrderKey::node
    \brief This public member variable contains the node for this key.  The default value
    is null.
*/

/*!
    \variable RenderOrderKey::state
    \brief This public member variable contains the state for this key.  The default value is
    an empty (default constructed) render state.
*/

/*!
    \fn bool RenderOrderKey::valid() const
    Returns true if this is a valid key; false otherwise.  The key is valid if the
    node() is non-null.
*/

/*!
    \fn bool RenderOrderKey::operator==(const RenderOrderKey &rhs) const
    Returns true if this is equal to \a rhs, that is if both the node and
    state are equal to that of \a rhs.
*/

/*!
    \class QGLRenderOrder
    \brief The QGLRenderOrder class represents an order of a scene node during rendering.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::scene

    The QGLRenderOrder class works with the QGLRenderSequencer to optimize
    the rendering order of scene nodes.

    The class encapsulates the ordering with which a QGLSceneNode appears in the
    rendering of a scene.

    By default all nodes which have the same effect type are rendered together,
    and then within that, those nodes which have the same material are
    rendered together.

    Custom render orders may be created by sub-classing QGLRenderOrder and
    reimplementing the following methods:
    \list
        \i isEqualTo()
        \i isLessThan()
        \i qHash()
    \endlist

    This QGLRenderOrder, the base class provides default implementations of
    these function to provide the default behaviour mentioned above.

    \section1 Creating Custom Render Orders

    To provide a different render order, for example MyRenderOrder, first create
    a subclass of QGLRenderOrder and reimplement the following methods for the
    new subclass:
    \list
        \i isEqualTo()
        \i isLessThan()
        \i qHash()
    \endlist

    \code
    class MyRenderOrder : public QGLRenderOrder
    {
        bool isEqual(const QGLRenderOrder &rhs) const;
        bool isLessThan(const QGLRenderOrder &rhs) const;
    }

    uint qHash(const QGLRenderOrder &order);
    \endcode

    Then create a subclass of QGLRenderOrderFactory and reimplement newOrder() so
    that it returns an instance of your new QGLRenderOrder, and set an instance
    of your factory onto QGLPainter:

    \code
    void MyView::paintGL(QGLPainter *painter)
    {
        painter->setRenderOrderFactory(m_myRenderOrderFactory);
        complexScene->draw(painter);
    }
    \endcode
*/

/*!
    \fn QGLRenderOrder::QGLRenderOrder(const RenderOrderKey &key)
    Creates a new QGLRenderOrder instance that encapsulates the order in this
    render pass represented by the given \a key.
*/

/*!
    \fn QGLRenderOrder::~QGLRenderOrder()
    Destroys this QGLRenderOrder and recovers any resources.
*/

/*!
    Returns a hash value representing the effect set onto the node for
    this render order.
*/
uint QGLRenderOrder::effectHash() const
{
    quint64 result = 0;
    if (effectiveHasEffect())
    {
        QGLAbstractEffect *eff = effectiveUserEffect();
        if (eff)
            result = reinterpret_cast<quint64>(eff);
        else
            result = effectiveStandardEffect();
        Q_ASSERT(result);
    }
    return qHash(result);
}

/*!
    Returns true if this QGLRenderOrder is equal to the \a rhs, otherwise
    returns false.  Reimplement this function when creating a sub-class of
    QGLRenderOrder.

    \sa isLessThan()
*/
bool QGLRenderOrder::isEqual(const QGLRenderOrder &rhs) const
{
    if (this == &rhs)
        return true;
    bool result = false;
    bool thisHasEffect = effectiveHasEffect();
    bool thatHasEffect = rhs.effectiveHasEffect();
    if (thisHasEffect && thatHasEffect)
    {
        QGLAbstractEffect *eff = effectiveUserEffect();
        if (eff)
            result = (eff == rhs.effectiveUserEffect());
        else
            result = (effectiveStandardEffect() == rhs.effectiveStandardEffect());
    }
    else
    {
        result = (thisHasEffect == thatHasEffect);
    }
    if (result)
    {
        result = (effectiveMaterial() == rhs.effectiveMaterial());
    }
    if (result)
    {
        result = (effectiveBackMaterial() == rhs.effectiveBackMaterial());
    }
    return result;
}

/*!
    Returns true if this QGLRenderOrder is less than the \a rhs, otherwise
    returns false.  Reimplement this function when creating a sub-class of
    QGLRenderOrder.

    The default implementation sorts first by effect, second by material (front
    then back).

    Sorting by material is ordered by pointer comparison.

    Sorting by effect is based on the following order, from lowest to
    highest:
    \list
        \o No effect - hasEffect() == true
        \o Standard effect - ordered by numerical value, eg QGL::FlatColor < QGL::LitMaterial
        \o User effect - ordered by pointer comparison
    \endlist
    So a node with \c{hasEffect() == false} node is \i{less than} a node with
    a custom user effect, for example.

    \sa isEqual()
*/
bool QGLRenderOrder::isLessThan(const QGLRenderOrder &rhs) const
{
    bool result = false;
    bool thisHasEffect = effectiveHasEffect();
    bool thatHasEffect = rhs.effectiveHasEffect();
    if (thisHasEffect && thatHasEffect)
    {
        QGLAbstractEffect *eff = effectiveUserEffect();
        if (eff)
            result = (eff < rhs.effectiveUserEffect());
        else
            result = !rhs.effectiveUserEffect() &&
                     (effectiveStandardEffect() < rhs.effectiveStandardEffect());
    }
    else
    {
        result = !thisHasEffect;
    }
    if (!result)
    {
        result = (effectiveMaterial() < rhs.effectiveMaterial());
    }
    if (!result)
    {
        result = (effectiveBackMaterial() < rhs.effectiveBackMaterial());
    }
    return result;
}

/*!
    \fn bool QGLRenderOrder::operator!=(const QGLRenderOrder &rhs) const
    Returns true if this QGLRenderOrder is not equal to the \a rhs, otherwise
    returns false.  This function simply returns \c{!isEqual(rhs)}.
*/

/*!
    \fn bool QGLRenderOrder::bool operator==(const QGLRenderOrder &rhs) const
    Returns true if this QGLRenderOrder is equal to the \a rhs, otherwise
    returns false.  This function simply returns \c{isEqual(rhs)}.
*/

/*!
    \fn bool QGLRenderOrder::operator<(const QGLRenderOrder &rhs) const
    Returns true if this QGLRenderOrder is less than to the \a rhs, otherwise
    returns false.  This function simply returns \c{isLessThan(rhs)}.
*/

/*!
    \fn const QGLSceneNode *QGLRenderOrder::node() const
    Returns a pointer to the scene node for which the render order is held by
    this QGLRenderOrder instance.  This is simply the value passed to the
    constructor.
*/

/*!
    \fn RenderOrderKey QGLRenderOrder::key() const
    Returns the key for this QGLRender order instance.  This is simply the value
    passed to the constructor.
*/

/*!
    \fn QGLRenderState QGLRenderOrder::state() const
    Returns a pointer to the render state for this order.  This is simply the
    value passed to the constructor.
*/

/*!
    \fn void QGLRenderOrder::setState(const QGLRenderState &state)
    Sets the \a state for this order.
*/

/*!
    \fn QGLAbstractEffect *QGLRenderOrder::effectiveUserEffect() const
    Returns the effective user effect of the node set for this render
    order, taking into account any effect inherited from parent nodes
    as specified by the render state().

    \sa state(), effectiveHasEffect()
*/

/*!
    \fn QGL::StandardEffect QGLRenderOrder::effectiveStandardEffect() const
    Returns the effective standard effect of the node set for this render
    order, taking into account any effect inherited from parent nodes
    as specified by the render state().

    \sa state(), effectiveHasEffect()
*/

/*!
    \fn QGLMaterial *QGLRenderOrder::effectiveMaterial() const
    Returns the effective material of the node set for this render
    order, taking into account any effect inherited from parent nodes
    as specified by the render state().

    \sa state(), effectiveBackMaterial()
*/

/*!
    \fn QGLMaterial *QGLRenderOrder::effectiveBackMaterial() const
    Returns the effective back material of the node set for this render
    order, taking into account any effect inherited from parent nodes
    as specified by the render state().

    \sa state(), effectiveMaterial()
*/

/*!
    \fn bool QGLRenderOrder::effectiveHasEffect() const
    Returns the effective value of whether an effect is set on the node
    set for this render order, taking into account any effect inherited
    from parent nodes as specified by the render state().

    \sa state()
*/

/*!
    \fn uint qHash(const QGLRenderOrder &order)
    \relates QGLRenderOrder
    Returns a hash value representation of the \a order.
*/

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QGLRenderOrder &order)
{
    dbg << "QGLRenderOrder for node:" << order.node()
            << "-- effect hash:" << order.effectHash()
            << "-- material:" << order.node()->material()
            << "-- back material:" << order.node()->backMaterial();
    return dbg;
}

#endif


QT_END_NAMESPACE
