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

#include "qglpainter.h"
#include "qglpainter_p.h"
#include "qglabstracteffect.h"
#include <QtOpenGL/qglpixelbuffer.h>
#include <QtOpenGL/private/qgl_p.h>
#include <QtOpenGL/qglshaderprogram.h>
#include <QtOpenGL/qglframebufferobject.h>
#include <QtGui/private/qwidget_p.h>
#include <QtGui/private/qwindowsurface_p.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpaintengine.h>
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qmap.h>
#if !defined(QT_NO_THREAD)
#include <QtCore/qthreadstorage.h>
#endif
#include "qglflatcoloreffect.h"
#include "qglflattextureeffect.h"
#include "qgllitmaterialeffect.h"
#include "qgllittextureeffect.h"
#include "qglpickcolors_p.h"
#include "qgltexture2d.h"
#include "qgltexture2d_p.h"
#include "qgltexturecube.h"
#include "qgeometrydata.h"
#include "qglvertexbuffer_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLPainter
    \brief The QGLPainter class provides portable API's for rendering into a GL context.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::painting

    TBD - lots of TBD

    All QGLPainter instances on a context share the same context state:
    matrices, effects, vertex attributes, etc.  For example, calling
    ortho() on one QGLPainter instance for a context will alter the
    projectionMatrix() as seen by the other QGLPainter instances.
*/

/*!
    \enum QGLPainter::Update
    This enum defines the values that were changed since the last QGLPainter::update().

    \value UpdateColor The color has been updated.
    \value UpdateModelViewMatrix The modelview matrix has been updated.
    \value UpdateProjectionMatrix The projection matrix has been updated.
    \value UpdateMatrices The combination of UpdateModelViewMatrix and
           UpdateProjectionMatrix.
    \value UpdateLights The lights have been updated.
    \value UpdateMaterials The material parameters have been updated.
    \value UpdateFog The fog parameters have been updated.
    \value UpdateAll All values have been updated.  This is specified
           when an effect is activated.
*/

#define QGLPAINTER_CHECK_PRIVATE() \
    Q_ASSERT_X(d, "QGLPainter", "begin() has not been called or it failed")
#define QGLPAINTER_CHECK_PRIVATE_RETURN(value) \
    Q_ASSERT_X(d, "QGLPainter", "begin() has not been called or it failed")

QGLPainterPrivate::QGLPainterPrivate()
    : ref(1),
      activePaintEngine(0),
      projectionMatrix(QGLMatrixStack::ProjectionMatrix),
      modelViewMatrix(QGLMatrixStack::ModelViewMatrix),
      eye(QGL::NoEye),
      lightModel(0),
      defaultLightModel(0),
      mainLight(0),
      defaultLight(0),
      frontMaterial(0),
      backMaterial(0),
      defaultMaterial(0),
      frontColorMaterial(0),
      backColorMaterial(0),
      fogParameters(0),
      viewingCube(QVector3D(-1, -1, -1), QVector3D(1, 1, 1)),
      scissor(0, 0, -3, -3),
      color(255, 255, 255, 255),
      updates(QGLPainter::UpdateAll),
      pick(0),
      boundVertexBuffer(0),
      boundIndexBuffer(0)
#if QT_VERSION < 0x040700
      , vertexAttribPointer(0)
#endif
{
    context = 0;
    effect = 0;
    userEffect = 0;
    standardEffect = QGL::FlatColor;
    memset(stdeffects, 0, sizeof(stdeffects));
    textureUnitCount = 0;
}

QGLPainterPrivate::~QGLPainterPrivate()
{
    delete defaultLightModel;
    delete defaultLight;
    delete defaultMaterial;
    delete frontColorMaterial;
    delete backColorMaterial;
    for (int effect = 0; effect < QGL_MAX_STD_EFFECTS; ++effect)
        delete stdeffects[effect];
    delete pick;
    qDeleteAll(cachedPrograms);
}

QGLPainterPickPrivate::QGLPainterPickPrivate()
{
    isPicking = false;
    objectPickId = -1;
    pickColorIndex = -1;
    pickColor = 0;
    defaultPickEffect = new QGLFlatColorEffect();
}

QGLPainterPickPrivate::~QGLPainterPickPrivate()
{
    delete defaultPickEffect;
}

#if !defined(QT_NO_THREAD)

// QGLContext's are thread-specific, so QGLPainterPrivateCache should be too.

typedef QThreadStorage<QGLPainterPrivateCache *> QGLPainterPrivateStorage;
Q_GLOBAL_STATIC(QGLPainterPrivateStorage, painterPrivateStorage)
static QGLPainterPrivateCache *painterPrivateCache()
{
    QGLPainterPrivateCache *cache = painterPrivateStorage()->localData();
    if (!cache) {
        cache = new QGLPainterPrivateCache();
        painterPrivateStorage()->setLocalData(cache);
    }
    return cache;
}

#else

Q_GLOBAL_STATIC(QGLPainterPrivateCache, painterPrivateCache)

#endif

QGLPainterPrivateCache::QGLPainterPrivateCache()
{
    connect(QGLSignalProxy::instance(),
            SIGNAL(aboutToDestroyContext(const QGLContext *)),
            this,
            SLOT(contextDestroyed(const QGLContext *)));
}

QGLPainterPrivateCache::~QGLPainterPrivateCache()
{
}

QGLPainterPrivate *QGLPainterPrivateCache::fromContext
    (const QGLContext *context)
{
    QGLPainterPrivate *priv = cache.value(context, 0);
    if (priv)
        return priv;
    priv = new QGLPainterPrivate();
    priv->context = context;
    cache.insert(context, priv);
    return priv;
}

QGLPainterPrivateCache *QGLPainterPrivateCache::instance()
{
    return painterPrivateCache();
}

void QGLPainterPrivateCache::contextDestroyed(const QGLContext *context)
{
    QGLPainterPrivate *priv = cache.value(context, 0);
    if (priv) {
        priv->context = 0;
        cache.remove(context);
        if (!priv->ref.deref())
            delete priv;
    }
    emit destroyedContext(context);
}

/*!
    Constructs a new GL painter.  Call begin() to attach the
    painter to a GL context.

    \sa begin()
*/
QGLPainter::QGLPainter()
    : d_ptr(0)
{
}

/*!
    Constructs a new GL painter and attaches it to \a context.
    It is not necessary to call begin() after construction.

    \sa begin()
*/
QGLPainter::QGLPainter(const QGLContext *context)
    : d_ptr(0)
{
    begin(context);
}

/*!
    Constructs a new GL painter and attaches it to the GL
    context associated with \a device.  It is not necessary to
    call begin() after construction.

    If \a device does not have a GL context, then isActive()
    will return false; true otherwise.

    This constructor can be used to paint into QGLWidget and QGLPixelBuffer
    paint devices, or into any QWidget if the OpenGL graphics system
    is active.

    \sa begin(), isActive()
*/
QGLPainter::QGLPainter(QPaintDevice *device)
    : d_ptr(0)
{
    begin(device);
}

/*!
    Constructs a new GL painter and attaches it to the GL context associated
    with \a painter.  It is assumed that \a painter is the currently
    active painter and that it is associated with the current GL context.

    If \a painter is not using an OpenGL paint engine, then isActive()
    will return false; true otherwise.

    This constructor is typically used when mixing regular Qt painting
    operations and GL painting operations on a widget that is being
    drawn using the OpenGL graphics system.

    \sa begin(), isActive()
*/
QGLPainter::QGLPainter(QPainter *painter)
    : d_ptr(0)
{
    begin(painter);

}

/*!
    Destroys this GL painter.
*/
QGLPainter::~QGLPainter()
{
    end();
}

/*!
    Begins painting on QGLContext::currentContext().  Returns false
    if there is no GL context current.

    \sa end()
*/
bool QGLPainter::begin()
{
    return begin(QGLContext::currentContext());
}

/*!
    Begins painting on \a context.  If painting was already in progress,
    then this function will call end() first.  The \a context will be
    made current if it is not already current.

    Returns true if painting can begin; false otherwise.

    All QGLPainter instances on a context share the same context state:
    matrices, the effect(), vertex attributes, etc.  For example,
    calling ortho() on one QGLPainter instance for a context will
    alter the projectionMatrix() as seen by the other QGLPainter instances.

    \sa end(), isActive()
*/
bool QGLPainter::begin(const QGLContext *context)
{
    if (!context)
        return false;
    end();
    const_cast<QGLContext *>(context)->makeCurrent();
    d_ptr = painterPrivateCache()->fromContext(context);
    d_ptr->ref.ref();

    // Force the matrices to be updated the first time we use them.
    d_ptr->modelViewMatrix.markDirty();
    d_ptr->projectionMatrix.markDirty();

    return true;
}

/*!
    Begins painting on the GL context associated with \a device.
    Returns false if \a device does not have a GL context.

    This function can be used to paint into QGLWidget and QGLPixelBuffer
    paint devices, or into any QWidget if the OpenGL graphics system
    is active.

    \sa end()
*/
bool QGLPainter::begin(QPaintDevice *device)
{
    if (!device)
        return false;
    end();
    int devType = device->devType();
    if (devType == QInternal::Widget) {
        QWidget *widget = static_cast<QWidget *>(device);

        // Handle the easy QGLWidget case first.
        QGLWidget *glWidget = qobject_cast<QGLWidget *>(widget);
        if (glWidget)
            return begin(glWidget->context());

        // See if there is a QPainter active for the widget's window surface.
        QWindowSurface *surf = widget->windowSurface();
        if (surf) {
            QPaintDevice *surfDevice = surf->paintDevice();
            if (surfDevice && surfDevice->paintingActive()) {
                // Check that the QPaintEngine is OpenGL and that the
                // currently active QPainter on that engine is this widget.
                QPaintEngine *engine = surfDevice->paintEngine();
                if (engine &&
                        (engine->type() == QPaintEngine::OpenGL ||
                         engine->type() == QPaintEngine::OpenGL2)) {
                    QPainter *painter = engine->painter();
                    if (painter && painter->device() == device)
                        return begin(painter);
                }
            }
        }
    } else if (devType == QInternal::Pbuffer) {
        QGLPixelBuffer *pbuffer = static_cast<QGLPixelBuffer *>(device);
        if (pbuffer->makeCurrent())
            return begin();
    }
    return false;
}

/*!
    Begins painting on the GL context associated with \a painter.
    Returns false if \a painter is not using an OpenGL paint engine.
    It is assumed that \a painter is the currently active painter
    and that it is associated with the current GL context.

    This function is typically used when mixing regular Qt painting
    operations and GL painting operations on a widget that is being
    drawn using the OpenGL graphics system.

    \sa end()
*/
bool QGLPainter::begin(QPainter *painter)
{
    // Validate that the painting is OpenGL-based.
    if (!painter)
        return false;
    QPaintEngine *engine = painter->paintEngine();
    if (!engine)
        return false;
    if (engine->type() != QPaintEngine::OpenGL &&
            engine->type() != QPaintEngine::OpenGL2)
        return false;

    // Force the OpenGL paint engine to flush its GL state
    // so that we can take over drawing.
    painter->beginNativePainting();

    // Begin GL painting operations.
    bool result = begin(QGLContext::currentContext());

    // Remember the paint engine so that end() can call endNativePainting().
    if (result) {
        Q_D(QGLPainter);
        if (engine->isExtended())
            d->activePaintEngine = static_cast<QPaintEngineEx *>(engine);
        else
            d->activePaintEngine = 0;
    }
    return result;
}

/*!
    Ends GL painting.  Returns true if painting was ended successfully;
    false if this painter was not bound to a GL context.

    The GL context that was bound to this painter will not have
    QGLContext::doneCurrent() called on it.  It is the responsibility
    of the caller to terminate context operations.

    The effect() will be left active in the GL context and will be
    assumed to still be active the next time begin() is called.
    If this assumption doesn't apply, then call disableEffect()
    to disable the effect before calling end().

    This function will pop all surfaces from the surface stack,
    and return currentSurface() to null (the default drawing surface).

    \sa begin(), isActive(), disableEffect()
*/
bool QGLPainter::end()
{
    if (!d_ptr)
        return false;
    if (d_ptr->boundVertexBuffer) {
        QGLBuffer::release(QGLBuffer::VertexBuffer);
        d_ptr->boundVertexBuffer = 0;
    }
    if (d_ptr->boundIndexBuffer) {
        QGLBuffer::release(QGLBuffer::IndexBuffer);
        d_ptr->boundIndexBuffer = 0;
    }
    if (!d_ptr->surfaceStack.isEmpty() && int(d_ptr->ref) <= 2) {
        // Restore the default drawing surface before ending painting
        // operations.  This is needed for proper interoperation with
        // the OpenGL paint engine which will assume that it is drawing
        // to the default drawing surface after endNativePainting().
        // The ref needs to be 2 or less so that we don't do this
        // if the QGLPainter was nested within another instance of itself.
        QGLFramebufferObject *fbo = d_ptr->surfaceStack.last();
        if (fbo)
            fbo->release();
        d_ptr->surfaceStack.clear();
    }
    if (d_ptr->activePaintEngine) {
        d_ptr->activePaintEngine->endNativePainting();
        d_ptr->activePaintEngine = 0;
    }
    if (!d_ptr->ref.deref())
        delete d_ptr;
    d_ptr = 0;
    return true;
}

/*!
    Returns true if this painter is currently bound to a GL context;
    false otherwise.

    \sa begin(), end()
*/
bool QGLPainter::isActive() const
{
    return (d_ptr != 0 && d_ptr->context != 0);
}

/*!
    Returns the GL context that is bound to this painter, or null
    if it is not currently bound.
*/
const QGLContext *QGLPainter::context() const
{
    if (d_ptr)
        return d_ptr->context;
    else
        return 0;
}

/*!
    Returns true if the underlying OpenGL implementation is OpenGL 1.x
    or OpenGL/ES 1.x and only supports fixed-function OpenGL operations.
    Returns false if the underlying OpenGL implementation is using
    GLSL or GLSL/ES shaders.

    If this function returns false, then the built-in effects will
    use shaders and QGLPainter will not update the fixed-function
    matrices in the OpenGL context when update() is called.
    User-supplied effects will need to use shaders also or update
    the fixed-function matrices themselves or call updateFixedFunction().

    \sa update(), updateFixedFunction()
*/
bool QGLPainter::isFixedFunction() const
{
#if defined(QT_OPENGL_ES_2)
    return false;
#elif defined(QT_OPENGL_ES_1)
    return true;
#else
    return !QGLShaderProgram::hasOpenGLShaderPrograms();
#endif
}

/*!
    Clears the specified rendering \a buffers.  The default \a buffers
    value is QGL::ClearColorBuffer | QGL::ClearDepthBuffer, which
    indicates that the color and depth buffers should be cleared.

    \sa setClearColor(), setClearDepth(), setClearStencil()
*/
void QGLPainter::clear(QGL::ClearBuffers buffers)
{
    glClear(GLuint(buffers));
}

/*!
    Sets the \a color to use to clear the color buffer when clear()
    is called.

    \sa clear()
*/
void QGLPainter::setClearColor(const QColor& color)
{
    glClearColor(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}

/*!
    Sets the \a depth to use to clear the depth buffer when clear()
    is called.

    \sa clear()
*/
void QGLPainter::setClearDepth(qreal depth)
{
#if defined(QT_OPENGL_ES)
    glClearDepthf(depth);
#else
    glClearDepth(depth);
#endif
}

/*!
    Sets the stencil \a value to use to clear the stencil buffer
    when clear() is called.

    \sa clear()
*/
void QGLPainter::setClearStencil(GLint value)
{
    glClearStencil(value);
}

/*!
    \fn void QGLPainter::setDepthTestingEnabled(bool value)

    Enables or disables depth testing according to \a value.
    This is a convience function that is equivalent to
    \c{glEnable(GL_DEPTH_TEST)} or \c{glDisable(GL_DEPTH_TEST)}.

    For more complex depth buffer configurations, use QGLDepthBufferOptions.

    \sa QGLDepthBufferOptions
*/

/*!
    \fn void QGLPainter::setStencilTestingEnabled(bool value)

    Enables or disables stencil testing according to \a value.
    This is a convience function that is equivalent to
    \c{glEnable(GL_STENCIL_TEST)} or \c{glDisable(GL_STENCIL_TEST)}.

    For more complex depth buffer configurations, use QGLStencilBufferOptions.

    \sa QGLStencilBufferOptions
*/

/*!
    \fn void QGLPainter::setBlendingEnabled(bool value)

    Enables or disables blending according to \a value.
    This is a convience function that is equivalent to
    \c{glEnable(GL_BLEND)} or \c{glDisable(GL_BLEND)}.

    For more complex blending configurations, use QGLBlendOptions.

    \sa QGLBlendOptions
*/

/*!
    Returns the viewport for the active GL context.  The origin for
    the returned rectangle is the top-left of the drawing surface.

    \sa setViewport(), resetViewport()
*/
QRect QGLPainter::viewport() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(QRect());
    if (d->viewport.isNull()) {
        GLint view[4];
        glGetIntegerv(GL_VIEWPORT, view);
        d->viewport = QRect(view[0], view[1], view[2], view[3]);
    }
    // Convert the GL viewport into standard Qt co-ordinates.
    return QRect(d->viewport.x(),
                 d->context->device()->height() -
                        (d->viewport.y() + d->viewport.height()),
                 d->viewport.width(), d->viewport.height());
}

/*!
    Sets the viewport for the active GL context to \a rect.
    The origin for \a rect is the top-left of the drawing surface.

    \sa viewport(), resetViewport()
*/
void QGLPainter::setViewport(const QRect& rect)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    int y = d->context->device()->height() - (rect.y() + rect.height());
    glViewport(rect.x(), y, rect.width(), rect.height());
    d->viewport = QRect(rect.x(), y, rect.width(), rect.height());
}

/*!
    Sets the viewport for the active GL context to start at the
    origin and extend for \a size.

    \sa viewport(), resetViewport()
*/
void QGLPainter::setViewport(const QSize& size)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    glViewport(0, 0, size.width(), size.height());
    d->viewport = QRect(0, 0, size.width(), size.height());
}

/*!
    Sets the viewport for the active GL context to start at the
    origin and extend for \a width and \a height.

    \sa viewport(), resetViewport()
*/
void QGLPainter::setViewport(int width, int height)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    glViewport(0, 0, width, height);
    d->viewport = QRect(0, 0, width, height);
}

/*!
    Resets this painter's notion of what the current viewport()
    is set to.  The next time viewport() is called, the actual
    viewport rectangle will be fetched from the GL server.

    This function is used to synchronize the state of the application
    with the GL server after the execution of raw GL commands that may
    have altered the viewport settings.

    \sa viewport(), setViewport()
*/
void QGLPainter::resetViewport()
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->viewport = QRect();
}

/*!
    Returns the currently active scissor rectangle; or a null rectangle
    if scissoring is disabled.

    The special rectangle value of (0, 0, -2, -2) is returned to
    indicate that scissoring is enabled but that the scissor
    is empty, effectively clipping away all drawing requests.
    This value is chosen so that it is distinguishable from
    the null rectangle (0, 0, -1, -1).

    Performance note: if setScissor() has not been called on this
    QGLPainter, then this function will have to perform a round-trip
    to the GL server to get the scissor rectangle.  This round-trip
    can be avoided by calling setScissor() before scissor().

    Note that the returned rectangle will be in window co-ordinates,
    with the origin at the top-left of the drawing surface.

    \sa setScissor(), resetScissor()
*/
QRect QGLPainter::scissor() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(QRect());
    if (d->scissor.width() <= -3) {
        if (glIsEnabled(GL_SCISSOR_TEST)) {
            QPaintDevice *device = d->context->device();
            GLint scissor[4];
            glGetIntegerv(GL_SCISSOR_BOX, scissor);
            if (scissor[2] != 0 && scissor[3] != 0) {
                d->scissor = QRect
                    (scissor[0], device->height() - (scissor[1] + scissor[3]),
                     scissor[2], scissor[3]);
            } else {
                // Special value indicating an empty, but active, scissor.
                d->scissor = QRect(0, 0, -2, -2);
            }
        } else {
            d->scissor = QRect();
        }
    }
    return d->scissor;
}

/*!
    Enables scissoring to the boundaries of \a rect if it has a
    non-zero size; disables scissoring if \a rect is null.

    The special rectangle value of (0, 0, -2, -2) is used to
    indicate that scissoring is enabled but that the scissor
    is empty, effectively clipping away all drawing requests.
    This value is chosen so that it is distinguishable from
    the null rectangle (0, 0, -1, -1).

    The \a rect is assumed to be in window co-ordinates, with the
    origin at the top-left of the drawing surface.  It will be
    intersected with the drawing surface's bounds before being
    set in the GL server.

    \sa scissor(), resetScissor(), intersectScissor()
*/
void QGLPainter::setScissor(const QRect& rect)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (rect.width() == -2) {
        // Special value indicating an empty, but active, scissor.
        d->scissor = rect;
        glScissor(0, 0, 0, 0);
        glEnable(GL_SCISSOR_TEST);
    } else if (!rect.isNull()) {
        QPaintDevice *device = d->context->device();
        int height = device->height();
        QRect r = rect.intersected(QRect(0, 0, device->width(), height));
        if (r.isValid()) {
            d->scissor = r;
            glScissor(r.x(), height - (r.y() + r.height()),
                      r.width(), r.height());
        } else {
            d->scissor = QRect(0, 0, -2, -2);
            glScissor(0, 0, 0, 0);
        }
        glEnable(GL_SCISSOR_TEST);
    } else {
        d->scissor = QRect();
        glDisable(GL_SCISSOR_TEST);
    }
}

/*!
    Intersects the current scissor rectangle with \a rect and sets
    the intersection as the new scissor.

    The \a rect is assumed to be in window co-ordinates, with the
    origin at the top-left of the drawing surface.

    \sa setScissor(), expandScissor()
*/
void QGLPainter::intersectScissor(const QRect& rect)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    QRect current = scissor();
    if (current.width() == -2) {
        // Scissor is already active and empty: nothing to do.
        return;
    } else if (rect.isEmpty()) {
        // Intersecting with an empty rectangle sets the scissor to empty.
        // This includes the case where "rect" is (0, 0, -2, -2).
        d->scissor = QRect(0, 0, -2, -2);
        glScissor(0, 0, 0, 0);
        glEnable(GL_SCISSOR_TEST);
        return;
    }
    QPaintDevice *device = d->context->device();
    QRect r;
    if (current.isNull())
        r = rect.intersected(QRect(0, 0, device->width(), device->height()));
    else
        r = current.intersected(rect);
    if (r.isValid()) {
        d->scissor = r;
        glScissor(r.x(), device->height() - (r.y() + r.height()),
                  r.width(), r.height());
    } else {
        d->scissor = QRect(0, 0, -2, -2);
        glScissor(0, 0, 0, 0);
    }
    glEnable(GL_SCISSOR_TEST);
}

/*!
    Expands the current scissor rectangle to also include the region
    defined by \a rect and sets the expanded region as the new scissor.
    The \a rect will be ignored if it is empty, or if the scissor is
    currently disabled.

    The \a rect is assumed to be in window co-ordinates, with the
    origin at the top-left of the drawing surface.

    \sa setScissor(), intersectScissor()
*/
void QGLPainter::expandScissor(const QRect& rect)
{
    if (rect.isEmpty())
        return;
    QRect current = scissor();
    if (current.width() == -2)
        setScissor(rect);
    else if (!current.isNull())
        setScissor(rect.united(current));
}

/*!
    Resets this painter's notion of what the current scissor()
    is set to.  The next time scissor() is called, the actual
    scissor rectangle will be fetched from the GL server.

    This function is used to synchronize the state of the application
    with the GL server after the execution of raw GL commands that may
    have altered the scissor settings.

    \sa scissor(), setScissor()
*/
void QGLPainter::resetScissor()
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->scissor = QRect(0, 0, -3, -3);
}

/*!
    Returns a reference to the projection matrix stack.

    It is recommended that setCamera() be used to set the projection
    matrix at the beginning of a scene rendering pass so that the
    eye position can be adjusted for stereo.

    \sa modelViewMatrix(), combinedMatrix(), setCamera()
*/
QGLMatrixStack& QGLPainter::projectionMatrix()
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(*((QGLMatrixStack *)0));
    return d->projectionMatrix;
}

/*!
    Returns a reference to the modelview matrix stack.

    \sa projectionMatrix(), combinedMatrix(), normalMatrix(), setCamera()
*/
QGLMatrixStack& QGLPainter::modelViewMatrix()
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(*((QGLMatrixStack *)0));
    return d->modelViewMatrix;
}

/*!
    \fn QMatrix4x4 QGLPainter::combinedMatrix() const

    Returns the result of multiplying the projectionMatrix()
    and the modelViewMatrix().  This combined matrix value is
    useful for setting uniform matrix values on shader programs.

    Calling this function is more efficient than calling
    projectionMatrix() and modelViewMatrix() separately and
    multiplying the return values.

    \sa projectionMatrix(), modelViewMatrix(), normalMatrix()
*/
// Implemented in qglmatrixstack.cpp.

/*!
    \fn QMatrix3x3 QGLPainter::normalMatrix() const

    Returns the normal matrix corresponding to modelViewMatrix().

    The normal matrix is the transpose of the inverse of the top-left
    3x3 part of the 4x4 modelview matrix.  If the 3x3 sub-matrix is not
    invertible, this function returns the identity.

    \sa modelViewMatrix(), combinedMatrix()
*/
// Implemented in qglmatrixstack.cpp.

/*!
    Returns the camera eye that is currently being used for stereo
    rendering.  The default is QGL::NoEye.

    The eye is used to adjust the camera position by a small amount
    when setCamera() is called.

    \sa setEye(), setCamera()
*/
QGL::Eye QGLPainter::eye() const
{
    Q_D(const QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(QGL::NoEye);
    return d->eye;
}

/*!
    Sets the camera \a eye that is currently being used for stereo
    rendering.

    The \a eye is used to adjust the camera position by a small amount
    when setCamera() is called.

    \sa eye(), setCamera()
*/
void QGLPainter::setEye(QGL::Eye eye)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->eye = eye;
}

/*!
    Sets the modelViewMatrix() and projectionMatrix() to the view
    defined by \a camera.  If eye() is not QGL::NoEye, then the view
    will be adjusted for the camera's eye separation.

    This function is typically called at the beginning of a scene rendering
    pass to initialize the modelview and projection matrices.

    \sa eye(), modelViewMatrix(), projectionMatrix()
*/
void QGLPainter::setCamera(QGLCamera *camera)
{
    Q_ASSERT(camera);
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->modelViewMatrix = camera->modelViewMatrix(d->eye);
    if (camera->projectionType() != QGLCamera::Orthographic2D)
        d->projectionMatrix = camera->projectionMatrix(aspectRatio());
    else
        d->projectionMatrix = camera->projectionMatrix2D(viewport(), d->eye);
}

/*!
    Returns true if \a point is outside the current viewing volume.
    This is used to perform object culling checks.
*/
bool QGLPainter::isCullable(const QVector3D& point) const
{
    Q_D(const QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(false);
    QVector3D projected = d->modelViewMatrix * point;
    projected = d->projectionMatrix * projected;
    return !d->viewingCube.contains(projected);
}

/*!
    Returns true if \a box is completely outside the current viewing volume.
    This is used to perform object culling checks.
*/
bool QGLPainter::isCullable(const QBox3D& box) const
{
    Q_D(const QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(false);
    QBox3D projected = box.transformed(d->modelViewMatrix);
    if (projected.minimum().z() >= 0.0f || projected.maximum().z() >= 0.0f) {
        // The box crosses the eye line in the view.  Don't do the
        // projection or the math will go all strange with a
        // perspective projection.  Just assume that it is cullable
        // if it passes the eye line, and hence is definitely outside
        // the viewing volume.  Note that it is possible that the box is
        // half in front of the eye and half behind, which we handle now
        // by truncating the box at the eye plane.
        if (projected.minimum().z() >= 0.0f)
            return true;
        projected.setExtents(projected.minimum(),
                             QVector3D(projected.maximum().x(),
                                       projected.maximum().y(), 0.0f));
    }
    projected.transform(d->projectionMatrix);
    return !d->viewingCube.intersects(projected);
}

/*!
    Returns the aspect ratio of the viewport() for adjusting projection
    transformations.
*/
qreal QGLPainter::aspectRatio() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(1.0f);

    // Get the size of the current viewport.
    QSize size = viewport().size();
    if (size.width() == 0 || size.height() == 0 ||
            size.width() == size.height())
        return 1.0f;

    // Use the device's DPI setting to determine the pixel aspect ratio.
    QPaintDevice *device = d->context->device();
    int dpiX = device->logicalDpiX();
    int dpiY = device->logicalDpiY();
    if (dpiX <= 0 || dpiY <= 0)
        dpiX = dpiY = 1;

    // Return the final aspect ratio based on viewport and pixel size.
    return ((qreal)(size.width() * dpiY)) / ((qreal)(size.height() * dpiX));
}

/*!
    Returns the current effect that is in use, which is userEffect()
    if it is not null, or the effect object associated with
    standardEffect() otherwise.

    If isPicking() is true, then this will return the effect object
    that is being used to generate pick colors.

    \sa userEffect(), standardEffect(), isPicking()
*/
QGLAbstractEffect *QGLPainter::effect() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(0);
    d->ensureEffect(const_cast<QGLPainter *>(this));
    return d->effect;
}

/*!
    Returns the user-defined effect that is being used for drawing
    operations, or null if standardEffect() is in use.

    \sa setUserEffect(), standardEffect(), effect()
*/
QGLAbstractEffect *QGLPainter::userEffect() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(0);
    return d->userEffect;
}

/*!
    Sets a user-defined \a effect to use for drawing operations
    in the current GL context.  If \a effect is null, this will
    disable user-defined effects and return to using standardEffect().

    An effect must be selected before any vertex attributes are
    specified that apply to that effect.  The caller must re-establish
    the vertex attribute state after changing effects.

    \sa effect(), draw(), setStandardEffect()
*/
void QGLPainter::setUserEffect(QGLAbstractEffect *effect)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (d->userEffect == effect)
        return;
    if (d->effect)
        d->effect->setActive(this, false);
    d->userEffect = effect;
    if (effect && (!d->pick || !d->pick->isPicking)) {
        d->effect = effect;
        d->effect->setActive(this, true);
        d->setRequiredFields(effect->requiredFields());
        d->updates = UpdateAll;
    } else {
        // Revert to the effect associated with standardEffect().
        d->effect = 0;
        d->ensureEffect(this);
    }
}

/*!
    Returns the standard effect to use for rendering fragments in
    the current GL context when userEffect() is null.

    \sa setStandardEffect(), userEffect()
*/
QGL::StandardEffect QGLPainter::standardEffect() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(QGL::FlatColor);
    return d->standardEffect;
}

/*!
    Sets a standard \a effect to use for rendering fragments
    in the current GL context.  This will also set userEffect()
    to null.  If \a effect is an invalid value, then the behavior
    of QGL::FlatColor will be used instead.

    \sa standardEffect(), setUserEffect()
*/
void QGLPainter::setStandardEffect(QGL::StandardEffect effect)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (d->standardEffect == effect && d->effect && d->userEffect == 0)
        return;
    if (d->effect)
        d->effect->setActive(this, false);
    d->standardEffect = effect;
    d->userEffect = 0;
    d->effect = 0;
    d->ensureEffect(this);
}

/*!
    Disables the current effect and sets userEffect() to null.
    Unlike setUserEffect() this not activate the standardEffect()
    until the next time effect() is called.

    This function can be used to disable all effect-based drawing
    operations prior to performing raw GL calls.  The next time
    effect() is called on this QGLPainter, the standardEffect()
    will be reactivated.  An effect can also be reactivated by
    calling setUserEffect() or setStandardEffect().

    \sa userEffect(), standardEffect()
*/
void QGLPainter::disableEffect()
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (d->effect)
        d->effect->setActive(this, false);
    d->userEffect = 0;
    d->effect = 0;
}

/*!
    Returns the cached shader program associated with \a name; or null
    if \a name is not currently associated with a shader program.

    \sa setCachedProgram()
*/
QGLShaderProgram *QGLPainter::cachedProgram(const QString& name) const
{
#if !defined(QT_OPENGL_ES_1)
    Q_D(const QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    return d->cachedPrograms.value(name, 0);
#else
    Q_UNUSED(name);
    return 0;
#endif
}

/*!
    Sets the cached shader \a program associated with \a name.

    Effect objects can use this function to store pre-compiled
    and pre-linked shader programs in the painter for future
    use by the same effect.  The \a program will be destroyed
    when context() is destroyed.

    If \a program is null, then the program associated with \a name
    will be destroyed.  If \a name is already present as a cached
    program, then it will be replaced with \a program.

    Names that start with "\c{qt.}" are reserved for use by Qt's
    internal effects.

    \sa cachedProgram()
*/
void QGLPainter::setCachedProgram
    (const QString& name, QGLShaderProgram *program)
{
#if !defined(QT_OPENGL_ES_1)
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    QGLShaderProgram *current = d->cachedPrograms.value(name, 0);
    if (current != program) {
        if (program)
            d->cachedPrograms[name] = program;
        else
            d->cachedPrograms.remove(name);
        delete current;
    }
#else
    // Wouldn't normally be called, but clean up anyway.
    Q_UNUSED(name);
    delete program;
#endif
}

void QGLPainterPrivate::createEffect(QGLPainter *painter)
{
    if (userEffect) {
        if (!pick || !pick->isPicking) {
            effect = userEffect;
            effect->setActive(painter, true);
            setRequiredFields(effect->requiredFields());
            updates = QGLPainter::UpdateAll;
            return;
        }
        if (userEffect->supportsPicking()) {
            effect = userEffect;
            effect->setActive(painter, true);
            setRequiredFields(effect->requiredFields());
            updates = QGLPainter::UpdateAll;
            return;
        }
        effect = pick->defaultPickEffect;
        effect->setActive(painter, true);
        setRequiredFields(effect->requiredFields());
        updates = QGLPainter::UpdateAll;
        return;
    }
    if (uint(standardEffect) >= QGL_MAX_STD_EFFECTS)
        effect = stdeffects[int(QGL::FlatColor)];
    else
        effect = stdeffects[int(standardEffect)];
    if (!effect) {
        switch (standardEffect) {
        case QGL::FlatColor: default:
            effect = new QGLFlatColorEffect();
            break;
        case QGL::FlatPerVertexColor:
            effect = new QGLPerVertexColorEffect();
            break;
        case QGL::FlatReplaceTexture2D:
            effect = new QGLFlatTextureEffect();
            break;
        case QGL::FlatDecalTexture2D:
            effect = new QGLFlatDecalTextureEffect();
            break;
        case QGL::LitMaterial:
            effect = new QGLLitMaterialEffect();
            break;
        case QGL::LitDecalTexture2D:
            effect = new QGLLitDecalTextureEffect();
            break;
        case QGL::LitModulateTexture2D:
            effect = new QGLLitModulateTextureEffect();
            break;
        }
        if (uint(standardEffect) >= QGL_MAX_STD_EFFECTS)
            stdeffects[int(QGL::FlatColor)] = effect;
        else
            stdeffects[int(standardEffect)] = effect;
    }
    if (!pick || !pick->isPicking || effect->supportsPicking()) {
        effect->setActive(painter, true);
    } else {
        effect = pick->defaultPickEffect;
        effect->setActive(painter, true);
    }
    setRequiredFields(effect->requiredFields());
    updates = QGLPainter::UpdateAll;
}

#ifndef QT_NO_DEBUG

void QGLPainterPrivate::removeRequiredFields
    (const QList<QGL::VertexAttribute>& array)
{
    for (int index = 0; index < array.size(); ++index)
        requiredFields.removeAll(array[index]);
}

void QGLPainter::checkRequiredFields()
{
    Q_D(QGLPainter);
    if (d->requiredFields.isEmpty())
        return;
    for (int index = 0; index < d->requiredFields.size(); ++index) {
        QGL::VertexAttribute attr = d->requiredFields[index];
        switch (attr) {
        case QGL::Position:
            qWarning("Attribute QGL::Position is missing"); break;
        case QGL::Normal:
            qWarning("Attribute QGL::Normal is missing"); break;
        case QGL::Color:
            qWarning("Attribute QGL::Color is missing"); break;
        case QGL::TextureCoord0:
            qWarning("Attribute QGL::TextureCoord0 is missing"); break;
        case QGL::TextureCoord1:
            qWarning("Attribute QGL::TextureCoord1 is missing"); break;
        case QGL::TextureCoord2:
            qWarning("Attribute QGL::TextureCoord2 is missing"); break;
        case QGL::CustomVertex0:
            qWarning("Attribute QGL::CustomVertex0 is missing"); break;
        case QGL::CustomVertex1:
            qWarning("Attribute QGL::CustomVertex1 is missing"); break;
        case QGL::UserVertex:
            qWarning("Attribute QGL::UserVertex is missing"); break;
        default:
            qWarning("Attribute UserVertex + %d is missing",
                     (int)(attr - QGL::UserVertex)); break;
        }
    }
}

#else

static inline void checkRequiredFields() {}

#endif

/*!
    Returns the last color that was set with setColor().  The default
    value is (1, 1, 1, 1).

    \sa setColor()
*/
QColor QGLPainter::color() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(QColor());
    return d->color;
}

/*!
    Sets the default fragment \a color for effects associated
    with this painter.  This function does not apply the color
    to the effect until update() is called.

    \sa color(), update()
*/
void QGLPainter::setColor(const QColor& color)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->color = color;
    d->updates |= UpdateColor;
}

#if !defined(QT_OPENGL_ES)

typedef void (APIENTRY *q_PFNGLACTIVETEXTUREPROC) (GLenum);
typedef void (APIENTRY *q_PFNGLCLIENTACTIVETEXTUREPROC) (GLenum);

class QGLMultiTextureExtensions
{
public:
    QGLMultiTextureExtensions()
    {
        activeTexture = 0;
        clientActiveTexture = 0;
        multiTextureResolved = false;
    }

    q_PFNGLACTIVETEXTUREPROC activeTexture;
    q_PFNGLCLIENTACTIVETEXTUREPROC clientActiveTexture;
    bool multiTextureResolved;
};

static void qt_multitexture_funcs_free(void *data)
{
    delete reinterpret_cast<QGLMultiTextureExtensions *>(data);
}

Q_GLOBAL_STATIC_WITH_ARGS(QGLContextResource, qt_multitexture_funcs, (qt_multitexture_funcs_free))

static QGLMultiTextureExtensions *resolveMultiTextureExtensions
    (const QGLContext *ctx)
{
    QGLMultiTextureExtensions *extn =
        reinterpret_cast<QGLMultiTextureExtensions *>
            (qt_multitexture_funcs()->value(ctx));
    if (!extn) {
        extn = new QGLMultiTextureExtensions();
        qt_multitexture_funcs()->insert(ctx, extn);
    }
    if (!(extn->multiTextureResolved)) {
        extn->multiTextureResolved = true;
        if (!extn->activeTexture) {
            extn->activeTexture = (q_PFNGLACTIVETEXTUREPROC)
                ctx->getProcAddress(QLatin1String("glActiveTexture"));
        }
        if (!extn->activeTexture) {
            extn->activeTexture = (q_PFNGLACTIVETEXTUREPROC)
                ctx->getProcAddress(QLatin1String("glActiveTextureARB"));
        }
        if (!extn->clientActiveTexture) {
            extn->clientActiveTexture = (q_PFNGLCLIENTACTIVETEXTUREPROC)
                ctx->getProcAddress(QLatin1String("glClientActiveTexture"));
        }
        if (!extn->clientActiveTexture) {
            extn->clientActiveTexture = (q_PFNGLCLIENTACTIVETEXTUREPROC)
                ctx->getProcAddress(QLatin1String("glClientActiveTextureARB"));
        }
    }
    return extn;
}

void qt_gl_ClientActiveTexture(GLenum texture)
{
    const QGLContext *ctx = QGLContext::currentContext();
    if (!ctx)
        return;
    QGLMultiTextureExtensions *extn = resolveMultiTextureExtensions(ctx);
    if (extn->clientActiveTexture)
        extn->clientActiveTexture(texture);
}

void qt_gl_ActiveTexture(GLenum texture)
{
    const QGLContext *ctx = QGLContext::currentContext();
    if (!ctx)
        return;
    QGLMultiTextureExtensions *extn = resolveMultiTextureExtensions(ctx);
    if (extn->activeTexture)
        extn->activeTexture(texture);
}

#endif

void qt_gl_setVertexAttribute(QGL::VertexAttribute attribute, const QGLAttributeValue& value)
{
#if !defined(QT_OPENGL_ES_2)
    switch (attribute) {
    case QGL::Position:
        glVertexPointer(value.tupleSize(), value.type(),
                        value.stride(), value.data());
        break;

    case QGL::Normal:
        if (value.tupleSize() == 3)
            glNormalPointer(value.type(), value.stride(), value.data());
        break;

    case QGL::Color:
        glColorPointer(value.tupleSize(), value.type(),
                       value.stride(), value.data());
        break;

#ifdef GL_TEXTURE_COORD_ARRAY
    case QGL::TextureCoord0:
    case QGL::TextureCoord1:
    case QGL::TextureCoord2:
    {
        int unit = (int)(attribute - QGL::TextureCoord0);
        qt_gl_ClientActiveTexture(GL_TEXTURE0 + unit);
        glTexCoordPointer(value.tupleSize(), value.type(),
                          value.stride(), value.data());
        if (unit != 0)  // Stay on unit 0 between requests.
            qt_gl_ClientActiveTexture(GL_TEXTURE0);
    }
    break;
#endif

    default: break;
    }
#else
    Q_UNUSED(attribute);
    Q_UNUSED(value);
#endif
}

void QGLAbstractEffect::setVertexAttribute(QGL::VertexAttribute attribute, const QGLAttributeValue& value)
{
    qt_gl_setVertexAttribute(attribute, value);
}

/*!
    Sets a vertex \a attribute on the current GL context to \a value.

    The effect() is notified via QGLAbstractEffect::setVertexAttribute()
    about the new attribute value, and is responsible for setting it
    on the GL state.  If the effect() does not need \a attribute,
    it will be ignored.

    \sa setVertexBuffer(), draw(), setCommonNormal()
*/
void QGLPainter::setVertexAttribute
    (QGL::VertexAttribute attribute, const QGLAttributeValue& value)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->ensureEffect(this);
    if (d->boundVertexBuffer) {
        QGLBuffer::release(QGLBuffer::VertexBuffer);
        d->boundVertexBuffer = 0;
    }
    d->effect->setVertexAttribute(attribute, value);
    d->removeRequiredField(attribute);
}

/*!
    Sets the vertex attributes on the current GL context that are
    stored in \a buffer.

    The effect() is notified via QGLAbstractEffect::setVertexAttribute()
    about the new attribute values, and is responsible for setting it
    on the GL state.  If the effect() does not need an attribute
    that is stored within \a buffer, it will be ignored.

    \sa setVertexAttribute(), draw(), setCommonNormal()
*/
void QGLPainter::setVertexBuffer(const QGLVertexBuffer& buffer)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->ensureEffect(this);
    const QGLVertexBufferPrivate *bd = buffer.d_func();
    if (bd->buffer) {
        GLuint id = bd->buffer->bufferId();
        if (id != d->boundVertexBuffer) {
            bd->buffer->bind();
            d->boundVertexBuffer = id;
        }
    } else if (d->boundVertexBuffer) {
        QGLBuffer::release(QGLBuffer::VertexBuffer);
        d->boundVertexBuffer = 0;
    }
    for (int index = 0; index < bd->attributes.size(); ++index) {
        QGLVertexBufferAttribute *attr = bd->attributes[index];
        d->effect->setVertexAttribute(attr->attribute, attr->value);
    }
#ifndef QT_NO_DEBUG
    d->removeRequiredFields(bd->attributeNames);
#endif
}

/*!
    Sets the common normal value on the current effect() for all
    vertices to \a value and disable any active normal arrays.

    \sa setVertexAttribute()
*/
void QGLPainter::setCommonNormal(const QVector3D& value)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->ensureEffect(this);
    d->effect->setCommonNormal(value);
#ifndef QT_NO_DEBUG
    d->requiredFields.removeAll(QGL::Normal);
#endif
}

/*!
    Returns the number of texture units associated with the
    current GL context.

    \sa setTexture()
*/
int QGLPainter::textureUnitCount() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(0);

    if (!(d->textureUnitCount)) {
#if defined(GL_MAX_TEXTURE_UNITS)
        glGetIntegerv(GL_MAX_TEXTURE_UNITS, &d->textureUnitCount);
#elif defined(GL_MAX_TEXTURE_UNITS_ARB)
        glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &d->textureUnitCount);
#elif defined(GL_MAX_TEXTURE_IMAGE_UNITS)
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &d->textureUnitCount);
#endif
        if (!(d->textureUnitCount)) {
            // Multitexturing is not available, so report only 1 unit.
            d->textureUnitCount = 1;
        }
    }

    return d->textureUnitCount;
}

/*!
    Sets a texture \a unit to the contents of \a texture.
    If \a texture is null, the texture unit will be disabled.

    \sa textureUnitCount()
*/
void QGLPainter::setTexture(int unit, const QGLTexture2D *texture)
{
    //If we are using the painter to determine pick IDs we shouldn't use 
    //texturing: it 'corrupts' the picking colours and results in 
    //erroneous pick IDs.
    if (this->isPicking()) return;

    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (unit < 0)
        return;

    // Select the texture unit and bind the texture.
    qt_gl_ActiveTexture(GL_TEXTURE0 + unit);
    if (!texture) {
        glBindTexture(GL_TEXTURE_2D, 0);
#if !defined(QT_OPENGL_ES_2)
        glDisable(GL_TEXTURE_2D);
#endif
    } else {
        texture->bind();
#if !defined(QT_OPENGL_ES_2)
        glEnable(GL_TEXTURE_2D);
#endif
    }

    // Leave the default setting on texture unit 0 just in case
    // raw GL code is being mixed in with QGLPainter code.
    if (unit != 0)
        qt_gl_ActiveTexture(GL_TEXTURE0);
}

/*!
    Sets a texture \a unit to the contents of \a texture.
    If \a texture is null, the texture unit will be disabled.

    \sa textureUnitCount()
*/
void QGLPainter::setTexture(int unit, const QGLTextureCube *texture)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (unit < 0)
        return;

    // Select the texture unit and bind the texture.
    qt_gl_ActiveTexture(GL_TEXTURE0 + unit);
    if (!texture) {
        QGLTextureCube::release();
#if !defined(QT_OPENGL_ES_2)
        glDisable(GL_TEXTURE_CUBE_MAP);
#endif
    } else {
        texture->bind();
#if !defined(QT_OPENGL_ES_2)
        glEnable(GL_TEXTURE_CUBE_MAP);
#endif
    }

    // Leave the default setting on texture unit 0 just in case
    // raw GL code is being mixed in with QGLPainter code.
    if (unit != 0)
        qt_gl_ActiveTexture(GL_TEXTURE0);
}

/*!
    \fn void QGLPainter::setTexture(const QGLTexture2D *texture)

    Sets texture unit 0 to the contents of \a texture.
    If \a texture is null, the texture unit will be disabled.

    \sa textureUnitCount()
*/

/*!
    \fn void QGLPainter::setTexture(const QGLTextureCube *texture)

    Sets texture unit 0 to the contents of \a texture.
    If \a texture is null, the texture unit will be disabled.

    \sa textureUnitCount()
*/

/*!
    Updates the projection, modelview, texture matrices, and lighting
    conditions in the currently active effect() object by calling
    QGLAbstractEffect::update().

    Normally this function is called automatically by draw().
    However, if the user wishes to use raw GL functions to draw fragments,
    it will be necessary to explicitly call this function to ensure that
    the matrix state and lighting conditions have been set on the
    active effect().

    Note that this function informs the effect that an update is needed.
    It does not change the GL state itself.  In particular, the
    modelview and projection matrices in the fixed-function pipeline
    are not changed unless the effect or application calls
    updateFixedFunction().

    \sa setUserEffect(), projectionMatrix(), modelViewMatrix()
    \sa draw(), updateFixedFunction()
*/
void QGLPainter::update()
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->ensureEffect(this);
    QGLPainter::Updates updates = d->updates;
    d->updates = 0;
    if (d->modelViewMatrix.needsUpdate())
        updates |= UpdateModelViewMatrix;
    if (d->projectionMatrix.needsUpdate())
        updates |= UpdateProjectionMatrix;
    if (updates != 0)
        d->effect->update(this, updates);
}

#if !defined(QT_OPENGL_ES_2)

static void setLight(int light, const QGLLightParameters *parameters,
                     const QMatrix4x4& transform)
{
    GLfloat params[4];

    QColor color = parameters->ambientColor();
    params[0] = color.redF();
    params[1] = color.greenF();
    params[2] = color.blueF();
    params[3] = color.alphaF();
    glLightfv(light, GL_AMBIENT, params);

    color = parameters->diffuseColor();
    params[0] = color.redF();
    params[1] = color.greenF();
    params[2] = color.blueF();
    params[3] = color.alphaF();
    glLightfv(light, GL_DIFFUSE, params);

    color = parameters->specularColor();
    params[0] = color.redF();
    params[1] = color.greenF();
    params[2] = color.blueF();
    params[3] = color.alphaF();
    glLightfv(light, GL_SPECULAR, params);

    QVector4D vector = parameters->eyePosition(transform);
    params[0] = vector.x();
    params[1] = vector.y();
    params[2] = vector.z();
    params[3] = vector.w();
    glLightfv(light, GL_POSITION, params);

    QVector3D spotDirection = parameters->eyeSpotDirection(transform);
    params[0] = spotDirection.x();
    params[1] = spotDirection.y();
    params[2] = spotDirection.z();
    glLightfv(light, GL_SPOT_DIRECTION, params);

    params[0] = parameters->spotExponent();
    glLightfv(light, GL_SPOT_EXPONENT, params);

    params[0] = parameters->spotAngle();
    glLightfv(light, GL_SPOT_CUTOFF, params);

    params[0] = parameters->constantAttenuation();
    glLightfv(light, GL_CONSTANT_ATTENUATION, params);

    params[0] = parameters->linearAttenuation();
    glLightfv(light, GL_LINEAR_ATTENUATION, params);

    params[0] = parameters->quadraticAttenuation();
    glLightfv(light, GL_QUADRATIC_ATTENUATION, params);
}

static void setMaterial(int face, const QGLMaterial *parameters)
{
    GLfloat params[17];

    QColor mcolor = parameters->ambientColor();
    params[0] = mcolor.redF();
    params[1] = mcolor.greenF();
    params[2] = mcolor.blueF();
    params[3] = mcolor.alphaF();

    mcolor = parameters->diffuseColor();
    params[4] = mcolor.redF();
    params[5] = mcolor.greenF();
    params[6] = mcolor.blueF();
    params[7] = mcolor.alphaF();

    mcolor = parameters->specularColor();
    params[8] = mcolor.redF();
    params[9] = mcolor.greenF();
    params[10] = mcolor.blueF();
    params[11] = mcolor.alphaF();

    mcolor = parameters->emittedLight();
    params[12] = mcolor.redF();
    params[13] = mcolor.greenF();
    params[14] = mcolor.blueF();
    params[15] = mcolor.alphaF();

    params[16] = parameters->shininess();

    glMaterialfv(face, GL_AMBIENT, params);
    glMaterialfv(face, GL_DIFFUSE, params + 4);
    glMaterialfv(face, GL_SPECULAR, params + 8);
    glMaterialfv(face, GL_EMISSION, params + 12);
    glMaterialfv(face, GL_SHININESS, params + 16);
}

#endif // !QT_OPENGL_ES_2

/*!
    Updates the fixed-function pipeline with the current painting
    state according to the flags in \a updates.

    This function is intended for use by effects in their
    QGLAbstractEffect::update() override if they are using the
    fixed-function pipeline.  It can also be used by user
    applications if they need the QGLPainter state to be
    set in the fixed-function pipeline.

    If the OpenGL implementation does not have a fixed-function
    pipeline, e.g. OpenGL/ES 2.0, this function does nothing.

    \sa update()
*/
void QGLPainter::updateFixedFunction(QGLPainter::Updates updates)
{
#if defined(QT_OPENGL_ES_2)
    Q_UNUSED(updates);
#else
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if ((updates & QGLPainter::UpdateColor) != 0) {
        QColor color;
        if (isPicking())
            color = pickColor();
        else
            color = this->color();
        glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    }
    if ((updates & QGLPainter::UpdateModelViewMatrix) != 0)
        d->modelViewMatrix.updateServer();
    if ((updates & QGLPainter::UpdateProjectionMatrix) != 0)
        d->projectionMatrix.updateServer();
    if ((updates & QGLPainter::UpdateLights) != 0) {
        // Save the current modelview matrix and load the identity.
        // We need to apply the light in the modelview transformation
        // that was active when the light was specified.
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // Enable the main light.
        const QGLLightParameters *params = mainLight();
        setLight(GL_LIGHT0, params, mainLightTransform());

        // Restore the previous modelview transformation.
        glPopMatrix();

        // Set up the light model parameters if at least one light is enabled.
        const QGLLightModel *lightModel = this->lightModel();
        GLfloat values[4];
#ifdef GL_LIGHT_MODEL_TWO_SIDE
        if (lightModel->model() == QGLLightModel::TwoSided)
            values[0] = 1.0f;
        else
            values[0] = 0.0f;
        glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, values);
#endif
#ifdef GL_LIGHT_MODEL_COLOR_CONTROL
        if (lightModel->colorControl() == QGLLightModel::SeparateSpecularColor)
            values[0] = GL_SEPARATE_SPECULAR_COLOR;
        else
            values[0] = GL_SINGLE_COLOR;
        glLightModelfv(GL_LIGHT_MODEL_COLOR_CONTROL, values);
#endif
#ifdef GL_LIGHT_MODEL_LOCAL_VIEWER
        if (lightModel->viewerPosition() == QGLLightModel::LocalViewer)
            values[0] = 1.0f;
        else
            values[0] = 0.0f;
        glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, values);
#endif
#ifdef GL_LIGHT_MODEL_AMBIENT
        QColor color = lightModel->ambientSceneColor();
        values[0] = color.redF();
        values[1] = color.blueF();
        values[2] = color.greenF();
        values[3] = color.alphaF();
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, values);
#endif
    }
    if ((updates & QGLPainter::UpdateMaterials) != 0) {
        const QGLMaterial *frontMaterial = faceMaterial(QGL::FrontFaces);
        const QGLMaterial *backMaterial = faceMaterial(QGL::BackFaces);
        if (frontMaterial == backMaterial) {
            setMaterial(GL_FRONT_AND_BACK, frontMaterial);
        } else {
            setMaterial(GL_FRONT, frontMaterial);
            setMaterial(GL_BACK, backMaterial);
        }
    }
    if ((updates & QGLPainter::UpdateFog) != 0) {
        const QGLFogParameters *fog = fogParameters();
        if (!fog) {
            glDisable(GL_FOG);
        } else {
            GLfloat color[4];
            QColor col = fog->color();
            color[0] = col.redF();
            color[1] = col.greenF();
            color[2] = col.blueF();
            color[3] = col.alphaF();
            int fmode;
            QGLFogParameters::Mode mode = fog->mode();
            if (mode == QGLFogParameters::Linear)
                fmode = GL_LINEAR;
            else if (mode == QGLFogParameters::Exponential)
                fmode = GL_EXP;
            else if (mode == QGLFogParameters::Exponential2)
                fmode = GL_EXP2;
            else
                fmode = GL_LINEAR;  // Just in case.
            glFogf(GL_FOG_MODE, fmode);
            glFogf(GL_FOG_DENSITY, fog->density());
            glFogf(GL_FOG_START, fog->nearDistance());
            glFogf(GL_FOG_END, fog->farDistance());
            glFogfv(GL_FOG_COLOR, color);
            glEnable(GL_FOG);
        }
    }
#endif
}

/*!
    Draws primitives using \a count vertices from the arrays specified
    by setVertexAttribute().  The type of primitive to draw is specified
    by \a mode.

    This operation will consume \a count values from the
    enabled arrays, starting at \a index.

    \sa update()
*/
void QGLPainter::draw(QGL::DrawingMode mode, int count, int index)
{
    update();
    checkRequiredFields();
    glDrawArrays((GLenum)mode, index, count);
}

/*!
    \overload

    Draws primitives using vertices from the arrays specified by
    setVertexAttribute().  The type of primitive to draw is
    specified by \a mode.

    This operation will consume \a count elements of \a indices,
    which are used to index into the enabled arrays.

    \sa update()
*/
void QGLPainter::draw(QGL::DrawingMode mode, const ushort *indices, int count)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    update();
    checkRequiredFields();
    if (d->boundIndexBuffer) {
        QGLBuffer::release(QGLBuffer::IndexBuffer);
        d->boundIndexBuffer = 0;
    }
    glDrawElements(GLenum(mode), count, GL_UNSIGNED_SHORT, indices);
}

/*!
    Pushes \a fbo onto the surface stack and makes it the current
    drawing surface for context().  If \a fbo is null, then the
    current drawing surface will be set to the default (e.g. the window).

    Note: the \a fbo object must remain valid until popped from
    the stack or end() is called.  All surfaces are popped from
    the stack by end().

    \sa popSurface(), currentSurface(), surfaceSize()
*/
void QGLPainter::pushSurface(QGLFramebufferObject *fbo)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(0);
    QGLFramebufferObject *fboTop;
    if (d->surfaceStack.isEmpty())
        fboTop = 0;
    else
        fboTop = d->surfaceStack.last();
    d->surfaceStack.append(fbo);
    if (fbo != fboTop) {
        if (fbo)
            fbo->bind();
        else if (fboTop)
            fboTop->release();
    }
}

/*!
    Pops the top-most framebuffer object from the surface stack
    and returns it.  The next object on the stack will be made
    the current drawing surface for context().  Does nothing
    if the surface stack is empty when this function is called.

    \sa pushSurface(), currentSurface()
*/
QGLFramebufferObject *QGLPainter::popSurface()
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(0);
    if (d->surfaceStack.isEmpty())
        return 0;
    QGLFramebufferObject *fbo = d->surfaceStack.takeLast();
    QGLFramebufferObject *fboNext;
    if (d->surfaceStack.isEmpty())
        fboNext = 0;
    else
        fboNext = d->surfaceStack.last();
    if (fboNext)
        fboNext->bind();
    else if (fbo)
        fbo->release();
    return fbo;
}

/*!
    Returns the framebuffer object corresponding to the current
    drawing surface; null if using the default drawing surface.

    \sa pushSurface(), popSurface(), surfaceSize()
*/
QGLFramebufferObject *QGLPainter::currentSurface() const
{
    Q_D(const QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (d->surfaceStack.isEmpty())
        return 0;
    else
        return d->surfaceStack.last();
}

/*!
    Returns the size of the current drawing surface.

    This function is typically used after calling pushSurface() or
    popSurface() to readjust the viewport() for the dimensions of
    the current drawing surface.

    \code
    painter.pushSurface(fbo);
    painter.setViewport(painter.surfaceSize());
    ...
    painter.popSurface();
    painter.setViewport(painter.surfaceSize());
    \endcode

    \sa currentSurface(), viewport()
*/
QSize QGLPainter::surfaceSize() const
{
    Q_D(const QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(QSize());
    QGLFramebufferObject *fbo;
    if (!d->surfaceStack.isEmpty() && (fbo = d->surfaceStack.last()) != 0)
        return fbo->size();
    QPaintDevice *device = d->context->device();
    return QSize(device->width(), device->height());
}

/*!
    Sets the point \a size to use with draw().

    This function has no effect if a shader program is in use,
    or on OpenGL/ES 2.0.  Shader programs must set the point size
    in the vertex shader.

    \sa draw(), setLineWidth()
*/
void QGLPainter::setPointSize(qreal size)
{
#if defined(QT_OPENGL_ES_2)
    Q_UNUSED(size);
#else
    glPointSize(size);
#endif
}

/*!
    Sets the line \a width to use with draw().

    \sa draw(), setPointSize()
*/
void QGLPainter::setLineWidth(qreal width)
{
    glLineWidth(width);
}

/*!
    Sets the \a faces to be culled based on whether they are front
    or back faces.
*/
void QGLPainter::setCullFaces(QGL::CullFaces faces)
{
    if ((faces & ~QGL::CullClockwise) == QGL::CullDisabled) {
        glDisable(GL_CULL_FACE);
    } else if (faces & QGL::CullClockwise) {
        glFrontFace(GL_CW);
        glCullFace((GLenum)(faces & ~QGL::CullClockwise));
        glEnable(GL_CULL_FACE);
    } else {
        glFrontFace(GL_CCW);
        glCullFace((GLenum)faces);
        glEnable(GL_CULL_FACE);
    }
}

/*!
    Returns the current lighting model.

    \sa setLightModel()
*/
const QGLLightModel *QGLPainter::lightModel() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(QGLLightModel());
    if (!d->lightModel) {
        if (!d->defaultLightModel)
            d->defaultLightModel = new QGLLightModel();
        d->lightModel = d->defaultLightModel;
    }
    return d->lightModel;
}

/*!
    Sets the current lighting model to \a value.  If \a value is
    null, then the default lighting model parameters will be used.

    The light settings in the GL server will not be changed until
    update() is called.

    \sa lightModel()
*/
void QGLPainter::setLightModel(const QGLLightModel *value)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->lightModel = value;
    d->updates |= QGLPainter::UpdateLights;
}

/*!
    Returns the parameters for the main light in the scene.

    The light parameters are specified in world co-ordinates at
    the point when setMainLight() was called.  The mainLightTransform()
    must be applied to obtain eye co-ordinates.

    \sa setMainLight(), mainLightTransform()
*/
const QGLLightParameters *QGLPainter::mainLight() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(0);
    if (!d->mainLight) {
        if (!d->defaultLight)
            d->defaultLight = new QGLLightParameters();
        return d->defaultLight;
    } else {
        return d->mainLight;
    }
}

/*!
    Sets the \a parameters for the main light in the scene.
    The mainLightTransform() is set to the current modelViewMatrix().

    Light parameters are stored in world co-ordinates, not eye co-ordinates.
    The mainLightTransform() specifies the transformation to apply to
    convert the world co-ordinates into eye co-ordinates when the light
    is used.

    Note: the \a parameters may be ignored by effect() if it
    has some other way to determine the lighting conditions.

    The light settings in the GL server will not be changed until
    update() is called.

    If \a parameters is null, then mainLight() will be set to a
    default internal light object.

    \sa mainLight(), mainLightTransform()
*/
void QGLPainter::setMainLight(QGLLightParameters *parameters)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->mainLight = parameters;
    d->mainLightTransform = modelViewMatrix();
    d->updates |= QGLPainter::UpdateLights;
}

/*!
    Sets the \a parameters for the main light in the scene, and set
    mainLightTransform() to \a transform.

    Light parameters are stored in world co-ordinates, not eye co-ordinates.
    The \a transform specifies the transformation to apply to convert the
    world co-ordinates into eye co-ordinates when the light is used.

    Note: the \a parameters may be ignored by effect() if it
    has some other way to determine the lighting conditions.

    The light settings in the GL server will not be changed until
    update() is called.

    If \a parameters is null, then mainLight() will be set to a
    default internal light object.

    \sa mainLight(), mainLightTransform()
*/
void QGLPainter::setMainLight
        (QGLLightParameters *parameters, const QMatrix4x4& transform)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->mainLight = parameters;
    d->mainLightTransform = transform;
    d->updates |= QGLPainter::UpdateLights;
}

/*!
    Returns the modelview transformation matrix for the main light that
    was set at the time setMainLight() was called.

    The light transform may be used by later painting operations to
    convert the light from world co-ordinates into eye co-ordinates.
    The eye transformation is set when the light is specified.

    \sa mainLight(), setMainLight()
*/
QMatrix4x4 QGLPainter::mainLightTransform() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(QMatrix4x4());
    return d->mainLightTransform;
}

/*!
    Returns the material that is used for drawing \a face on polygons.
    If \a face is QGL::FrontFaces or QGL::AllFaces, then the front
    material is returned.  If \a face is QGL::BackFaces, then the
    back material is returned.

    \sa setFaceMaterial(), setFaceColor()
*/
const QGLMaterial *QGLPainter::faceMaterial(QGL::Face face) const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(QGLMaterial());
    if (face == QGL::BackFaces) {
        if (!d->backMaterial) {
            if (!d->defaultMaterial)
                d->defaultMaterial = new QGLMaterial();
            d->backMaterial = d->defaultMaterial;
        }
        return d->backMaterial;
    } else {
        if (!d->frontMaterial) {
            if (!d->defaultMaterial)
                d->defaultMaterial = new QGLMaterial();
            d->frontMaterial = d->defaultMaterial;
        }
        return d->frontMaterial;
    }
}

/*!
    Sets the material that is used for drawing \a face on polygons
    to \a value.  If \a face is QGL::FrontFaces, then the front
    material is set.  If \a face is QGL::BackFaces, then the
    back material is set.  If \a face is QGL::AllFaces, then both
    the front and back materials are set.

    If \a value is null, then the \a face material will be set to
    the default material properties.

    The material settings in the GL server will not be changed until
    update() is called.

    \sa faceMaterial(), setFaceColor()
*/
void QGLPainter::setFaceMaterial
        (QGL::Face face, const QGLMaterial *value)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (face == QGL::FrontFaces) {
        if (d->frontMaterial == value)
            return;
        d->frontMaterial = value;
    } else if (face == QGL::BackFaces) {
        if (d->backMaterial == value)
            return;
        d->backMaterial = value;
    } else {
        if (d->frontMaterial == value && d->backMaterial == value)
            return;
        d->frontMaterial = value;
        d->backMaterial = value;
    }
    d->updates |= QGLPainter::UpdateMaterials;
}

static QGLMaterial *createColorMaterial
    (QGLMaterial *prev, const QColor& color)
{
    QGLMaterial *material;
    if (prev)
        material = prev;
    else
        material = new QGLMaterial();
    material->setColor(color);
    return material;
}

/*!
    Sets the material that is used for drawing \a face on polygons
    to \a color.  This is a convenience function for setting materials
    to simple colors.

    The RGB components of the ambient color of the material will be set
    to 20% of \a color, and the RGB components of the diffuse color of the
    material will be set to 80% of \a color.  The alpha components of
    the ambient and diffuse material colors will both be set to the
    alpha component of \a color.

    If \a face is QGL::FrontFaces, then the front material is set.
    If \a face is QGL::BackFaces, then the back material is set.
    If \a face is QGL::AllFaces, then both the front and back
    materials are set.

    The material settings in the GL server will not be changed until
    update() is called.

    \sa faceMaterial(), setFaceMaterial()
*/
void QGLPainter::setFaceColor(QGL::Face face, const QColor& color)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (face == QGL::FrontFaces) {
        d->frontColorMaterial =
            createColorMaterial(d->frontColorMaterial, color);
        d->frontMaterial = d->frontColorMaterial;
    } else if (face == QGL::BackFaces) {
        d->backColorMaterial =
            createColorMaterial(d->backColorMaterial, color);
        d->backMaterial = d->backColorMaterial;
    } else {
        d->frontColorMaterial =
            createColorMaterial(d->frontColorMaterial, color);
        d->backColorMaterial =
            createColorMaterial(d->backColorMaterial, color);
        d->frontMaterial = d->frontColorMaterial;
        d->backMaterial = d->backColorMaterial;
    }
    d->updates |= QGLPainter::UpdateMaterials;
}

/*!
    Returns the current fog parameters; or null if fog has been disabled.
    The default is that fog is disabled.

    \sa setFogParameters()
*/
const QGLFogParameters *QGLPainter::fogParameters() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(0);
    return d->fogParameters;
}

/*!
    Sets the current fog parameters to \a value.  If \a value is
    null, then fog will be disabled.

    The fog settings in the GL server will not be changed until
    update() is called, and if the effect() supports fogging.

    All standard effects support fogging on systems with fixed-function
    pipelines.  Under OpenGL/ES 2.0, fogging is not supported by the
    standard effects, but it can be implemented by user-defined effects.

    \sa fogParameters()
*/
void QGLPainter::setFogParameters(const QGLFogParameters *value)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->fogParameters = value;
    d->updates |= QGLPainter::UpdateFog;
}

/*!
    Returns true if this painter is in object picking mode;
    false if this painter is in normal rendering mode.

    \sa setPicking(), objectPickId()
*/
bool QGLPainter::isPicking() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(false);
    return (d->pick ? d->pick->isPicking : false);
}

/*!
    Enables or disables object picking mode according to \a value.

    If \a value is true, then the effect() will be overridden with a
    simple flat color effect that renders objects with pickColor().
    These colors can be read back later with pickObject().

    \sa isPicking(), objectPickId(), pickObject()
*/
void QGLPainter::setPicking(bool value)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (!d->pick)
        d->pick = new QGLPainterPickPrivate();
    if (d->pick->isPicking != value) {
        // Switch to/from the pick effect.
        d->pick->isPicking = value;
        if (d->effect)
            d->effect->setActive(this, false);
        d->effect = 0;
        d->ensureEffect(this);
    }
}

/*!
    Returns the current object pick identifier.  The default value
    is -1 which indicates that rendered objects should not have a
    pickColor() associated with them.

    \sa setObjectPickId(), clearPickObjects(), pickObject()
*/
int QGLPainter::objectPickId() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(-1);
    return (d->pick ? d->pick->objectPickId : -1);
}

/*!
    Sets the current object pick identifier to \a value.  If \a value
    is -1, then subsequent objects will be rendered without a pickColor().

    If value is not -1, then the pickColor() is changed to a color
    that represents that object pick identifier.  If \a value has been
    seen previously, then the same pickColor() as last time will
    be returned.

    The function call will be ignored if isPicking() is false.

    \sa objectPickId(), clearPickObjects(), pickObject()
*/
void QGLPainter::setObjectPickId(int value)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (!d->pick || !d->pick->isPicking)
        return;
    d->pick->objectPickId = value;
    if (value != -1) {
        QRgb color = d->pick->pickObjectToColor.value(value, 0);
        if (!color) {
            color = qt_qgl_pick_color(d->pick->pickColorIndex++);
            d->pick->pickObjectToColor[value] = color;
            d->pick->pickColorToObject[color] = value;
        }
        d->pick->pickColor = color;
        d->updates |= UpdateColor;
    } else {
        d->pick->pickColor = 0;
        d->updates |= UpdateColor;
    }
}

/*!
    Clears the objectPickId() to pickColor() mappings that
    were used previously.  This will also set objectPickId()
    to -1 and pickColor() to (0, 0, 0, 1).

    The function call will be ignored if isPicking() is false.

    \sa objectPickId(), pickColor()
*/
void QGLPainter::clearPickObjects()
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (d->pick && d->pick->isPicking) {
        d->pick->pickObjectToColor.clear();
        d->pick->pickColorToObject.clear();
        d->pick->pickColorIndex = 0;
        d->pick->objectPickId = -1;
        d->pick->pickColor = 0;
        d->updates |= UpdateColor;
    }
}

/*!
    Returns the current pick color to use to render the object
    associated with objectPickId().  The returned color will
    be (0, 0, 0, 1) if objectPickId() is -1.

    \sa objectPickId(), clearPickObjects()
*/
QColor QGLPainter::pickColor() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(QColor());
    if (d->pick) {
        QColor color;
        color.setRgb(d->pick->pickColor);        
        return color;
    } else {
        return Qt::black;
    }
}

/*!
    Picks the color at (\a x, \a y) in the color buffer and
    returns the objectPickId() that corresponds to that color.
    Returns -1 if (\a x, \a y) is not positioned over a
    recognized object.  The origin (0, 0) is assumed to be
    the bottom-left corner of the drawing surface.

    \sa objectPickId()
*/
int QGLPainter::pickObject(int x, int y) const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(-1);

    if (!d->pick)
    {
        return -1;
    }

    // Fetch the color at the specified pixel.
    unsigned char data[4] = {0, 0, 0, 0};
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    QRgb color = qRgb(data[0], data[1], data[2]);

    // Normalize the color to account for floating-point rounding.
    color = qt_qgl_normalize_pick_color(color); // XXX: detect RGB444 screens.
    
    // Map the color back to an object identifier.
    return d->pick->pickColorToObject.value(color, -1);
}

QT_END_NAMESPACE
