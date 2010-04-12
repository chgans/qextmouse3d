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

#include "qglviewport.h"
#include "qgldepthbufferoptions.h"
#include "qglblendoptions.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLViewport
    \brief The QGLViewport class provides a viewport layer for a QGLView.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::viewing
*/

class QGLViewportPrivate
{
public:
    QGLViewportPrivate(QGLViewport *viewport)
        : layer(0)
        , isEnabled(true)
    {
        camera = defaultCamera = new QGLCamera(viewport);

        QObject::connect(camera, SIGNAL(projectionChanged()),
                         viewport, SIGNAL(viewportChanged()));
        QObject::connect(camera, SIGNAL(viewChanged()),
                         viewport, SIGNAL(viewportChanged()));

        depthBufferOptions.setEnabled(true);
        depthBufferOptions.setFunction(QGLDepthBufferOptions::Less);

        blendOptions.setEnabled(false);
        blendOptions.setSourceColorFactor(QGLBlendOptions::SrcAlpha);
        blendOptions.setSourceAlphaFactor(QGLBlendOptions::SrcAlpha);
        blendOptions.setDestinationColorFactor(QGLBlendOptions::OneMinusSrcAlpha);
        blendOptions.setDestinationAlphaFactor(QGLBlendOptions::OneMinusSrcAlpha);
    }

    int layer;
    bool isEnabled;
    QRect rect;
    QGLCamera *camera;
    QGLCamera *defaultCamera;
    QColor backgroundColor;
    QGLDepthBufferOptions depthBufferOptions;
    QGLBlendOptions blendOptions;
};

/*!
    Constructs a new viewport and attaches it to \a parent.
*/
QGLViewport::QGLViewport(QObject *parent)
    : QObject(parent)
    , d_ptr(new QGLViewportPrivate(this))
{
}

/*!
    Destroys this viewport.
*/
QGLViewport::~QGLViewport()
{
}

/*!
    \property QGLViewport::enabled
    \brief the enable state for the viewport.

    The value is true if this viewport is enabled for display in the view;
    false otherwise.  The default value is true.

    \sa viewportChanged()
*/
bool QGLViewport::isEnabled() const
{
    Q_D(const QGLViewport);
    return d->isEnabled;
}

void QGLViewport::setEnabled(bool enable)
{
    Q_D(QGLViewport);
    if (d->isEnabled != enable) {
        d->isEnabled = enable;
        emit viewportChanged();
    }
}

/*!
    \property QGLViewport::rect
    \brief the rectangle that is occupied by this viewport.

    The rectangle shold be null if the viewport occupies the full
    extent of the window.  The default value is null.

    \sa viewportChanged()
*/
QRect QGLViewport::rect() const
{
    Q_D(const QGLViewport);
    return d->rect;
}

void QGLViewport::setRect(const QRect &rect)
{
    Q_D(QGLViewport);
    if (d->rect != rect) {
        d->rect = rect;
        emit viewportChanged();
    }
}

/*!
    \property QGLViewport::layer
    \brief the view layer that is occupied by this viewport.

    Viewports are rendered in increasing order of layer.
    The default value is zero.

    \sa viewportChanged()
*/
int QGLViewport::layer() const
{
    Q_D(const QGLViewport);
    return d->layer;
}

void QGLViewport::setLayer(int layer)
{
    Q_D(QGLViewport);
    if (d->layer != layer) {
        d->layer = layer;
        emit viewportChanged();
    }
}

/*!
    \property QGLViewport::camera
    \brief the camera parameters.

    The camera defines the projection to apply to convert eye co-ordinates
    into window co-ordinates, and the position and orientation of the
    viewer's eye.

    If the value is set to null, then the default camera object will be used.

    \sa viewportChanged()
*/
QGLCamera *QGLViewport::camera() const
{
    Q_D(const QGLViewport);
    return d->camera;
}

void QGLViewport::setCamera(QGLCamera *camera)
{
    Q_D(QGLViewport);

    if (!camera)
        camera = d->defaultCamera;

    if (d->camera == camera)
        return;

    disconnect(d->camera, SIGNAL(projectionChanged()),
               this, SIGNAL(viewportChanged()));
    disconnect(d->camera, SIGNAL(viewChanged()),
               this, SIGNAL(viewportChanged()));

    d->camera = camera;

    connect(d->camera, SIGNAL(projectionChanged()),
            this, SIGNAL(viewportChanged()));
    connect(d->camera, SIGNAL(viewChanged()),
            this, SIGNAL(viewportChanged()));

    emit viewportChanged();
}

/*!
    \property QGLViewport::backgroundColor
    \brief the background color to use to clear the viewport before
    drawing its contents.

    If the color is invalid, then the background will not be cleared.
    The default value is null.

    If the color includes a non-opaque alpha component, then the color
    will be blended over the top of the current window contents.

    \sa viewportChanged()
*/
QColor QGLViewport::backgroundColor() const
{
    Q_D(const QGLViewport);
    return d->backgroundColor;
}

void QGLViewport::setBackgroundColor(const QColor& color)
{
    Q_D(QGLViewport);
    if (d->backgroundColor != color) {
        d->backgroundColor = color;
        emit viewportChanged();
    }
}

/*!
    Prepares to render GL content with \a painter into this viewport
    from the perspective of \a eye.  The \a windowRect specifies the
    full \a window region containing the viewport, which will be adjusted
    by rect().  The default implementation does the following:

    \list
    \o Sets the \c{glViewport()} and \c{glScissor()} to constrain
       painting to the viewport's rect() (the scissor is disabled if
       the viewport covers the entire window).
    \o Disables stencil testing.
    \o Clears the background of the viewport to backgroundColor()
       if it is not null.
    \o Enables depth testing with the default QGLDepthBufferOptions.
    \o Enables the default QGLBlendOptions, but with blending disabled.
       Use QGLPainter::setBlendingEnabled() to enable blending.
    \o Clears the depth buffer.
    \o Sets the culling mode to QGL::CullDisabled.
    \o Sets the modelview and projection matrices on \a painter to
       reflect the viewpoint defined by camera() and \a eye.
    \endlist

    \sa paintGL()
*/
void QGLViewport::prepareGL(QGLPainter *painter, QGL::Eye eye,
                            QPaintDevice *window, const QRect &windowRect)
{
    Q_D(QGLViewport);

    // Set the GL viewport and scissor.
    QRect rect;
    if (d->rect.isNull())
        rect = windowRect;
    else
        rect = d->rect.translated(windowRect.topLeft()).intersected(windowRect);
    painter->setViewport(rect);
    int width = window->width();
    int height = window->height();
    if (windowRect.x() == 0 && windowRect.y() == 0 &&
            windowRect.width() == width && windowRect.height() == height) {
        painter->setScissor(QRect());
    } else {
        painter->setScissor(rect);
    }
    glDisable(GL_STENCIL_TEST);

    // Adjust the blend options.  Set this before clearing to the
    // background color in case it involves transparency.
    d->blendOptions.apply();

    // Clear the background.
    bool depthCleared = false;
    if (d->backgroundColor.isValid()) {
        if (painter->isPicking()) {
            // When picking, just black out the background of the
            // viewport so that objects in lower viewports aren't
            // picked when the mouse is in this viewport.
            painter->setClearColor(Qt::black);
            painter->clear();
            depthCleared = true;
        } else if (d->backgroundColor.alpha() == 255) {
            // Use glClear() to fill the background.
            painter->setClearColor(d->backgroundColor);
            painter->clear();
            depthCleared = true;
        } else {
            // We clear the background by drawing a triangle fan so
            // that the background color will blend with the underlying
            // screen content if it has an alpha component.
            glDisable(GL_DEPTH_TEST);
            QVector2DArray array;
            array.append(-1, -1);
            array.append(1, -1);
            array.append(1, 1);
            array.append(-1, 1);
            painter->projectionMatrix().setToIdentity();
            painter->modelViewMatrix().setToIdentity();
            painter->setStandardEffect(QGL::FlatColor);
            painter->setColor(d->backgroundColor);
            painter->setVertexAttribute(QGL::Position, array);
            painter->draw(QGL::TriangleFan, 4);
        }
    }

    // Clear the depth buffer and set the depth buffer options.
    if (!depthCleared)
        painter->clear(QGL::ClearDepthBuffer);
    d->depthBufferOptions.apply();

    // Disable back face culling.
    painter->setCullFaces(QGL::CullDisabled);

    // Apply the camera modelview and projection matrices for the eye.
    painter->setEye(eye);
    painter->setCamera(d->camera);
}

/*!
    \fn void QGLViewport::paintGL(QGLPainter *painter)

    Paints the scene within this viewport onto \a painter.  The buffers
    and camera() will have already been initialized by prepareGL().

    If \a painter has object picking enabled, then this function may
    choose to render a simplified version of the scene that is suitable
    for picking the selectable objects, while ignoring objects that
    are not selectable.

    \sa prepareGL(), QGLPainter::isPicking(), needsPickUpdate()
*/

/*!
    Returns true if paintGL() needs to be called to refresh
    the contents of the pick buffer; false otherwise.

    The default implementation returns true to indicate that
    the pick buffer must be refreshed whenever the view is repainted.
    This can cause the pick buffer to be regenerated very often if
    the scene is animated.

    If the animations are limited to objects bouncing in place,
    rotating on an axis, or moving along a short path, the pick
    buffer probably does not need to be regenerated every time
    paintGL() is called.

    Subclasses can override needsPickUpdate() and return false if
    the scene has not changed substantially enough to require a
    pick buffer change.

    Subclasses will usually modify paintGL() to render an elongated
    version of the object into the pick buffer that covers the complete
    animation path.  This way, the user can click anywhere along the
    animation path to select the object rather than having to hit a
    moving object exactly to select it.

    \sa paintGL()
*/
bool QGLViewport::needsPickUpdate()
{
    return true;
}

/*!
    Requests that the view containing this viewport be updated.

    \sa viewportChanged()
*/
void QGLViewport::update()
{
    emit viewportChanged();
}

/*!
    \fn void QGLViewport::viewportChanged()

    This signal is emitted when any of the viewport parameters change,
    which will require a redraw of the contents.
*/

QT_END_NAMESPACE
