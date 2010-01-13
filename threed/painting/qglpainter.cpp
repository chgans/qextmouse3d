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

#include "qglpainter.h"
#include "qglpainter_p.h"
#include "qglabstracteffect.h"
#include <QtOpenGL/qglpixelbuffer.h>
// qglextensions_p.h defines GL_TEXTURE0, but we want to know
// if it doesn't actually exist on the underlying GL system.
#if defined(GL_TEXTURE0)
#define QGL_TEXTURE0        GL_TEXTURE0
#endif
#include <QtOpenGL/private/qgl_p.h>
#include <QtGui/private/qwidget_p.h>
#include <QtGui/private/qwindowsurface_p.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpaintengine.h>
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qmap.h>
#if !defined(QT_NO_THREAD)
#include <QtCore/qthreadstorage.h>
#endif
#include "qglflatcoloreffect_p.h"
#include "qglflattextureeffect_p.h"
#include "qgllitmaterialeffect_p.h"
#include "qgllittextureeffect_p.h"
#include "qglpickcolors_p.h"
#include "qgltexture2d.h"
#include "qgltexture2d_p.h"
#include "qgltexturecube.h"



QT_BEGIN_NAMESPACE

/*!
    \class QGLPainter
    \brief The QGLPainter class provides portable API's for rendering into a GL context.
    \since 4.6
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

#ifndef Q_WS_WIN
Q_GLOBAL_STATIC(QGLPainterExtensions, painterExtensions)
#endif

QGLPainterPrivate::QGLPainterPrivate()
    : ref(1),
      activePaintEngine(0),
      projectionMatrix(QGLMatrixStack::ProjectionMatrix),
      modelViewMatrix(QGLMatrixStack::ModelViewMatrix),
      lightModel(0),
      defaultLightModel(0),
      defaultLight0(0),
      defaultLight1(0),
      frontMaterial(0),
      backMaterial(0),
      defaultMaterial(0),
      frontColorMaterial(0),
      backColorMaterial(0),
      fogParameters(0),
      viewingCube(QVector3D(-1, -1, -1), QVector3D(1, 1, 1)),
      color(255, 255, 255, 255),
      updates(QGLPainter::UpdateAll),
      pick(0)
{
    context = 0;
    effect = 0;
    userEffect = 0;
    standardEffect = QGL::FlatColor;
    memset(stdeffects, 0, sizeof(stdeffects));
#ifdef Q_WS_WIN
    extensionFuncs = 0;
    shaderExtensionFuncs = 0;
#endif
    textureUnitCount = 0;
    texturesInUse = QBitArray(8);

    memset(lights, 0, sizeof(lights));
    enabledLights = 0;
    maxLights = 0;
    currentBufferId = 0;
}

QGLPainterPrivate::~QGLPainterPrivate()
{
    delete defaultLightModel;
    delete defaultLight0;
    delete defaultLight1;
    delete defaultMaterial;
    delete frontColorMaterial;
    delete backColorMaterial;
#ifdef Q_WS_WIN
    delete extensionFuncs;
    delete shaderExtensionFuncs;
#endif
    for (int effect = 0; effect < QGL_MAX_STD_EFFECTS; ++effect)
        delete stdeffects[effect];
    delete pick;
}

QGLPainterExtensions *QGLPainterPrivate::extensions()
{
#ifdef Q_WS_WIN
    if (!extensionFuncs)
        extensionFuncs = new QGLPainterExtensions();
    return extensionFuncs;
#else
    return painterExtensions();
#endif
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

    \sa begin(), isActive(), disableEffect()
*/
bool QGLPainter::end()
{
    if (!d_ptr)
        return false;
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
    Enables or disables depth testing according to \a value.
    This is a convience function that is equivalent to
    \c{glEnable(GL_DEPTH_TEST)} or \c{glDisable(GL_DEPTH_TEST)}.

    For more complex depth buffer configurations, use QGLDepthBufferOptions.

    \sa QGLDepthBufferOptions
*/
void QGLPainter::setDepthTestingEnabled(bool value)
{
    if (value)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

/*!
    Returns the viewport for the active GL context.  The origin for
    the returned rectangle is the top-left of the drawing surface.

    \sa setViewport()
*/
QRect QGLPainter::viewport() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(QRect());
    QPaintDevice *device = d->context->device();
    if (device) {
        GLint view[4];
        glGetIntegerv(GL_VIEWPORT, view);
        return QRect(view[0], device->height() - (view[1] + view[3]),
                     view[1], view[3]);
    } else {
        return QRect();
    }
}

/*!
    Sets the viewport for the active GL context to \a rect.
    The origin for \a rect is the top-left of the drawing surface.

    \sa viewport()
*/
void QGLPainter::setViewport(const QRect& rect)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    QPaintDevice *device = d->context->device();
    if (device) {
        int y = device->height() - (rect.y() + rect.height());
        glViewport(rect.x(), y, rect.width(), rect.height());
    }
}

/*!
    Sets the viewport for the active GL context to start at the
    origin and extend for \a size.

    \sa viewport()
*/
void QGLPainter::setViewport(const QSize& size)
{
    glViewport(0, 0, size.width(), size.height());
}

/*!
    Sets the viewport for the active GL context to start at the
    origin and extend for \a width and \a height.

    \sa viewport()
*/
void QGLPainter::setViewport(int width, int height)
{
    glViewport(0, 0, width, height);
}

/*!
    Returns the currently active scissor rectangle.  The rectangle
    will have zero size if scissoring is disabled.

    Performance note: if setScissorRect() has not been called on this
    QGLPainter, then this function will have to perform a round-trip
    to the GL server to get the scissor rectangle.  This round-trip
    can be avoided by calling setScissorRect() before scissorRect().

    Note: OpenGL/ES 1.0 does not have a mechanism to fetch the
    scissor rectangle.  On that platform, this function will forcibly
    disable scissoring if it has not been set previously with
    setScissorRect().

    \sa setScissorRect(), resetScissorRect()
*/
QRect QGLPainter::scissorRect() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(QRect(0, 0, 0, 0));
    if (d->scissorRect.isNull()) {
#if !defined(GL_OES_VERSION_1_0) || defined(GL_OES_VERSION_1_1)
        if (glIsEnabled(GL_SCISSOR_TEST)) {
            GLint scissor[4];
            glGetIntegerv(GL_SCISSOR_BOX, scissor);
            d->scissorRect =
                QRect(scissor[0], scissor[1], scissor[2], scissor[3]);
        } else {
            d->scissorRect = QRect(0, 0, 0, 0);
        }
#else
        // OpenGL/ES 1.0 does not have glIsEnabled() or GL_SCISSOR_BOX,
        // so force the scissor setting to a known state.
        d->scissorRect = QRect(0, 0, 0, 0);
        glDisable(GL_SCISSOR_TEST);
#endif
    }
    return d->scissorRect;
}

/*!
    Enables scissoring to the boundaries of \a rect if it has a
    non-zero size; disables scissoring if \a rect has a zero size.

    \sa scissorRect(), resetScissorRect()
*/
void QGLPainter::setScissorRect(const QRect& rect)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (rect.width() > 0 && rect.height() > 0) {
        d->scissorRect = rect;
        glScissor(rect.x(), rect.y(), rect.width(), rect.height());
        glEnable(GL_SCISSOR_TEST);
    } else {
        d->scissorRect = QRect(0, 0, 0, 0);
        glDisable(GL_SCISSOR_TEST);
    }
}

/*!
    Resets this painter's notion of what the current scissorRect()
    is set to.  The next time scissorRect() is called, the actual
    scissor rectangle will be fetched from the GL server.

    This function is used to synchronize the state of the application
    with the GL server after the execution of raw GL commands that may
    have altered the scissor settings.

    \sa scissorRect(), setScissorRect()
*/
void QGLPainter::resetScissorRect()
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->scissorRect = QRect();
}

/*!
    Returns a reference to the projection matrix stack.

    \sa modelViewMatrix(), combinedMatrix()
*/
QGLMatrixStack& QGLPainter::projectionMatrix()
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(*((QGLMatrixStack *)0));
    return d->projectionMatrix;
}

/*!
    Returns a reference to the modelview matrix stack.

    \sa projectionMatrix(), combinedMatrix()
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

    \sa projectionMatrix(), modelViewMatrix()
*/
// Implemented in qglmatrixstack.cpp.

/*!
    Returns true if \a point is visible within the current viewing volume.
    This is used to perform object culling checks.
*/
bool QGLPainter::isVisible(const QVector3D& point) const
{
    Q_D(const QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(false);
    QVector3D projected = d->modelViewMatrix * point;
    projected = d->projectionMatrix * point;
    return d->viewingCube.contains(projected);
}

/*!
    Returns true if \a box is completely or partially visible within the
    current viewing volume.  This is used to perform object culling checks.
*/
bool QGLPainter::isVisible(const QBox3D& box) const
{
    Q_D(const QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(false);
    QBox3D projected = box.transformed
        (d->projectionMatrix * d->modelViewMatrix);
    return d->viewingCube.intersects(projected);
}

/*!
    Returns the aspect ratio of the underlying device (usually a window),
    for adjusting projection transformations.

    \sa QGLMatrixStack::perspective()
*/
qreal QGLPainter::aspectRatio() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(1.0f);

    // Get the size of the underlying paint device.
    QPaintDevice *device = d->context->device();
    int width = device->width();
    int height = device->height();
    if (width == 0 || height == 0 || width == height)
        return 1.0f;

    // Use the device's DPI setting to determine the pixel aspect ratio.
    int dpiX = device->logicalDpiX();
    int dpiY = device->logicalDpiY();
    if (dpiX <= 0 || dpiY <= 0)
        dpiX = dpiY = 1;

    // Return the final aspect ratio based on window and pixel size.
    return ((qreal)(width * dpiY)) / ((qreal)(height * dpiX));
}

/*!
    Returns the aspect ratio for a viewport of size \a viewportSize,
    accounting for the DPI of the underlying device.

    \sa QGLMatrixStack::perspective()
*/
qreal QGLPainter::aspectRatio(const QSize& viewportSize) const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(1.0f);

    // Get the size of the underlying paint device.
    int width = viewportSize.width();
    int height = viewportSize.height();
    if (width <= 0 || height <= 0 || width == height)
        return 1.0f;

    // Use the device's DPI setting to determine the pixel aspect ratio.
    QPaintDevice *device = d->context->device();
    int dpiX = device->logicalDpiX();
    int dpiY = device->logicalDpiY();
    if (dpiX <= 0 || dpiY <= 0)
        dpiX = dpiY = 1;

    // Return the final aspect ratio based on window and pixel size.
    return ((qreal)(width * dpiY)) / ((qreal)(height * dpiX));
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
    d->ensureEffect();
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
        d->effect->setActive(false);
    d->userEffect = effect;
    if (effect && (!d->pick || !d->pick->isPicking)) {
        d->effect = effect;
        d->effect->setActive(true);
        d->updates = UpdateAll;
    } else {
        // Revert to the effect associated with standardEffect().
        d->effect = 0;
        d->ensureEffect();
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
        d->effect->setActive(false);
    d->standardEffect = effect;
    d->userEffect = 0;
    d->effect = 0;
    d->ensureEffect();
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
        d->effect->setActive(false);
    d->userEffect = 0;
    d->effect = 0;
}

void QGLPainterPrivate::createEffect()
{
    if (userEffect) {
        if (!pick || !pick->isPicking) {
            effect = userEffect;
            effect->setActive(true);
            setRequiredFields(effect->requiredFields());
            updates = QGLPainter::UpdateAll;
            return;
        }
        if (userEffect->supportsPicking()) {
            effect = userEffect;
            effect->setActive(true);
            setRequiredFields(effect->requiredFields());
            updates = QGLPainter::UpdateAll;
            return;
        }
        effect = pick->defaultPickEffect;
        effect->setActive(true);
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
        effect->setActive(true);
    } else {
        effect = pick->defaultPickEffect;
        effect->setActive(true);
    }
    setRequiredFields(effect->requiredFields());
    updates = QGLPainter::UpdateAll;
}

#ifndef QT_NO_DEBUG

void QGLPainterPrivate::removeRequiredFields(const QGLVertexArray& array)
{
    int count = array.m_fields.fieldCount();
    for (int index = 0; index < count; ++index)
        requiredFields.removeAll(array.m_fields.fieldAttribute(index));
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
        case QGL::TextureCoord3:
            qWarning("Attribute QGL::TextureCoord3 is missing"); break;
        case QGL::TextureCoord4:
            qWarning("Attribute QGL::TextureCoord4 is missing"); break;
        case QGL::TextureCoord5:
            qWarning("Attribute QGL::TextureCoord5 is missing"); break;
        case QGL::TextureCoord6:
            qWarning("Attribute QGL::TextureCoord6 is missing"); break;
        case QGL::TextureCoord7:
            qWarning("Attribute QGL::TextureCoord7 is missing"); break;
        case QGL::CustomVertex0:
            qWarning("Attribute QGL::CustomVertex0 is missing"); break;
        case QGL::CustomVertex1:
            qWarning("Attribute QGL::CustomVertex1 is missing"); break;
        case QGL::CustomVertex2:
            qWarning("Attribute QGL::CustomVertex2 is missing"); break;
        case QGL::CustomVertex3:
            qWarning("Attribute QGL::CustomVertex3 is missing"); break;
        case QGL::CustomVertex4:
            qWarning("Attribute QGL::CustomVertex4 is missing"); break;
        case QGL::CustomVertex5:
            qWarning("Attribute QGL::CustomVertex5 is missing"); break;
        case QGL::CustomVertex6:
            qWarning("Attribute QGL::CustomVertex6 is missing"); break;
        case QGL::CustomVertex7:
            qWarning("Attribute QGL::CustomVertex7 is missing"); break;
        default:
            qWarning("Attribute %d is missing", (int)attr); break;
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


#if !defined(QT_OPENGL_ES) && defined (QGL_TEXTURE0) && defined(Q_WS_WIN)

static QGLPainterExtensions *resolveMultiTextureExtensions(QGLPainterPrivate *pd)
{
    QGLPainterExtensions *extn = pd->extensions();
    if (!(extn->multiTextureResolved)) {
        extn->multiTextureResolved = true;
        if (!extn->qt_glActiveTexture) {
            extn->qt_glActiveTexture = (_glActiveTexture)
                pd->context->getProcAddress
                    (QLatin1String("glActiveTexture"));
        }
        if (!extn->qt_glClientActiveTexture) {
            extn->qt_glClientActiveTexture = (_glClientActiveTexture)
                pd->context->getProcAddress
                    (QLatin1String("glClientActiveTexture"));
        }
    }
    return extn;
}

#endif

void QGLAbstractEffect::setVertexArray(const QGLVertexArray& array)
{
#if !defined(QT_OPENGL_ES_2)
    for (int field = 0; field < array.m_fields.fieldCount(); ++field) {
        QGLAttributeValue value = array.attributeValue(field);
        switch (array.m_fields.fieldAttribute(field)) {
        case QGL::Position:
            glVertexPointer(value.size(), GL_FLOAT,
                            value.stride(), value.data());
            break;

        case QGL::Normal:
            if (value.size() == 3)
                glNormalPointer(GL_FLOAT, value.stride(), value.data());
            break;

        case QGL::Color:
            glColorPointer(value.size(), GL_FLOAT,
                           value.stride(), value.data());
            break;

#ifdef GL_TEXTURE_COORD_ARRAY
        case QGL::TextureCoord0:
        case QGL::TextureCoord1:
        case QGL::TextureCoord2:
        case QGL::TextureCoord3:
        case QGL::TextureCoord4:
        case QGL::TextureCoord5:
        case QGL::TextureCoord6:
        case QGL::TextureCoord7:
        {
            int unit = (int)(array.m_fields.fieldAttribute(field) - QGL::TextureCoord0);

#if defined (QGL_TEXTURE0) && defined(Q_WS_WIN)			
            // The GL implementation does not support multitexturing natively - we should attempt to resolve it or use the supported base-functions
            QGLPainter painter(QGLContext::currentContext());
			if (!painter.d_ptr->extensions()->multiTextureResolved) painter.d_ptr->resolveMultiTextureExtensions();

			if (painter.d_ptr->extensions()->qt_glClientActiveTexture)
			{
				painter.d_ptr->extensions()->qt_glClientActiveTexture(QGL_TEXTURE0+unit);			
				glTexCoordPointer(value.size(), GL_FLOAT,
								  value.stride(), value.data());
				if (unit != 0)  // Stay on unit 0 between requests.
					painter.d_ptr->extensions()->qt_glClientActiveTexture(QGL_TEXTURE0);			
			}
			else if (unit!=0)
			{
				glTexCoordPointer(value.size(), GL_FLOAT,
                                  value.stride(), value.data());
			}
#elif defined(QGL_TEXTURE0)
			glClientActiveTexture(QGL_TEXTURE0 + unit);
            glTexCoordPointer(value.size(), GL_FLOAT,
                              value.stride(), value.data());
            if (unit != 0)  // Stay on unit 0 between requests.
                glClientActiveTexture(QGL_TEXTURE0);
#elif defined(GL_TEXTURE0_ARB) && defined (Q_WS_WIN)
           QGLPainter painter(QGLContext::currentContext());
			if (!painter.d_ptr->extensions()->multiTextureARBResolved) painter.d_ptr->resolveMultiTextureExtensionsARB();

			if (painter.d_ptr->extensions()->qt_glClientActiveTextureARB)
			{
				painter.d_ptr->extensions()->qt_glClientActiveTextureARB(GL_TEXTURE0_ARB+unit);			
				glTexCoordPointer(value.size(), GL_FLOAT,
								  value.stride(), value.data());
				if (unit != 0)  // Stay on unit 0 between requests.
					painter.d_ptr->extensions()->qt_glClientActiveTextureARB(GL_TEXTURE0_ARB);			
			}
			else if (unit!=0)
			{
				glTexCoordPointer(value.size(), GL_FLOAT,
                                  value.stride(), value.data());
			}
#elif defined(GL_TEXTURE0_ARB)
            glClientActiveTextureARB(GL_TEXTURE0_ARB + unit);
            glTexCoordPointer(value.size(), GL_FLOAT,
                              value.stride(), value.data());
            if (unit != 0)
                glClientActiveTextureARB(GL_TEXTURE0_ARB);
#else
            if (unit == 0)
                glTexCoordPointer(value.size(), GL_FLOAT,
                                  value.stride(), value.data());
#endif
        }
        break;
#endif

        default: break;
        }
    }
#else
    Q_UNUSED(array);
#endif
}

#if !defined(QT_OPENGL_ES_2)

void QGLAbstractEffect::enableVertexAttribute(QGL::VertexAttribute attribute)
{
    switch (attribute) {
        case QGL::Position:
            glEnableClientState(GL_VERTEX_ARRAY);
            break;

        case QGL::Normal:
            glEnableClientState(GL_NORMAL_ARRAY);
            break;

        case QGL::Color:
            glEnableClientState(GL_COLOR_ARRAY);
            break;

#ifdef GL_TEXTURE_COORD_ARRAY
        case QGL::TextureCoord0:
        case QGL::TextureCoord1:
        case QGL::TextureCoord2:
        case QGL::TextureCoord3:
        case QGL::TextureCoord4:
        case QGL::TextureCoord5:
        case QGL::TextureCoord6:
        case QGL::TextureCoord7:
        {
            int unit = (int)(attribute - QGL::TextureCoord0);
#if defined (QGL_TEXTURE0) && defined(Q_WS_WIN)
            // The GL implementation does not support multitexturing natively - we should attempt to resolve it or use the supported base-functions
            QGLPainter painter(QGLContext::currentContext());
			if (!painter.d_ptr->extensions()->multiTextureResolved) painter.d_ptr->resolveMultiTextureExtensions();

			if (painter.d_ptr->extensions()->qt_glClientActiveTexture)
			{
				painter.d_ptr->extensions()->qt_glClientActiveTexture(QGL_TEXTURE0+unit);			
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				if (unit != 0)  // Stay on unit 0 between requests.
					painter.d_ptr->extensions()->qt_glClientActiveTexture(QGL_TEXTURE0);			
			}
            // The GL implementation does not support multitexturing.
            else if (unit == 0)
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#elif defined(QGL_TEXTURE0)
            glClientActiveTexture(QGL_TEXTURE0 + unit);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            if (unit != 0)  // Stay on unit 0 between requests.
                glClientActiveTexture(QGL_TEXTURE0);
#elif defined(GL_TEXTURE0_ARB) && defined (Q_WS_WIN)
           QGLPainter painter(QGLContext::currentContext());
			if (!painter.d_ptr->extensions()->multiTextureARBResolved) painter.d_ptr->resolveMultiTextureExtensionsARB();

			if (painter.d_ptr->extensions()->qt_glClientActiveTextureARB)
			{
				painter.d_ptr->extensions()->qt_glClientActiveTextureARB(GL_TEXTURE0_ARB+unit);			
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				if (unit != 0)  // Stay on unit 0 between requests.
					painter.d_ptr->extensions()->qt_glClientActiveTextureARB(GL_TEXTURE0_ARB);			
			}
			// The GL implementation does not support multitexturing.
            else if (unit == 0)
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#elif defined(GL_TEXTURE0_ARB)
            glClientActiveTextureARB(GL_TEXTURE0_ARB + unit);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            if (unit != 0)
                glClientActiveTextureARB(GL_TEXTURE0_ARB);
#else
            // The GL implementation does not support multitexturing.
            if (unit == 0)
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
        }
        break;
#endif

        default: break;
    }
}

void QGLAbstractEffect::disableVertexAttribute(QGL::VertexAttribute attribute)
{
    switch (attribute) {
        case QGL::Position:
            glDisableClientState(GL_VERTEX_ARRAY);
            break;

        case QGL::Normal:
            glDisableClientState(GL_NORMAL_ARRAY);
            break;

        case QGL::Color:
            glDisableClientState(GL_COLOR_ARRAY);
            break;

#ifdef GL_TEXTURE_COORD_ARRAY
        case QGL::TextureCoord0:
        case QGL::TextureCoord1:
        case QGL::TextureCoord2:
        case QGL::TextureCoord3:
        case QGL::TextureCoord4:
        case QGL::TextureCoord5:
        case QGL::TextureCoord6:
        case QGL::TextureCoord7:
        {
            int unit = (int)(attribute - QGL::TextureCoord0);
#if defined (QGL_TEXTURE0) && defined(Q_WS_WIN)
            // The GL implementation does not support multitexturing natively - we should attempt to resolve it or use the supported base-functions
            QGLPainter painter(QGLContext::currentContext());
			if (!painter.d_ptr->extensions()->multiTextureResolved) painter.d_ptr->resolveMultiTextureExtensions();

			if (painter.d_ptr->extensions()->qt_glClientActiveTexture)
			{
				painter.d_ptr->extensions()->qt_glClientActiveTexture(QGL_TEXTURE0+unit);			
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				if (unit != 0)  // Stay on unit 0 between requests.
					painter.d_ptr->extensions()->qt_glClientActiveTexture(QGL_TEXTURE0);			
			}
            // The GL implementation does not support multitexturing.
            else if (unit == 0)
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#elif defined(QGL_TEXTURE0)
            glClientActiveTexture(QGL_TEXTURE0 + unit);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            if (unit != 0)  // Stay on unit 0 between requests.
                glClientActiveTexture(QGL_TEXTURE0);
#elif defined(GL_TEXTURE0_ARB) && defined (Q_WS_WIN)
           QGLPainter painter(QGLContext::currentContext());
			if (!painter.d_ptr->extensions()->multiTextureARBResolved) painter.d_ptr->resolveMultiTextureExtensionsARB();

			if (painter.d_ptr->extensions()->qt_glClientActiveTextureARB)
			{
				painter.d_ptr->extensions()->qt_glClientActiveTextureARB(GL_TEXTURE0_ARB+unit);			
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				if (unit != 0)  // Stay on unit 0 between requests.
					painter.d_ptr->extensions()->qt_glClientActiveTextureARB(GL_TEXTURE0_ARB);			
			}
			// The GL implementation does not support multitexturing.
            else if (unit == 0)
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#elif defined(GL_TEXTURE0_ARB)
            glClientActiveTextureARB(GL_TEXTURE0_ARB + unit);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            if (unit != 0)
                glClientActiveTextureARB(GL_TEXTURE0_ARB);
#else
            if (unit == 0)
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
        }
        break;
#endif

        default: break;
    }
}

bool QGLAbstractEffect::setVertexAttribute
        (QGL::VertexAttribute attribute, const QGLVertexArray& array)
{
    QGLAttributeValue value = array.attributeValue(attribute);
    if (!value.isNull()) {
        switch (attribute) {
        case QGL::Position:
            glVertexPointer(value.size(), GL_FLOAT,
                            value.stride(), value.data());
            break;

        case QGL::Normal:
            if (value.size() == 3)
                glNormalPointer(GL_FLOAT, value.stride(), value.data());
            break;

        case QGL::Color:
            glColorPointer(value.size(), GL_FLOAT,
                           value.stride(), value.data());
            break;

#ifdef GL_TEXTURE_COORD_ARRAY
        case QGL::TextureCoord0:
        case QGL::TextureCoord1:
        case QGL::TextureCoord2:
        case QGL::TextureCoord3:
        case QGL::TextureCoord4:
        case QGL::TextureCoord5:
        case QGL::TextureCoord6:
        case QGL::TextureCoord7:
        {
            int unit = (int)(attribute - QGL::TextureCoord0);


#if defined (QGL_TEXTURE0) && defined(Q_WS_WIN)			
            // The GL implementation does not support multitexturing natively - we should attempt to resolve it or use the supported base-functions
            QGLPainter painter(QGLContext::currentContext());
			if (!painter.d_ptr->extensions()->multiTextureResolved) painter.d_ptr->resolveMultiTextureExtensions();

			if (painter.d_ptr->extensions()->qt_glClientActiveTexture)
			{
				painter.d_ptr->extensions()->qt_glClientActiveTexture(QGL_TEXTURE0+unit);			
				glTexCoordPointer(value.size(), GL_FLOAT,
								  value.stride(), value.data());
				if (unit != 0)  // Stay on unit 0 between requests.
					painter.d_ptr->extensions()->qt_glClientActiveTexture(QGL_TEXTURE0);			
			}
			else if (unit==0)
			{
				glTexCoordPointer(value.size(), GL_FLOAT,
                                  value.stride(), value.data());
			}
#elif defined(QGL_TEXTURE0)
            glClientActiveTexture(QGL_TEXTURE0 + unit);
            glTexCoordPointer(value.size(), GL_FLOAT,
                              value.stride(), value.data());
            if (unit != 0)  // Stay on unit 0 between requests.
                glClientActiveTexture(QGL_TEXTURE0);
#elif defined(GL_TEXTURE0_ARB) && defined (Q_WS_WIN)
           QGLPainter painter(QGLContext::currentContext());
			if (!painter.d_ptr->extensions()->multiTextureARBResolved) painter.d_ptr->resolveMultiTextureExtensionsARB();

			if (painter.d_ptr->extensions()->qt_glClientActiveTextureARB)
			{
				painter.d_ptr->extensions()->qt_glClientActiveTextureARB(GL_TEXTURE0_ARB+unit);			
				glTexCoordPointer(value.size(), GL_FLOAT,
								  value.stride(), value.data());
				if (unit != 0)  // Stay on unit 0 between requests.
					painter.d_ptr->extensions()->qt_glClientActiveTextureARB(GL_TEXTURE0_ARB);			
			}
			else if (unit==0)
			{
				glTexCoordPointer(value.size(), GL_FLOAT,
                                  value.stride(), value.data());
			}
#elif defined(GL_TEXTURE0_ARB)
            glClientActiveTextureARB(GL_TEXTURE0_ARB + unit);
            glTexCoordPointer(value.size(), GL_FLOAT,
                              value.stride(), value.data());
            if (unit != 0)
                glClientActiveTextureARB(GL_TEXTURE0_ARB);
#else

            if (unit == 0)
                glTexCoordPointer(value.size(), GL_FLOAT,
                                  value.stride(), value.data());
#endif
		}
        break;
#endif

        default: break;
        }
        return true;
    } else {
        return false;
    }
}

#endif // !QT_OPENGL_ES_2

/*!
    Sets vertex attributes on the current GL context based on the
    fields in \a array.

    The following example draws a single triangle, where each
    vertex consists of a 3D position and a 2D texture co-ordinate:

    \code
    QGLVertexArray array(QGL::Position, 3, QGL::TextureCoord0, 2);
    array.append(60.0f,  10.0f,  0.0f);
    array.append(0.0f, 0.0f);
    array.append(110.0f, 110.0f, 0.0f);
    array.append(1.0f, 0.0f);
    array.append(10.0f,  110.0f, 0.0f);
    array.append(1.0f, 1.0f);
    painter.setVertexArray(array);
    painter.draw(QGL::Triangles, 3);
    \endcode

    If \a array has been uploaded into the GL server as a vertex
    buffer, then the vertex buffer will be used to set the attributes.

    \sa draw(), setCommonNormal(), QGLVertexArray::upload()
*/
void QGLPainter::setVertexArray(const QGLVertexArray& array)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->ensureEffect();
    if (array.isUploaded() && array.bind()) {
        QGLVertexArray bufferArray = array.toBufferForm();
        d->effect->setVertexArray(bufferArray);
        d->removeRequiredFields(bufferArray);
        array.release();
    } else {
        d->effect->setVertexArray(array);
        d->removeRequiredFields(array);
    }
}

/*!
    Sets the common normal value on the current effect() for all
    vertices to \a value and disable any active normal arrays.

    \sa setVertexArray()
*/
void QGLPainter::setCommonNormal(const QVector3D& value)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->ensureEffect();
    d->effect->setCommonNormal(value);
#ifndef QT_NO_DEBUG
    d->requiredFields.removeAll(QGL::Normal);
#endif
}

/*!
    Returns the number of texture units associated with the
    current GL context.

    \sa setTexture(), isTextureUnitActive()
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
    Returns true if the specified texture \a unit is active;
    false otherwise.  To activate a texture unit, call setTexture()
    with a non-null texture object.  To deactivate a texture unit,
    call setTexture() with a null texture object.

    \sa setTexture(), textureUnitCount()
*/
bool QGLPainter::isTextureUnitActive(int unit) const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(false);
    if (unit < 0 || unit >= d->texturesInUse.size())
        return false;
    else
        return d->texturesInUse[unit];
}

/*!
    Sets a texture \a unit to the contents of \a texture.
    If \a texture is null, the texture unit will be disabled.

    \sa textureUnitCount(), isTextureUnitActive()
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

    // Record whether this texture unit is in use or not.
    if (unit >= d->texturesInUse.size())
        d->texturesInUse.resize(unit + 1);
    d->texturesInUse[unit] = (texture != 0);

    // Select the texture unit and bind the texture.
#undef glActiveTexture      // Remove definition in qglextensions_p.h.
//#if !defined(QT_OPENGL_ES)
//    const QGLContext *ctx = d->context;
//#else
//#undef glActiveTexture      // Remove definition in qglextensions_p.h.
//#endif

#if defined (QGL_TEXTURE0) && defined (Q_WS_WIN)
    d->resolveMultiTextureExtensions();
    if (d->extensionFuncs->qt_glActiveTexture) 
		d->extensionFuncs->qt_glActiveTexture(QGL_TEXTURE0+unit);
#elif defined(QGL_TEXTURE0)
    glActiveTexture(QGL_TEXTURE0 + unit);
#elif defined (GL_TEXTURE0_ARB) && defined (Q_WS_WIN)
    d->resolveMultiTextureExtensionsARB();
    if (d->extensionFuncs->qt_glActiveTextureARB)
		d->extensionFuncs->qt_glActiveTextureARB(GL_TEXTURE0_ARB+unit);
#elif defined(GL_TEXTURE0_ARB)
  glActiveTextureARB(GL_TEXTURE0_ARB + unit);
#endif
    if (!texture) {
        glBindTexture(GL_TEXTURE_2D, 0);
#if !defined(QGL_SHADERS_ONLY)
        glDisable(GL_TEXTURE_2D);
#endif
    } else {
        texture->bind();
#if !defined(QGL_SHADERS_ONLY)
        glEnable(GL_TEXTURE_2D);
#endif
    }

    // Leave the default setting on texture unit 0 just in case
    // raw GL code is being mixed in with QGLPainter code.
#if defined (QGL_TEXTURE0) && defined (Q_WS_WIN)
    d->resolveMultiTextureExtensions();
    if (d->extensionFuncs->qt_glActiveTexture && unit!=0) 
		d->extensionFuncs->qt_glActiveTexture(QGL_TEXTURE0+unit);
#elif defined(QGL_TEXTURE0)
    if (unit != 0)
        glActiveTexture(QGL_TEXTURE0);
#elif defined (GL_TEXTURE0_ARB) && defined (Q_WS_WIN)
	d->resolveMultiTextureExtensionsARB();
    if (d->extensionFuncs->qt_glActiveTextureARB && unit!=0) 
		d->extensionFuncs->qt_glActiveTextureARB(GL_TEXTURE0_ARB+unit);
#elif defined(GL_TEXTURE0_ARB)
    if (unit != 0)
        glActiveTextureARB(GL_TEXTURE0_ARB);
#endif
}

/*!
    Sets a texture \a unit to the contents of \a texture.
    If \a texture is null, the texture unit will be disabled.

    \sa textureUnitCount(), isTextureUnitActive()
*/
void QGLPainter::setTexture(int unit, const QGLTextureCube *texture)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (unit < 0)
        return;

    // Record whether this texture unit is in use or not.
    if (unit >= d->texturesInUse.size())
        d->texturesInUse.resize(unit + 1);
    d->texturesInUse[unit] = (texture != 0);

    // Select the texture unit and bind the texture.
#undef glActiveTexture      // Remove definition in qglextensions_p.h.

//#if !defined(QT_OPENGL_ES)
//    const QGLContext *ctx = d->context;
//#else
//#undef glActiveTexture      // Remove definition in qglextensions_p.h.
//#endif

#if defined (QGL_TEXTURE0) && defined (Q_WS_WIN)
    d->resolveMultiTextureExtensions();
    if (d->extensionFuncs->qt_glActiveTexture) 
		d->extensionFuncs->qt_glActiveTexture(QGL_TEXTURE0+unit);
#elif defined(QGL_TEXTURE0)
    glActiveTexture(QGL_TEXTURE0 + unit);
#elif defined (GL_TEXTURE0_ARB) && defined (Q_WS_WIN)
	d->resolveMultiTextureExtensionsARB();
    if (d->extensionFuncs->qt_glActiveTextureARB) 
		d->extensionFuncs->qt_glActiveTextureARB(GL_TEXTURE0_ARB+unit);
#elif defined(GL_TEXTURE0_ARB)
    glActiveTextureARB(GL_TEXTURE0_ARB + unit);
#endif
    if (!texture) {
        QGLTextureCube::release();
#if !defined(QGL_SHADERS_ONLY)
        glDisable(GL_TEXTURE_CUBE_MAP);
#endif
    } else {
        texture->bind();
#if !defined(QGL_SHADERS_ONLY)
        glEnable(GL_TEXTURE_CUBE_MAP);
#endif
    }

    // Leave the default setting on texture unit 0 just in case
    // raw GL code is being mixed in with QGLPainter code.
#if defined (QGL_TEXTURE0) && defined (Q_WS_WIN)
    d->resolveMultiTextureExtensions();
    if (d->extensionFuncs->qt_glActiveTexture && unit!=0) 
		d->extensionFuncs->qt_glActiveTexture(QGL_TEXTURE0+unit);
#elif defined(QGL_TEXTURE0)
    if (unit != 0)
        glActiveTexture(QGL_TEXTURE0);
#elif defined (GL_TEXTURE0_ARB) && defined (Q_WS_WIN)
	d->resolveMultiTextureExtensionsARB();
    if (d->extensionFuncs->qt_glActiveTextureARB && unit!=0) 
		d->extensionFuncs->qt_glActiveTextureARB(GL_TEXTURE0_ARB+unit);
#elif defined(GL_TEXTURE0_ARB)
    if (unit != 0)
        glActiveTextureARB(GL_TEXTURE0_ARB);
#endif
}

/*!
    \fn void QGLPainter::setTexture(const QGLTexture2D *texture)

    Sets texture unit 0 to the contents of \a texture.
    If \a texture is null, the texture unit will be disabled.

    \sa textureUnitCount(), isTextureUnitActive()
*/

/*!
    \fn void QGLPainter::setTexture(const QGLTextureCube *texture)

    Sets texture unit 0 to the contents of \a texture.
    If \a texture is null, the texture unit will be disabled.

    \sa textureUnitCount(), isTextureUnitActive()
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

    \sa setUserEffect(), projectionMatrix(), modelViewMatrix()
    \sa draw()
*/
void QGLPainter::update()
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    d->ensureEffect();
    QGLPainter::Updates updates = d->updates;
    d->updates = 0;
    if (d->modelViewMatrix.updateServer())
        updates |= UpdateModelViewMatrix;
    if (d->projectionMatrix.updateServer())
        updates |= UpdateProjectionMatrix;
    d->effect->update(this, updates);
}

/*!
    Draws primitives using \a count vertices from the arrays specified
    by setVertexArray().  The type of primitive to draw is specified
    by \a mode.

    This operation will consume \a count values from the
    enabled arrays, starting at \a index.

    \sa update()
*/
void QGLPainter::draw(QGL::DrawingMode mode, int count, int index)
{
#ifndef QT_NO_DEBUG
    if (mode == QGL::NoDrawingMode)
        qWarning("Calling QGLPainter::draw with no drawing mode set");
#endif
    update();
    checkRequiredFields();
    glDrawArrays((GLenum)mode, index, count);
}

/*!
    \overload

    Draws primitives using vertices from the arrays specified by
    setVertexArray().  The type of primitive to draw is specified by \a mode.

    This operation will consume all of the elements of \a indices,
    which are used to index into the enabled arrays.

    If \a indices has been uploaded to the GL server as an index
    buffer, then this function will draw using that index buffer.

    \sa update(), QGLIndexArray::upload()
*/
void QGLPainter::draw(QGL::DrawingMode mode, const QGLIndexArray& indices)
{
#ifndef QT_NO_DEBUG
    if (mode == QGL::NoDrawingMode)
        qWarning("Calling QGLPainter::draw with no drawing mode set");
#endif
    update();
    checkRequiredFields();
    if (indices.isUploaded() && indices.bind()) {
        glDrawElements((GLenum)mode, indices.size(), indices.type(),
                       reinterpret_cast<const char *>(0));
        indices.release();
    } else {
        glDrawElements((GLenum)mode, indices.size(),
                       indices.type(), indices.constData());
    }
}

/*!
    \overload

    Draws primitives using vertices from the arrays specified by
    setVertexArray().  The type of primitive to draw is specified by \a mode.

    This operation will consume \a count elements of \a indices,
    starting at \a offset, which are used to index into the enabled arrays.

    If \a indices has been uploaded to the GL server as an index
    buffer, then this function will draw using that index buffer.

    \sa update(), QGLIndexArray::upload()
*/
void QGLPainter::draw(QGL::DrawingMode mode, const QGLIndexArray& indices,
                      int offset, int count)
{
    Q_ASSERT(offset >= 0 && count >= 0 && (offset + count) <= indices.size());
#ifndef QT_NO_DEBUG
    if (mode == QGL::NoDrawingMode)
        qWarning("Calling QGLPainter::draw with no drawing mode set");
#endif
    update();
    checkRequiredFields();
    if (indices.isUploaded() && indices.bind()) {
        if (indices.type() == GL_UNSIGNED_SHORT) {
            glDrawElements((GLenum)mode, count, indices.type(),
                reinterpret_cast<const char *>(offset * sizeof(ushort)));
        } else {
            glDrawElements((GLenum)mode, count, indices.type(),
                reinterpret_cast<const char *>(offset * sizeof(int)));
        }
        indices.release();
    } else {
        if (indices.type() == GL_UNSIGNED_SHORT) {
            glDrawElements((GLenum)mode, count, indices.type(),
                reinterpret_cast<const ushort *>(indices.constData()) + offset);
        } else {
            glDrawElements((GLenum)mode, count, indices.type(),
                reinterpret_cast<const int *>(indices.constData()) + offset);
        }
    }
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
    Returns the total number of lights that are supported by the OpenGL
    implementation, whether they are enabled or not.  The returned value
    will be zero before begin() is called.

    \sa isLightEnabled()
*/
int QGLPainter::lightCount() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(0);
    if (!d->maxLights) {
#if defined(GL_LIGHTING) && defined(GL_MAX_LIGHTS)
        d->maxLights = 8;
        glGetIntegerv(GL_MAX_LIGHTS, &d->maxLights);
        if (d->maxLights >= QGL_MAX_LIGHTS)
            d->maxLights = QGL_MAX_LIGHTS;
#else
        d->maxLights = 8;
#endif
    }
    return d->maxLights;
}

/*!
    Returns true if there are enabled lights in this painter.

    \sa isLightEnabled()
*/
bool QGLPainter::hasEnabledLights() const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(false);
    return (d->enabledLights != 0);
}

/*!
    Returns true if light \a number has been enabled.

    \sa setLightEnabled(), lightCount(), lightParameters(), hasEnabledLights()
*/
bool QGLPainter::isLightEnabled(int number) const
{
    Q_D(const QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(false);
    if (!d->maxLights)
        lightCount();
    if (number >= 0 && number < d->maxLights)
        return (d->enabledLights & (1 << number)) != 0;
    else
        return false;
}

/*!
    Sets the enable state for light \a number to \a value.  The function
    call will be ignored if \a number is out of range.

    The light settings in the GL server will not be changed until
    update() is called.  At that time, the \c{GL_LIGHTn} property
    will be enabled, but not \c{GL_LIGHTING}.  The effect() is
    responsible for enabling \c{GL_LIGHTING} if it supports lights.

    \sa isLightEnabled(), lightCount(), setLightParameters()
*/
void QGLPainter::setLightEnabled(int number, bool value) const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (!d->maxLights)
        lightCount();
    if (number >= 0 && number < d->maxLights) {
        if (value)
            d->enabledLights |= (1 << number);
        else
            d->enabledLights &= ~(1 << number);
        d->updates |= QGLPainter::UpdateLights;
    }
}

/*!
    Returns the parameters for light \a number.  If \a number is out
    of range, a default QGLLightParameters object will be returned.

    The light parameters are specified in world co-ordinates at
    the point when setLightParameters() was called for light \a number.
    The lightTransform() for light \a number must be applied to
    obtain eye co-ordinates.

    Note: the default diffuse and specular color for light 0 is
    white, and the default diffuse and specular color for other
    lights is black.  All other defaults are as defined in the
    QGLLightParameters class.

    \sa lightTransform(), setLightParameters(), isLightEnabled()
*/
const QGLLightParameters *QGLPainter::lightParameters(int number) const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(QGLLightParameters());
    if (!d->maxLights)
        lightCount();
    if (number >= 0 && number < d->maxLights) {
        if (d->lights[number])
            return d->lights[number];
    }
    if (number == 0) {
        if (!d->defaultLight0)
            d->defaultLight0 = new QGLLightParameters();
        return d->defaultLight0;
    } else {
        if (!d->defaultLight1) {
            d->defaultLight1 = new QGLLightParameters();
            d->defaultLight1->setDiffuseColor(QColor(0, 0, 0, 255));
            d->defaultLight1->setSpecularColor(QColor(0, 0, 0, 255));
        }
        return d->defaultLight1;
    }
}

/*!
    Returns the modelview transformation matrix for light \a number that
    was set at the time setLightParameters() was called.  If \a number is out
    of range, the identity matrix will be returned.

    The light transform may be used by later painting operations to
    convert the light from world co-ordinates into eye co-ordinates.
    The eye transformation is set when the light is specified.

    \sa lightParameters(), setLightParameters(), isLightEnabled()
*/
QMatrix4x4 QGLPainter::lightTransform(int number) const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(QGLLightParameters());
    if (!d->maxLights)
        lightCount();
    if (number >= 0 && number < d->maxLights)
        return d->lightTransforms[number];
    else
        return QMatrix4x4();
}

/*!
    Sets the \a parameters for light \a number.  If \a number is out
    of range, the function call will be ignored.  The lightTransform()
    for light \a number is set to the current modelViewMatrix().

    Light parameters are stored in world co-ordinates, not eye co-ordinates.
    The lightTransform() for the light specifies the transformation to
    apply to convert the world co-ordinates into eye co-ordinates when
    the light is used.

    Note: the \a parameters may be ignored by effect() if it
    has some other way to determine the lighting conditions.

    The light settings in the GL server will not be changed until
    update() is called.

    \sa lightParameters(), lightTransform(), setLightEnabled()
*/
void QGLPainter::setLightParameters(int number, const QGLLightParameters *parameters)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (!d->maxLights)
        lightCount();
    if (number >= 0 && number < d->maxLights) {
        d->lights[number] = parameters;
        d->lightTransforms[number] = modelViewMatrix();
        d->updates |= QGLPainter::UpdateLights;
    }
}

/*!
    Sets the \a parameters for light \a number.  If \a number is out
    of range, the function call will be ignored.  The lightTransform()
    for light \a number is set to \a transform.

    Light parameters are stored in world co-ordinates, not eye co-ordinates.
    The lightTransform() for the light specifies the transformation to
    apply to convert the world co-ordinates into eye co-ordinates when
    the light is used.

    Note: the \a parameters may be ignored by effect() if it
    has some other way to determine the lighting conditions.

    The light settings in the GL server will not be changed until
    update() is called.

    \sa lightParameters(), lightTransform(), setLightEnabled()
*/
void QGLPainter::setLightParameters
    (int number, const QGLLightParameters *parameters,
     const QMatrix4x4& transform)
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE();
    if (!d->maxLights)
        lightCount();
    if (number >= 0 && number < d->maxLights) {
        d->lights[number] = parameters;
        d->lightTransforms[number] = transform;
        d->updates |= QGLPainter::UpdateLights;
    }
}

/*!
    Returns the material that is used for drawing \a face on polygons.
    If \a face is QGL::FrontFaces or QGL::AllFaces, then the front
    material is returned.  If \a face is QGL::BackFaces, then the
    back material is returned.

    \sa setFaceMaterial(), setFaceColor()
*/
const QGLMaterialParameters *QGLPainter::faceMaterial(QGL::Face face) const
{
    Q_D(QGLPainter);
    QGLPAINTER_CHECK_PRIVATE_RETURN(QGLMaterialParameters());
    if (face == QGL::BackFaces) {
        if (!d->backMaterial) {
            if (!d->defaultMaterial)
                d->defaultMaterial = new QGLMaterialParameters();
            d->backMaterial = d->defaultMaterial;
        }
        return d->backMaterial;
    } else {
        if (!d->frontMaterial) {
            if (!d->defaultMaterial)
                d->defaultMaterial = new QGLMaterialParameters();
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
        (QGL::Face face, const QGLMaterialParameters *value)
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

static QGLMaterialParameters *createColorMaterial
    (QGLMaterialParameters *prev, const QColor& color)
{
    QGLMaterialParameters *material;
    if (prev)
        material = prev;
    else
        material = new QGLMaterialParameters();
    material->setAmbientColor
        (QColor::fromRgbF(color.redF() * 0.2, color.greenF() * 0.2,
                          color.blueF() * 0.2, color.alphaF()));
    material->setDiffuseColor
        (QColor::fromRgbF(color.redF() * 0.8, color.greenF() * 0.8,
                          color.blueF() * 0.8, color.alphaF()));
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
            d->effect->setActive(false);
        d->effect = 0;
        d->ensureEffect();
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
