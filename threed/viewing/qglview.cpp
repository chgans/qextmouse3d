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

#include "qglview.h"
#include "qglframebufferobject.h"
#include "qgldepthbufferoptions.h"
#include "qglblendoptions.h"
#include <QtGui/qevent.h>
#include <QtCore/qmap.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qtimer.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLView
    \brief The QGLView class extends QGLWidget with support for 3D viewing.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::viewing

    \section1 Navigating

    Navigation in 3D space is possible under keyboard and mouse control.

    Holding down the left mouse button and dragging it will rotate the
    camera() position around the object being viewed.  Holding down the
    left mouse button and the Shift key pans the view in a plane without
    rotating the viewed object.

    Using the mouse wheel, the view can be zoomed in or out.  If the
    system does not have a mouse wheel, then holding down the left mouse
    button and the Control key and moving the mouse up and down will
    also zoom in and out.

    On the keyboard, the left, right, up, and down keys can also be used
    to shift the camera() position around the object being viewed.  Shift
    and Control modify keys the same way they modify the left mouse
    button above.  Hitting the Home key will cause the camera position
    to be reset to its original position.

    \section1 Stereo viewing support

    If the hardware supports stereo buffers, then each time the scene needs
    to be painted, QGLView renders it twice: first from the perspective of
    the left eye, and then from the perspective of the right eye.
    The separation between the eye positions is specified by
    QGLCamera::eyeSeparation().  If the eye separation is zero,
    then stereo viewing is disabled and only a single image will
    be rendered per frame.

    Two kinds of stereo viewing are possible: hardware stereo
    and simulated stereo.

    Hardware stereo relies upon specialized hardware that can render
    the left and right eye images into separate buffers and then show
    them independently to each eye through the use of polarized glasses
    or similar technology.  Hardware stereo is the default if the
    hardware supports it.

    Simulated stereo is used when the hardware doesn't have specialized
    stereo buffer tracking support.  The left image is masked by a red
    filter and the right image is masked by a cyan filter.  This makes
    the resulting images suitable for viewing with standard red-cyan
    anaglyph glasses.

    When using red-cyan anaglyphs, it is recommended that the
    scene use non-primary object colors.  Pure primary colors such
    as red, green, and blue will only appear to one of the viewer's
    eyes and will inhibit the 3D effect.  Non-primary colors or
    grayscale should be used to get the best effects.

    Red-cyan anaglyphs can be disorienting to some viewers.  If hardware
    stereo is not available and stereo viewing is not critical to
    the application, then stereo can be disabled by setting
    QGLCamera::eyeSeparation() to zero.

    Ctrl-Left and Ctrl-Right can be used to make the eye separation
    smaller or larger under keyboard control.

    \sa {Stereo Viewing Example}
*/

/*!
    \enum QGLView::Option
    This enum defines an option for QGLView.

    \value ObjectPicking Object picking is enabled.  Disabled by default.
    \value ShowPicking Objects are rendered with their pick colors instead
           of their normal colors and materials.  This can help debug
           problems with object picking.  Disabled by default.
    \value CameraNavigation Camera navigation using the keyboard and mouse
           is enabled.  Enabled by default.
    \omitvalue PaintingLog
*/

/*!
    \enum QGLView::StereoType
    This enum defines the type of stereo viewing technology being used by QGLView.

    \value Hardware Specialized stereo hardware is being used.
    \value RedCyanAnaglyph Stereo is being simulated for viewing by
        red-cyan anaglyph classes.
*/

class QGLViewPrivate
{
public:
    QGLViewPrivate(QGLView *parent)
    {
        options = QGLView::CameraNavigation;
        fbo = 0;

        if (parent->format().stereo())
            stereoType = QGLView::Hardware;
        else
            stereoType = QGLView::RedCyanAnaglyph;

        pickBufferForceUpdate = true;
        pickBufferMaybeInvalid = true;
        updateQueued = false;

        pressedObject = 0;
        pressedButton = Qt::NoButton;
        enteredObject = 0;

        defaultCamera = new QGLCamera(parent);
        camera = defaultCamera;

        panning = false;
        startPan = QPoint(-1, -1);

        depthBufferOptions.setEnabled(true);
        depthBufferOptions.setFunction(QGLDepthBufferOptions::Less);

        blendOptions.setEnabled(true);
        blendOptions.setSourceColorFactor(QGLBlendOptions::SrcAlpha);
        blendOptions.setSourceAlphaFactor(QGLBlendOptions::SrcAlpha);
        blendOptions.setDestinationColorFactor(QGLBlendOptions::OneMinusSrcAlpha);
        blendOptions.setDestinationAlphaFactor(QGLBlendOptions::OneMinusSrcAlpha);

        QObject::connect(defaultCamera, SIGNAL(projectionChanged()),
                         parent, SLOT(cameraChanged()));
        QObject::connect(defaultCamera, SIGNAL(viewChanged()),
                         parent, SLOT(cameraChanged()));

        logTime.start();
        lastFrameTime.start();
        QByteArray env = qgetenv("QT3D_LOG_EVENTS");
        if (env == "1")
            options |= QGLView::PaintingLog;
    }
    ~QGLViewPrivate()
    {
        delete fbo;
    }

    QGLView::Options options;
    QGLView::StereoType stereoType;
    QGLFramebufferObject *fbo;
    bool pickBufferForceUpdate;
    bool pickBufferMaybeInvalid;
    bool updateQueued;
    QMap<int, QObject *> objects;
    QObject *pressedObject;
    Qt::MouseButton pressedButton;
    QObject *enteredObject;
    QGLCamera *defaultCamera;
    QGLCamera *camera;
    bool panning;
    QPoint startPan;
    QGLDepthBufferOptions depthBufferOptions;
    QGLBlendOptions blendOptions;
    QTime logTime;
    QTime enterTime;
    QTime lastFrameTime;

    inline void logEnter(const char *message);
    inline void logLeave(const char *message);
};

inline void QGLViewPrivate::logEnter(const char *message)
{
    if ((options & QGLView::PaintingLog) == 0)
        return;
    int ms = logTime.elapsed();
    enterTime.start();
    int sinceLast = lastFrameTime.restart();
    qDebug("LOG[%d:%02d:%02d.%03d]: ENTER: %s (%d ms since last enter)",
           ms / 3600000, (ms / 60000) % 60,
           (ms / 1000) % 60, ms % 1000, message, sinceLast);
}

inline void QGLViewPrivate::logLeave(const char *message)
{
    if ((options & QGLView::PaintingLog) == 0)
        return;
    int ms = logTime.elapsed();
    int duration = enterTime.elapsed();
    qDebug("LOG[%d:%02d:%02d.%03d]: LEAVE: %s (%d ms elapsed)",
           ms / 3600000, (ms / 60000) % 60,
           (ms / 1000) % 60, ms % 1000, message, duration);
}

static QGLFormat makeStereoGLFormat(const QGLFormat& format)
{
#if defined(GL_BACK_LEFT) && defined(GL_BACK_RIGHT)
    QGLFormat fmt(format);
    fmt.setOption(QGL::StereoBuffers);
    return fmt;
#else
    QGLFormat fmt(format);
    fmt.setOption(QGL::NoStereoBuffers);
    return fmt;
#endif
}

/*!
    Constructs a new view widget and attaches it to \a parent.

    This constructor will request a stereo rendering context if
    the hardware supports it.
*/
QGLView::QGLView(QWidget *parent)
    : QGLWidget(makeStereoGLFormat(QGLFormat::defaultFormat()), parent)
{
    d = new QGLViewPrivate(this);
    setMouseTracking(true);
}

/*!
    Constructs a new view widget and attaches it to \a parent.
    The \a format argument specifies the desired QGLFormat
    rendering options.

    If \a format does not include the stereo option, then a stereo
    viewing context will not be requested.
*/
QGLView::QGLView(const QGLFormat& format, QWidget *parent)
    : QGLWidget(format, parent)
{
    d = new QGLViewPrivate(this);
    setMouseTracking(true);
}

/*!
    Destroys this view widget.
*/
QGLView::~QGLView()
{
    delete d;
}

/*!
    Returns the options for this view.  The default value is
    CameraNavigation.

    \sa setOptions(), setOption()
*/
QGLView::Options QGLView::options() const
{
    return d->options;
}

/*!
    Sets the options for this view to \a value.

    \sa options(), setOption()
*/
void QGLView::setOptions(QGLView::Options value)
{
    d->options = value;
}

/*!
    Enables or disables \a option according to \a value.

    \sa options(), setOptions()
*/
void QGLView::setOption(QGLView::Option option, bool value)
{
    if (value)
        d->options |= option;
    else
        d->options &= ~option;
}

/*!
    Returns the type of stereo viewing technology that is in use:
    Hardware or RedCyanAnaglyph.
*/
QGLView::StereoType QGLView::stereoType() const
{
    return d->stereoType;
}

/*!
    Registers an \a object with this view to be notified when
    \a objectId is selected with the mouse.  The \a object must
    persist for the lifetime of the QGLView, or until
    deregisterObject() is called for \a objectId.

    \sa deregisterObject(), pickGL(), objectUnderMouse()
*/
void QGLView::registerObject(int objectId, QObject *object)
{
    d->objects[objectId] = object;
}

/*!
    Deregisters the object associated with \a objectId.

    \sa registerObject(), pickGL()
*/
void QGLView::deregisterObject(int objectId)
{
    d->objects.remove(objectId);
}

/*!
    Returns the camera parameters.  The camera defines the projection
    to apply to convert eye co-ordinates into window co-ordinates,
    and the position and orientation of the viewer's eye.

    \sa setCamera()
*/
QGLCamera *QGLView::camera() const
{
    return d->camera;
}

/*!
    Sets the camera parameters to \a value.  The camera defines the
    projection to apply to convert eye co-ordinates into window
    co-ordinates, and the position and orientation of the viewer's eye.

    If \a value is null, then the default camera object will be used.

    This function will call queueUpdate() to force the view to
    update with the new camera parameters upon the next event loop.

    \sa camera()
*/
void QGLView::setCamera(QGLCamera *value)
{
    if (!value)
        value = d->defaultCamera;

    if (d->camera == value)
        return;

    disconnect(d->camera, SIGNAL(projectionChanged()),
               this, SLOT(cameraChanged()));
    disconnect(d->camera, SIGNAL(viewChanged()),
               this, SLOT(cameraChanged()));

    d->camera = value;

    connect(d->camera, SIGNAL(projectionChanged()),
            this, SLOT(cameraChanged()));
    connect(d->camera, SIGNAL(viewChanged()),
            this, SLOT(cameraChanged()));

    cameraChanged();
}

/*!
    Maps \a point from viewport co-ordinates to eye co-ordinates.

    The returned vector will have its x and y components set to the
    position of the point on the near plane, and the z component
    set to the inverse of the camera's near plane.

    This function is used for converting a mouse event's position
    into eye co-ordinates within the current camera view.

    \sa QGLCamera::mapPoint()
*/
QVector3D QGLView::mapPoint(const QPoint &point) const
{
    QSize viewportSize(size());
    qreal aspectRatio;

    // Get the size of the underlying paint device.
    int width = viewportSize.width();
    int height = viewportSize.height();

    // Use the device's DPI setting to determine the pixel aspect ratio.
    int dpiX = logicalDpiX();
    int dpiY = logicalDpiY();
    if (dpiX <= 0 || dpiY <= 0)
        dpiX = dpiY = 1;

    // Derive the aspect ratio based on window and pixel size.
    if (width <= 0 || height <= 0)
        aspectRatio = 1.0f;
    else
        aspectRatio = ((qreal)(width * dpiY)) / ((qreal)(height * dpiX));

    // Map the point into eye co-ordinates.
    return d->camera->mapPoint(point, aspectRatio, viewportSize);
}

void QGLView::cameraChanged()
{
    // The pick buffer will need to be refreshed at the new camera position.
    d->pickBufferForceUpdate = true;

    // Queue an update for the next event loop.
    queueUpdate();
}

/*!
    Queues an updateGL() to happen during the next event loop.
    If multiple calls are made to this function, only one
    call to updateGL() occurs during the next event loop.
*/
void QGLView::queueUpdate()
{
    if (!d->updateQueued) {
        d->updateQueued = true;
        QTimer::singleShot(0, this, SLOT(performUpdate()));
    }
}

void QGLView::performUpdate()
{
    d->updateQueued = false;
    updateGL();
}

/*!
    \internal
*/
void QGLView::initializeGL()
{
    d->logEnter("QGLView::initializeGL");
    QGLPainter painter;
    painter.begin();
    d->depthBufferOptions.apply(&painter);
    d->blendOptions.apply(&painter);
    painter.setCullFaces(QGL::CullDisabled);
    initializeGL(&painter);
    d->logLeave("QGLView::initializeGL");
}

/*!
    \internal
*/
void QGLView::resizeGL(int w, int h)
{
    // Set up the standard viewport for the new window size.
    glViewport(0, 0, w, h);

    // We will need to regenerate the pick buffer.
    d->pickBufferForceUpdate = true;
}

/*!
    \internal
*/
void QGLView::paintGL()
{
    d->logEnter("QGLView::paintGL");
    // We may need to regenerate the pick buffer on the next mouse event.
    d->pickBufferMaybeInvalid = true;

    // Paint the scene contents.
    QGLPainter painter;
    painter.begin();
    if (d->options & QGLView::ShowPicking) {
        // If showing picking, then render normally.
        painter.setPicking(true);
        painter.clearPickObjects();
        earlyPaintGL(&painter);
        d->camera->apply(&painter);
        paintGL(&painter);
        painter.setPicking(false);
    } else if (d->camera->eyeSeparation() == 0.0f) {
        // No camera separation, so draw without stereo.  If the hardware
        // has stereo buffers, then render the same image into both buffers.
#if defined(GL_BACK_LEFT) && defined(GL_BACK_RIGHT)
        if (d->stereoType == QGLView::Hardware) {
            bool doubleBuffered = doubleBuffer();
            if (doubleBuffered)
                glDrawBuffer(GL_BACK);
            else
                glDrawBuffer(GL_FRONT);
            earlyPaintGL(&painter);
            d->camera->apply(&painter);
            paintGL(&painter);
        } else
#endif
        {
            earlyPaintGL(&painter);
            d->camera->apply(&painter);
            paintGL(&painter);
        }
    } else {
        // Determine the left and right camera positions and the viewport size.
        QVector3D vector = d->camera->translation
            (d->camera->eyeSeparation() / 2.0f, 0.0f, 0.0f);
        QSize viewportSize = QSize(width(), height());

        // Paint the scene twice, from the perspective of each camera.
        // In RedCyanAnaglyph mode, the color mask is set each time to only
        // extract the color planes that we want to see through that eye.
        if (d->stereoType == QGLView::RedCyanAnaglyph) {
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            earlyPaintGL(&painter);

            glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
            d->camera->apply(&painter, viewportSize, -vector);
            paintGL(&painter);

            glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
            glClear(GL_DEPTH_BUFFER_BIT);
            d->camera->apply(&painter, viewportSize, vector);
            paintGL(&painter);

            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
#if defined(GL_BACK_LEFT) && defined(GL_BACK_RIGHT)
        } else {
            bool doubleBuffered = doubleBuffer();
            if (doubleBuffered)
                glDrawBuffer(GL_BACK_LEFT);
            else
                glDrawBuffer(GL_FRONT_LEFT);
            earlyPaintGL(&painter);
            d->camera->apply(&painter, viewportSize, -vector);
            paintGL(&painter);

            if (doubleBuffered)
                glDrawBuffer(GL_BACK_RIGHT);
            else
                glDrawBuffer(GL_FRONT_RIGHT);
            earlyPaintGL(&painter);
            d->camera->apply(&painter, viewportSize, vector);
            paintGL(&painter);
#endif
        }
    }
    d->logLeave("QGLView::paintGL");
}

/*!
    Initializes the current GL context represented by \a painter.

    \sa paintGL()
*/
void QGLView::initializeGL(QGLPainter *painter)
{
    Q_UNUSED(painter);
}

/*!
    Performs early painting operations just after \a painter
    is initialized but before the camera is set up.  The default
    implementation clears the color buffer and depth buffer.

    This function is typically overridden to draw scene backdrops
    on the color buffer before the rest of the scene is drawn
    by paintGL().

    \sa paintGL()
*/
void QGLView::earlyPaintGL(QGLPainter *painter)
{
    painter->clear();
}

/*!
    \fn void QGLView::paintGL(QGLPainter *painter)

    Paints the scene onto \a painter.  The color and depth buffers
    will have already been cleared, and the camera() position set.

    \sa pickGL(), earlyPaintGL()
*/

/*!
    Paints the scene onto \a painter in "object picking mode".
    The scene is rendered into a background buffer using flat
    colors so that mouse events can determine which object lies
    under the mouse pointer.

    The default implementation calls paintGL().  Subclasses may
    override the default implementation of pickGL() to draw a
    simpler version of the scene.

    \sa needsPickGL(), paintGL(), QGLPainter::setPicking()
*/
void QGLView::pickGL(QGLPainter *painter)
{
    paintGL(painter);
}

/*!
    Returns true if pickGL() needs to be called to refresh
    the contents of the pick buffer; false otherwise.

    The default implementation returns true if paintGL() has
    been called since the last call to pickGL().  This can cause
    the pick buffer to be regenerated very often if the scene
    is animated.

    If the animations are limited to objects bouncing in place,
    rotating on an axis, or moving along a short path, the pick
    buffer probably does not need to be regenerated every time
    paintGL() is called.

    Subclasses can override needsPickGL() and return false if
    the scene has not changed substantially enough to require a
    pick buffer change.  Subclasses will usually also override
    pickGL() and render an elongated version of the object into
    the pick buffer that covers the complete animation path.
    This way, the user can click anywhere along the animation
    path to select the object rather than having to hit a
    moving object exactly to select it.

    \sa pickGL()
*/
bool QGLView::needsPickGL()
{
    return d->pickBufferMaybeInvalid;
}

/*!
    Processes the mouse press event \a e.
*/
void QGLView::mousePressEvent(QMouseEvent *e)
{
    QObject *object;
    if (!d->panning && (d->options & QGLView::ObjectPicking) != 0)
        object = objectUnderMouse(e);
    else
        object = 0;
    if (d->pressedObject) {
        // Send the press event to the pressed object.  Use a position
        // of (0, 0) if the mouse is still within the pressed object,
        // or (-1, -1) if the mouse is no longer within the pressed object.
        QMouseEvent event
            (QEvent::MouseButtonPress,
             (d->pressedObject == object) ? QPoint(0, 0) : QPoint(-1, -1),
             e->globalPos(), e->button(), e->buttons(), e->modifiers());
        QCoreApplication::sendEvent(d->pressedObject, &event);
    } else if (object) {
        // Record the object that was pressed and forward the event.
        d->pressedObject = object;
        d->enteredObject = 0;
        d->pressedButton = e->button();

        // Send a mouse press event for (0, 0).
        QMouseEvent event(QEvent::MouseButtonPress, QPoint(0, 0),
                          e->globalPos(), e->button(), e->buttons(),
                          e->modifiers());
        QCoreApplication::sendEvent(object, &event);
    } else if ((d->options & QGLView::CameraNavigation) != 0 &&
                    e->button() == Qt::LeftButton) {
        d->panning = true;
        d->startPan = e->pos();
#ifndef QT_NO_CURSOR
        setCursor(Qt::ClosedHandCursor);
#endif
    }
    QGLWidget::mousePressEvent(e);
}

/*!
    Processes the mouse release event \a e.
*/
void QGLView::mouseReleaseEvent(QMouseEvent *e)
{
    if (d->panning && e->button() == Qt::LeftButton) {
        d->panning = false;
#ifndef QT_NO_CURSOR
        unsetCursor();
#endif
    }
    if (d->pressedObject) {
        // Notify the previously pressed object about the release.
        QObject *object = objectUnderMouse(e);
        QObject *pressed = d->pressedObject;
        if (e->button() == d->pressedButton) {
            d->pressedObject = 0;
            d->pressedButton = Qt::NoButton;
            d->enteredObject = object;

            // Send the release event to the pressed object.  Use a position
            // of (0, 0) if the mouse is still within the pressed object,
            // or (-1, -1) if the mouse is no longer within the pressed object.
            QMouseEvent event
                (QEvent::MouseButtonRelease,
                 (pressed == object) ? QPoint(0, 0) : QPoint(-1, -1),
                 e->globalPos(), e->button(), e->buttons(), e->modifiers());
            QCoreApplication::sendEvent(pressed, &event);

            // Send leave and enter events if necessary.
            if (object != pressed) {
                sendLeaveEvent(pressed);
                if (object)
                    sendEnterEvent(object);
            }
        } else {
            // Some other button than the original was released.
            // Forward the event to the pressed object.
            QMouseEvent event
                (QEvent::MouseButtonRelease,
                 (pressed == object) ? QPoint(0, 0) : QPoint(-1, -1),
                 e->globalPos(), e->button(), e->buttons(), e->modifiers());
            QCoreApplication::sendEvent(pressed, &event);
        }
    }
    QGLWidget::mouseReleaseEvent(e);
}

/*!
    Processes the mouse double click event \a e.
*/
void QGLView::mouseDoubleClickEvent(QMouseEvent *e)
{
    if ((d->options & QGLView::ObjectPicking) != 0) {
        QObject *object = objectUnderMouse(e);
        if (object) {
            // Simulate a double click event for (0, 0).
            QMouseEvent event
                (QEvent::MouseButtonDblClick, QPoint(0, 0),
                 e->globalPos(), e->button(), e->buttons(), e->modifiers());
            QCoreApplication::sendEvent(object, &event);
        }
    }
    QGLWidget::mouseDoubleClickEvent(e);
}

/*!
    Processes the mouse move event \a e.
*/
void QGLView::mouseMoveEvent(QMouseEvent *e)
{
    if (d->panning) {
        QPoint delta = e->pos() - d->startPan;
        d->startPan = e->pos();
        if ((e->modifiers() & Qt::ControlModifier) != 0)
            wheel(delta.y() * -60);
        else if ((e->modifiers() & Qt::ShiftModifier) != 0)
            pan(delta.x(), delta.y());
        else
            rotate(delta.x(), delta.y());
    } else if ((d->options & QGLView::ObjectPicking) != 0) {
        QObject *object = objectUnderMouse(e);
        if (d->pressedObject) {
            // Send the move event to the pressed object.  Use a position
            // of (0, 0) if the mouse is still within the pressed object,
            // or (-1, -1) if the mouse is no longer within the pressed object.
            QMouseEvent event
                (QEvent::MouseMove,
                 (d->pressedObject == object) ? QPoint(0, 0) : QPoint(-1, -1),
                 e->globalPos(), e->button(), e->buttons(), e->modifiers());
            QCoreApplication::sendEvent(d->pressedObject, &event);
        } else if (object) {
            if (object != d->enteredObject) {
                if (d->enteredObject)
                    sendLeaveEvent(d->enteredObject);
                d->enteredObject = object;
                sendEnterEvent(d->enteredObject);
            }
            QMouseEvent event
                (QEvent::MouseMove, QPoint(0, 0),
                 e->globalPos(), e->button(), e->buttons(), e->modifiers());
            QCoreApplication::sendEvent(object, &event);
        } else if (d->enteredObject) {
            sendLeaveEvent(d->enteredObject);
            d->enteredObject = 0;
        }
    }
    QGLWidget::mouseMoveEvent(e);
}

/*!
    Processes the leave event \a e.
*/
void QGLView::leaveEvent(QEvent *e)
{
    if (!d->pressedObject && d->enteredObject) {
        sendLeaveEvent(d->enteredObject);
        d->enteredObject = 0;
    }
    QGLWidget::leaveEvent(e);
}

#ifndef QT_NO_WHEELEVENT

/*!
    Processes the wheel event \a e.
*/
void QGLView::wheelEvent(QWheelEvent *e)
{
    if ((d->options & QGLView::CameraNavigation) != 0)
        wheel(e->delta());
    QGLWidget::wheelEvent(e);
}

#endif

/*!
    Processes the key press event \a e.
*/
void QGLView::keyPressEvent(QKeyEvent *e)
{
    QGLCamera *camera;
    qreal sep;

    if ((d->options & QGLView::CameraNavigation) == 0) {
        QGLWidget::keyPressEvent(e);
        return;
    }
    switch (e->key()) {

        case Qt::Key_Left:
        {
            if ((e->modifiers() & Qt::ShiftModifier) != 0) {
                pan(-10, 0);
            } else if ((e->modifiers() & Qt::ControlModifier) != 0) {
                camera = this->camera();
                sep = camera->eyeSeparation();
                sep -= (sep / 10.0f);
                if (sep < 0.0f)
                    sep = 0.0f;
                camera->setEyeSeparation(sep);
                e->accept();
                return;
            } else {
                rotate(-10, 0);
            }
        }
        break;

        case Qt::Key_Right:
        {
            if ((e->modifiers() & Qt::ShiftModifier) != 0) {
                pan(10, 0);
            } else if ((e->modifiers() & Qt::ControlModifier) != 0) {
                camera = this->camera();
                sep = camera->eyeSeparation();
                sep += (sep / 10.0f);
                camera->setEyeSeparation(sep);
                e->accept();
                return;
            } else {
                rotate(10, 0);
            }
        }
        break;

        case Qt::Key_Up:
        {
            if ((e->modifiers() & Qt::ControlModifier) != 0)
                wheel(120);
            else if ((e->modifiers() & Qt::ShiftModifier) != 0)
                pan(0, -10);
            else
                rotate(0, -10);
        }
        break;

        case Qt::Key_Down:
        {
            if ((e->modifiers() & Qt::ControlModifier) != 0)
                wheel(-120);
            else if ((e->modifiers() & Qt::ShiftModifier) != 0)
                pan(0, 10);
            else
                rotate(0, 10);
        }
        break;
    }
    QGLWidget::keyPressEvent(e);
}

// Find the next power of 2 which is "value" or greater.
static inline int powerOfTwo(int value)
{
    int p = 1;
    while (p < value)
        p <<= 1;
    return p;
}

/*!
    Returns the registered object that is under the mouse position
    specified by \a e.  This may be used by subclasses to re-implement
    mousePressEvent(), mouseReleaseEvent(), etc.

    \sa registerObject()
*/
QObject *QGLView::objectUnderMouse(QMouseEvent *e)
{
    // Check the window boundaries in case a mouse move has
    // moved the pointer outside the window.
    int x = e->x();
    int y = e->y();
    if (!rect().contains(x, y))
        return 0;

    // Do we need to refresh the pick buffer contents?
    QGLPainter painter(this);
    if (d->pickBufferForceUpdate || needsPickGL()) {
        // Initialize the painter, which will make the window context current.
        painter.setPicking(true);
        painter.clearPickObjects();

        // Create a framebuffer object as big as the window to act
        // as the pick buffer.  TODO: use the window back buffer if no fbo's.
        QSize fbosize = size();
        fbosize = QSize(powerOfTwo(fbosize.width()), powerOfTwo(fbosize.height()));
        if (!d->fbo) {
            d->fbo = new QGLFramebufferObject(fbosize, QGLFramebufferObject::CombinedDepthStencil);
        } else if (d->fbo->size() != fbosize) {
            delete d->fbo;
            d->fbo = new QGLFramebufferObject(fbosize, QGLFramebufferObject::CombinedDepthStencil);
        }

        // Render the pick version of the scene into the framebuffer object.
        d->fbo->bind();
        painter.clear();
        d->camera->apply(&painter);
        pickGL(&painter);
        painter.setPicking(false);

        // The pick buffer contents are now valid.
        d->pickBufferForceUpdate = false;
        d->pickBufferMaybeInvalid = false;
    } else {
        // Bind the framebuffer object to the window's context.
        makeCurrent();
        if (d->fbo)
            d->fbo->bind();
    }

    // Pick the object under the mouse.
    int objectId = painter.pickObject(x, height() - 1 - y);    
    QObject *object = d->objects.value(objectId, 0);
    
    // Release the framebuffer object and return.
    painter.end();
    if (d->fbo)
        d->fbo->release();
    doneCurrent();
    return object;
}

void QGLView::sendEnterEvent(QObject *object)
{
    QEvent event(QEvent::Enter);
    QCoreApplication::sendEvent(object, &event);
}

void QGLView::sendLeaveEvent(QObject *object)
{
    QEvent event(QEvent::Leave);
    QCoreApplication::sendEvent(object, &event);
}

// Zoom in and out according to the change in wheel delta.
void QGLView::wheel(int delta)
{
    qreal scale = qAbs(viewDelta(delta, delta).x());
    if (delta < 0)
        scale = -scale;
    if (scale >= 0.0f)
        scale += 1.0f;
    else
        scale = 1.0f / (1.0f - scale);
    qreal fov = d->camera->fieldOfView();
    if (fov != 0.0f)
        d->camera->setFieldOfView(d->camera->fieldOfView() / scale);
    else
        d->camera->setViewSize(d->camera->viewSize() / scale);
}

// Pan left/right/up/down without rotating about the object.
void QGLView::pan(int deltax, int deltay)
{
    QPointF delta = viewDelta(deltax, deltay);
    QVector3D t = d->camera->translation(delta.x(), -delta.y(), 0.0f);

    // Technically panning the eye left should make the object appear to
    // move off to the right, but this looks weird on-screen where the user
    // actually thinks they are picking up the object and dragging it rather
    // than moving the eye.  We therefore apply the inverse of the translation
    // to make it "look right".
    d->camera->translateEye(-t);
    d->camera->translateCenter(-t);
}

// Rotate about the object being viewed.
void QGLView::rotate(int deltax, int deltay)
{
    int rotation = d->camera->screenRotation();
    if (rotation == 90 || rotation == 270) {
        qSwap(deltax, deltay);
    }
    if (rotation == 90 || rotation == 180) {
        deltax = -deltax;
    }
    if (rotation == 180 || rotation == 270) {
        deltay = -deltay;
    }
    qreal anglex = deltax * 90.0f / width();
    qreal angley = deltay * 90.0f / height();
    QQuaternion q = d->camera->pan(-anglex);
    q *= d->camera->tilt(-angley);
    d->camera->rotateCenter(q);
}

/*!
    Converts \a deltax and \a deltay into percentages of the
    view width and height.  Returns a QPointF containing the
    percentage values, typically between -1 and 1.

    This function is typically used by subclasses to convert a
    change in mouse position into a relative distance travelled
    across the field of view.

    The returned value is corrected for the camera() screen
    rotation and view size.
*/
QPointF QGLView::viewDelta(int deltax, int deltay) const
{
    int w = width();
    int h = height();
    bool scaleToWidth;
    qreal scaleFactor, scaleX, scaleY;
    QSizeF viewSize = d->camera->viewSize();
    if (w >= h) {
        if (viewSize.width() >= viewSize.height())
            scaleToWidth = true;
        else
            scaleToWidth = false;
    } else {
        if (viewSize.width() >= viewSize.height())
            scaleToWidth = false;
        else
            scaleToWidth = true;
    }
    int rotation = d->camera->screenRotation();
    if (rotation == 90 || rotation == 270) {
        scaleToWidth = !scaleToWidth;
        qSwap(deltax, deltay);
    }
    if (rotation == 90 || rotation == 180) {
        deltax = -deltax;
    }
    if (rotation == 180 || rotation == 270) {
        deltay = -deltay;
    }
    if (scaleToWidth) {
        scaleFactor = 2.0f / viewSize.width();
        scaleX = scaleFactor * ((qreal)h) / ((qreal)w);
        scaleY = scaleFactor;
    } else {
        scaleFactor = 2.0f / viewSize.height();
        scaleX = scaleFactor;
        scaleY = scaleFactor * ((qreal)w) / ((qreal)h);
    }
    return QPointF(deltax * scaleX / w, deltay * scaleY / h);
}

/*!
    \fn QPointF QGLView::viewDelta(const QPoint &delta) const
    \overload

    Converts the x and y components of \a delta into percentages
    of the view width and height.  Returns a QPointF containing
    the percentage values, typically between -1 and 1.
*/

QT_END_NAMESPACE
