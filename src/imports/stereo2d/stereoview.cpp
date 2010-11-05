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

#include "stereoview.h"
#include "stereovieweffect.h"
#include <QtGui/qpainter.h>
#include <QtGui/qpaintengine.h>
#include <QtGui/qgraphicsview.h>
#include <QtCore/qtimer.h>
#include <QtOpenGL/qgl.h>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass StereoView StereoView
    \brief The StereoView item defines the layout of a view with left and right stereo eye images.
    \since 4.8
    \ingroup qt3d::qml3d::stereo2d

    \code
    import Qt3D.Stereo2D 1.0

    StereoView {
        layout: StereoView.LeftRight
        width: 720; height: 640
        Rectangle {
            anchors.fill: parent
            ...
        }
    }
    \endcode

    \sa FloatingItem
*/

/*!
    \internal
*/
StereoView::StereoView(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
    , m_layout(StereoView::Default)
    , m_eye(StereoView::NoEye)
{
    // We need paint() to be called to switch to an OpenGL viewport.
    setFlag(QGraphicsItem::ItemHasNoContents, false);

    // Create a viewport item that defines the width and height
    // of a single eye image.  Our item children are parented here.
    m_viewport = new QDeclarativeItem(this);

    // Create a QGraphicsEffect for performing the double-render.
    m_effect = new StereoViewEffect(this);
    setGraphicsEffect(m_effect);
}

/*!
    \internal
*/
StereoView::~StereoView()
{
}

/*!
    \qmlproperty enumeration StereoView::layout

    This property defines the layout of the left and right eye
    images within this stereo view, and the relative size of
    the \l viewport.

    \list
    \o \c Default - the stereo layout is determined by the hardware.
       If the hardware is incapable of stereo, then the depth
       values on FloatingItem elements will be ignored.  The
       viewport is determined by the hardware type.
    \o \c RedCyan - render the left eye image with a red filter and the
       right eye image with a cyan filter, which makes the result
       suitable for viewing with red/cyan anaglyph glasses.
       The viewport occupies the entire StereoView.
    \o \c LeftRight - the left and right eye images are rendered into
       the left and right halves of the StereoView, with the
       viewport occupying half the width and the full height.
    \o \c RightLeft - the left and right eye images are rendered into
       the right and left halves of the StereoView, with the
       viewport occupying half the width and the full height.
    \o \c TopBottom - the left and right eye images are rendered into
       the top and bottom halves of the StereoView, with the
       viewport occupying the full width and half the height.
    \o \c BottomTop - the left and right eye images are rendered into
       the bottom and top halves of the StereoView, with the
       viewport occupying the full width and half the height.
    \o \c Disabled - stereo rendering is disabled, depth values on
       FloatingItem elements will be ignored, and the viewport
       occupies the entire StereoView.
    \endlist
*/

void StereoView::setLayout(StereoView::Layout layout)
{
    if (m_layout != layout) {
        m_layout = layout;
        updateViewportSize();
        update();
    }
}

/*!
    \internal
*/
QDeclarativeListProperty<QObject> StereoView::stereoViewData()
{
    // Return the "data" property for the viewport, which is
    // where we add all of the item children.
    return m_viewport->property("data").value< QDeclarativeListProperty<QObject> >();
}

/*!
    \qmlproperty Item StereoView::viewport

    This property defines the extent and position of the eye
    image viewport.  All child items are parented under the viewport,
    and should be positioned within it.  In the following example,
    the viewport will be the left half of the extent of the StereoView;
    i.e. (0, 0, 360, 640).

    \code
    StereoView {
        layout: StereoView.LeftRight
        width: 720; height: 640
        Rectangle {
            anchors.fill: viewport
            ...
        }
    }
    \endcode

    The viewport property also appears as the \c parent property
    of the children, so the following is equivalent:

    \code
    StereoView {
        layout: StereoView.LeftRight
        width: 720; height: 640
        Rectangle {
            anchors.fill: parent
            ...
        }
    }
    \endcode
*/

/*!
    \internal
    Finds the StereoView surrounding a specific \a item, usually
    an instance of FloatingItem.
*/
StereoView *StereoView::findView(QDeclarativeItem *item)
{
    while (item != 0) {
        StereoView *view = qobject_cast<StereoView *>(item);
        if (view)
            return view;
        item = item->parentItem();
    }
    return 0;
}

/*!
    \internal
*/
void StereoView::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *widget)
{
    if (m_layout != RedCyan)
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
void StereoView::switchToOpenGL()
{
    // If there are multiple StereoView's in the QML, then it is
    // possible that another StereoView has already switched to QGLWidget.
    QList<QGraphicsView *> views = scene()->views();
    if (!views.isEmpty()) {
        QGLWidget *glw = qobject_cast<QGLWidget *>(views[0]->viewport());
        if (glw)
            return;
        views[0]->setViewport(new QGLWidget(views[0]));
    }
}

/*!
    \internal
*/
void StereoView::geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry)
{
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
    updateViewportSize();
}

/*!
    \internal
*/
void StereoView::updateViewportSize()
{
    qreal w = width();
    qreal h = height();
    switch (m_layout) {
    case LeftRight: case RightLeft: w /= 2.0f; break;
    case TopBottom: case BottomTop: h /= 2.0f; break;
    default: break;
    }
    m_viewport->setSize(QSizeF(w, h));
}

QT_END_NAMESPACE
