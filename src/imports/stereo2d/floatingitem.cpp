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

#include "floatingitem.h"
#include "redcyaneffect.h"
#include <QtGui/qpainter.h>
#include <QtGui/qpaintengine.h>
#include <QtGui/qgraphicsview.h>
#include <QtCore/qtimer.h>
#include <QtOpenGL/qgl.h>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass FloatingItem FloatingItem
    \brief The FloatingItem item defines the depth that a 2D element will be placed at in the 3D visual field.
    \since 4.8
    \ingroup qt3d::qml3d::stereo2d

    \code
    import Qt3D.Stereo2D 1.0

    FloatingItem {
        depth: -4
        Rectangle {
            anchors.fill: parent
            ...
        }
    }
    \endcode
*/

/*!
    \internal
*/
FloatingItem::FloatingItem(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);

    // TODO: 3D effects using something other than red-cyan.
    m_effect = new RedCyanEffect(this);
    setGraphicsEffect(m_effect);
}

/*!
    \internal
*/
FloatingItem::~FloatingItem()
{
}

/*!
    \qmlproperty real FloatingItem::depth

    This property defines the depth within the 3D field where children
    of this item will appear.  The default value of zero indicates
    that the left and right eye images will co-incide at the center of
    focus.

    If the value is non-zero, then it indicates the number of pixels that
    separate the left and right eye images.  A negative value places
    the item behind the center of focus and a positive value places
    the item in front of the center of focus.
*/

void FloatingItem::setDepth(qreal depth)
{
    if (m_depth != depth) {
        m_depth = depth;
        emit depthChanged();
        if (m_effect)
            m_effect->setDepth(depth);
    }
}

/*!
    \internal
*/
QRectF FloatingItem::boundingRect() const
{
    // Adjust the bounding rectangle to account for the overdraw.
    QRectF bounds = QDeclarativeItem::boundingRect();
    if (m_effect)
        return bounds;
    qreal depth = qAbs(m_depth) / 2.0f;
    return bounds.adjusted(-depth, -depth, depth, depth);
}

/*!
    \internal
*/
void FloatingItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *widget)
{
    if (!m_effect)
        return;

    // Using a red-cyan effect, for which we must have OpenGL rendering.
    QPaintEngine *engine = painter->paintEngine();
    if (engine->type() != QPaintEngine::OpenGL &&
            engine->type() != QPaintEngine::OpenGL2) {
        if (widget) {
            // Switch the surrounding QGraphicsView to use a QGLWidget as its
            // viewport.  We cannot do it here during painting, so schedule a
            // slot to switch it the next time we reach the event loop.
            QGraphicsView *view =
                qobject_cast<QGraphicsView *>(widget->parentWidget());
            if (view) {
                QTimer::singleShot(0, this, SLOT(switchToOpenGL()));
                return;
            }
        }
    }
}

/*!
    \internal
*/
void FloatingItem::switchToOpenGL()
{
    // If there are multiple FloatingItem's in the QML, then it is
    // possible that another FloatingItem has already switched to QGLWidget.
    QList<QGraphicsView *> views = scene()->views();
    if (!views.isEmpty()) {
        QGLWidget *glw = qobject_cast<QGLWidget *>(views[0]->viewport());
        if (glw)
            return;
        views[0]->setViewport(new QGLWidget(views[0]));
    }
}

QT_END_NAMESPACE
