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

#include "qglrenderorderrepository.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLRenderOrderRepository
    \brief The QGLRenderOrderRepository class supplies QGLRenderOrder instances.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::scene

    The QGLRenderOrderRepository class works with the QGLRenderSequencer and
    QGLRenderOrder classes to optimize the rendering order of scene nodes.

    This class is responsible for producing QGLRenderOrder instances for
    the QGLRenderSequencer.

    Custom render orders may be created by sub-classing QGLRenderOrderRepository
    and reimplementing the following method:
    \list
        \i newOrder()
    \endlist

    Then create a class inheriting from QGLRenderOrderRepository, reimplement
    newOrder() so that it returns an instance of your new QGLRenderOrder
    sub-class, and set an instance of your factory onto QGLPainter:

    \code
    void MyView::initializeGL(QGLPainter *painter)
    {
        painter->renderSequencer()->setRenderOrderFactory(m_renderOrderFactory);
    }

    void MyView::paintGL(QGLPainter *painter)
    {
        // draw uses QGLRenderOrder sub-class instances from m_renderOrderFactory
        complexScene->draw(painter);
    }
    \endcode

    Note that due to view frustum culling and other effects, all the scene nodes
    in a scene may not be tested in a render pass.  This is especially the case
    in larger scenes.  Therefore no attempt is made to store all orders
    between renders.

    See the QGLRenderOrder class documentation for more details.

    \sa QGLRenderOrder
*/

/*!
    \fn QGLRenderOrderRepository::QGLRenderOrderRepository()
    Construct a new QGLRenderOrderRepository.
*/

/*!
    \fn QGLRenderOrder *QGLRenderOrderRepository::getOrder(const RenderOrderKey &key)
    Returns a new instance of a QGLRenderOrder or sub-class, based on the render order
    \a key given.

    Reimplement this function when creating custom render orders.
*/

QT_END_NAMESPACE
