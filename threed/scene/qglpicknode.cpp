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

#include "qglpicknode.h"
#include "qglabstractscene.h"

#include <QtGui/qevent.h>

/*!
    \class QGLPickNode
    \brief The QGLPickNode class enables picking for objects in a 3D scene.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::scene

    QGLPickNode is a QObject sub-class, relatively light-weight, that can
    shadow QSceneObject instances and provide picking capability.  The
    QGLPickNode instance receives events sent to it by picking views, such
    as the QGLView class, and emits relevant signals in response.

    QGLSceneObjects wanting to support picking, should implement the methods
    \list
    \o QGLSceneObject::pickNode()
    \o QGLSceneObject::setPickNode()
    \endlist
    and also implement rendering such that the QGLPainter has the relevant
    \l{QGLPainter::setObjectPickId()}{pick id function} called.

    These functions are already implemented for the QGLSceneNode object type.

    Picking can then be simply enabled by calling the QGLAbstractScene function
    setPickable() to turn on picking and generate pick nodes for the relevant
    scene objects.

    In order to respond to picking, the view class should simply register the
    picknodes, and connect a relevant slot to the pick nodes signal.

    For the QGLView class that code is simply:
    \code
    QList<QGLPickNode *>nodes = manager->pickNodes();
    foreach (QGLPickNode *node, nodes)
    {
        registerObject(node->id(), node);
        connect(node, SIGNAL(clicked()),
                this, SLOT(objectPicked()));
    }
    \endcode

    The pick nodes should be unregistered with similar code for the QGLView if
    switching to a different scene.

    To be able to recover the relevant QGLSceneObject inside the called slot,
    the object may be set onto the QGLPickNode with the setTarget() function,
    and recovered with target() inside the slot, since the QGLPickNode will
    be the sender() for the slot.

    QGLSceneNode already sets itself as the target during the pick enable step.

    \sa QGLSceneNode, QGLView
*/

/*!
    Construct a new QGLPickNode object managed by and parented onto
    the \a parent scene.  The nextPickId() function will be called on
    \a parent to assign a pick id to this new node.
*/
QGLPickNode::QGLPickNode(QGLAbstractScene *parent) :
    QObject(parent)
{
    m_id = parent->nextPickId();
}

/*!
    \fn int QGLPickNode::id() const
    Returns the unique id for this node.  This value is assigned by
    the parent scene for this node and should be different for every
    node in the scene.
*/

/*!
    \fn void QGLPickNode::setId(int id)
    Sets the unique \a id for this node.  Generally this function should not
    be needed, since the constructor causes a unique id to be obtained
    from the parent scene.  This function exists mainly for testing
    purposes.
*/

/*!
    \fn QGLSceneObject *QGLPickNode::target() const
    Returns the QGLSceneObject which is the target of this pick node.
*/

/*!
    \fn void QGLPickNode::setTarget(QGLSceneObject *target)
    Sets the \a target for this pick node.
*/

/*!
    \fn void QGLPickNode::pressed()
    Signal emitted when the scene object for this node has the mouse pressed
    while the cursor is on the object in the scene.
*/

/*!
    \fn void QGLPickNode::released()
    Signal emitted when the scene object for this node has the mouse released
    while the cursor is on the object in the scene.
*/

/*!
    \fn void QGLPickNode::clicked()
    Signal emitted when the scene object for this node has the mouse pressed
    and then released while the cursor is on the object in the scene.
*/

/*!
    \fn void QGLPickNode::doubleClicked()
    Signal emitted when the scene object for this node has the mouse clicked
    twice in succession while the cursor is on the object in the scene.
*/

/*!
    \fn void QGLPickNode::hoverChanged()
    Signal emitted when the scene object for this node has the mouse moved
    into or out of this object in the scene.
*/

/*!
    \internal
*/
bool QGLPickNode::event(QEvent *e)
{
    // ripped off from teaservice demo, but not before the same code
    // was ripped off and put in item3d.cpp - those should probably all
    // use this implementation here
    if (e->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *me = (QMouseEvent *)e;
        if (me->button() == Qt::LeftButton)
            emit pressed();
    }
    else if (e->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *me = (QMouseEvent *)e;
        if (me->button() == Qt::LeftButton)
        {
            emit released();
            if (me->x() >= 0)   // Positive: inside object, Negative: outside.
                emit clicked();
        }
    }
    else if (e->type() == QEvent::MouseButtonDblClick)
    {
        emit doubleClicked();
    }
    else if (e->type() == QEvent::Enter)
    {
        //m_hovering = true;
        emit hoverChanged();
    }
    else if (e->type() == QEvent::Leave)
    {
        //m_hovering = false;
        emit hoverChanged();
    }
    return QObject::event(e);
}
