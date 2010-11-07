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

#include "viewport.h"
#include "effect.h"
#include "qgllightmodel.h"
#include "qgllightparameters.h"
#include "qglcamera.h"
#include "qglview.h"
#include "qglsubsurface.h"
#include "qglframebufferobjectsurface.h"
#include "stereoview.h"
#include <QtGui/qpainter.h>
#include <QtGui/qgraphicsview.h>
#include <QtGui/qgraphicsscene.h>
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qevent.h>
#include <QtOpenGL/qglframebufferobject.h>
#include <QtCore/qtimer.h>
#include <QtCore/qcoreapplication.h>

/*!
    \qmlclass Viewport Viewport
    \brief The Viewport item defines the logical viewport for a 3D scene.  It includes all necessary
    references and parameters for the contents of the scene, as well as drawing and painting functions
    \since 4.8
    \ingroup qt3d::qml3d

    The Viewport item is usually the outermost in a 3D scene, specifying
    the size of the view, the camera position, lights, and the main 3D object:

    \code
    import Qt 4.7
    import Qt3D 1.0

    Viewport {
        width: 640; height: 480
        camera: Camera {}
        light: Light {}
        Item3D {
            mesh: Mesh { source: "meshes/teapot.bez" }
            effect: Effect {}
        }
    }
    \endcode

    \sa Camera, StereoView
*/

QT_BEGIN_NAMESPACE

class ViewportPrivate
{
public:
    ViewportPrivate();
    ~ViewportPrivate();

    bool picking;
    bool showPicking;
    bool navigation;
    bool blending;
    bool itemsInitialized;
    bool needsPick;
    QGLCamera *camera;
    QGLLightParameters *light;
    QGLLightModel *lightModel;
    Effect *backdrop;
    QColor backgroundColor;
    QGLVertexBundle backdropVertices;
    QGLView *view;
    QWidget *viewWidget;
    int pickId;
    QGLFramebufferObject *pickFbo;
    QMap<int, QObject *> objects;
    QObject *pressedObject;
    Qt::MouseButton pressedButton;
    QObject *enteredObject;
    bool panning;
    QPointF startPan;
    QPointF lastPan;
    QVector3D startEye;
    QVector3D startCenter;
    QVector3D startUpVector;
    Qt::KeyboardModifiers panModifiers;

    void setDefaults(QGLPainter *painter);
};

ViewportPrivate::ViewportPrivate()
    : picking(false)
    , showPicking(false)
    , navigation(true)
    , blending(false)
    , itemsInitialized(false)
    , needsPick(true)
    , camera(0)
    , light(0)
    , lightModel(0)
    , backdrop(0)
    , backgroundColor(Qt::black)
    , view(0)
    , viewWidget(0)
    , pickId(1)
    , pickFbo(0)
    , pressedObject(0)
    , pressedButton(Qt::NoButton)
    , enteredObject(0)
    , panning(false)
    , startPan(-1, -1)
    , lastPan(-1, -1)
    , panModifiers(Qt::NoModifier)
{
    // Construct the vertices for a quad with (0, 0) as the
    // texture co-ordinate for the bottom-left of the screen
    // and (1, 1) as the texture co-ordinate for the top-right.

    QArray<QVector2D> pos;
    pos.append(QVector2D(-1.0f, -1.0f));
    pos.append(QVector2D(-1.0f,  1.0f));
    pos.append(QVector2D( 1.0f,  1.0f));
    pos.append(QVector2D( 1.0f, -1.0f));

    QArray<QVector2D> tex;
    pos.append(QVector2D(0.0f, 0.0f));
    pos.append(QVector2D(0.0f, 1.0f));
    pos.append(QVector2D(1.0f, 1.0f));
    pos.append(QVector2D(1.0f, 0.0f));

    backdropVertices.addAttribute(QGL::Position, pos);
    backdropVertices.addAttribute(QGL::TextureCoord0, tex);

    //backdropVertices.append(-1.0f, -1.0f);
    //backdropVertices.append(0.0f, 0.0f);
    //backdropVertices.append(-1.0f, 1.0f);
    //backdropVertices.append(0.0f, 1.0f);
    //backdropVertices.append(1.0f, 1.0f);
    //backdropVertices.append(1.0f, 1.0f);
    //backdropVertices.append(1.0f, -1.0f);
    //backdropVertices.append(1.0f, 0.0f);
}

ViewportPrivate::~ViewportPrivate()
{
    delete pickFbo;
}

void ViewportPrivate::setDefaults(QGLPainter *painter)
{
    // Set the default depth buffer options.
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
#if defined(QT_OPENGL_ES)
    glDepthRangef(0.0f, 1.0f);
#else
    glDepthRange(0.0f, 1.0f);
#endif

    // Set the default blend options.
    glDisable(GL_BLEND);
    if (painter->hasOpenGLFeature(QOpenGLFunctions::BlendColor))
        painter->glBlendColor(0, 0, 0, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (painter->hasOpenGLFeature(QOpenGLFunctions::BlendEquation))
        painter->glBlendEquation(GL_FUNC_ADD);
    else if (painter->hasOpenGLFeature(QOpenGLFunctions::BlendEquationSeparate))
        painter->glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
}

void qt_gl_set_qml_viewport(QObject *viewport);

/*!
    \internal
    Construct the class and assign it a \a parent QDeclarativeItem.
*/
Viewport::Viewport(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
{
    d = new ViewportPrivate();
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    qt_gl_set_qml_viewport(this);

    connect(this, SIGNAL(viewportChanged()), this, SLOT(update3d()));

    setCamera(new QGLCamera(this));
    setLight(new QGLLightParameters(this));

    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
}

/*!
    \internal
    Class destruction and cleanup.
*/
Viewport::~Viewport()
{
    delete d;
}

/*!
    \qmlproperty bool Viewport::picking

    User interaction in QML/3d is handled through the concept of object picking.  Each
    item has a unique picking id which s queried for a given screen click position when the
    mouse is clicked.

    If this property is set to true, picking will be supported for this
    viewport, while if the property is false, no picking will be applied.

    The default value for this property is false.

    \sa showPicking
*/
bool Viewport::picking() const
{
    return d->picking;
}

void Viewport::setPicking(bool value)
{
    d->picking = value;
    emit viewportChanged();
}

/*!
    \qmlproperty bool Viewport::showPicking
    The underlying mechanism for picking is based on painting an off-screen buffer with a flat
    coloured image containing all of the objects with a unique color value.

    Setting this property to true will display this flat-colour picking
    representation in the viewport, which can be useful for debugging
    problems with object selection.

    The default value for this property is false.

    \sa picking
*/

bool Viewport::showPicking() const
{
    return d->showPicking;
}

void Viewport::setShowPicking(bool value)
{
    d->showPicking = value;
    emit viewportChanged();
}

/*!
    \qmlproperty bool Viewport::navigation

    This property is used to set or unset camera navigation in for the viewport.
    Camera navigation allows the user to move the camera position around using the moose.

    By default, camera navigation is set to true.
*/
bool Viewport::navigation() const
{
    return d->navigation;
}

void Viewport::setNavigation(bool value)
{
    d->navigation = value;
    emit viewportChanged();
}

/*!
    \qmlproperty bool Viewport::blending

    The blending property is used to enable or disable GL_BLEND
    on the viewport, for alpha blending of drawn objects.

    By default, blending is set to false.

    \sa Effect::blending
*/
bool Viewport::blending() const
{
    return d->blending;
}

void Viewport::setBlending(bool value)
{
    d->blending = value;
    emit viewportChanged();
}

/*!
    \qmlproperty Camera Viewport::camera

    This property sets the camera parameters which will be used for
    the appropriate viewing transforms in OpenGL.  The default is
    a perspective camera with its eye located at (0, 0, 10) looking
    at the center (0, 0, 0), with the y axis as up.
*/
QGLCamera *Viewport::camera() const
{
    return d->camera;
}

void Viewport::setCamera(QGLCamera *value)
{
    if (d->camera != value) {
        if (d->camera) {
            disconnect(d->camera, SIGNAL(projectionChanged()),
                       this, SLOT(cameraChanged()));
            disconnect(d->camera, SIGNAL(viewChanged()),
                       this, SLOT(cameraChanged()));
        }
        d->camera = value;
        if (d->camera) {
            connect(d->camera, SIGNAL(projectionChanged()),
                    this, SLOT(cameraChanged()));
            connect(d->camera, SIGNAL(viewChanged()),
                    this, SLOT(cameraChanged()));
        }
        cameraChanged();
    }
}

/*!
    \qmlproperty Light Viewport::light

    This property defines the main scene light to use for 3D items
    that are drawn in this viewport.

    \sa lightModel
*/

QGLLightParameters *Viewport::light() const
{
    return d->light;
}

void Viewport::setLight(QGLLightParameters *value)
{
    if (d->light != value) {
        if (d->light) {
            disconnect(d->light, SIGNAL(lightChanged()),
                       this, SLOT(update3d()));
        }
        d->light = value;
        if (d->light) {
            connect(d->light, SIGNAL(lightChanged()),
                    this, SLOT(update3d()));
        }
        emit viewportChanged();
    }
}

/*!
    \qmlproperty LightModel Viewport::lightModel
    The user is able to set a lighting model for the 3d environment through the use of the
    lightModel property.  By default the light model is undefined.

    \sa light
*/
QGLLightModel *Viewport::lightModel() const
{
    return d->lightModel;
}

void Viewport::setLightModel(QGLLightModel *value)
{
    if (d->lightModel != value) {
        if (d->lightModel) {
            disconnect(d->lightModel, SIGNAL(lightModelChanged()),
                       this, SLOT(update3d()));
        }
        d->lightModel = value;
        if (d->lightModel) {
            connect(d->lightModel, SIGNAL(lightModelChanged()),
                    this, SLOT(update3d()));
        }
        emit viewportChanged();
    }
}

/*!
    \qmlproperty Effect Viewport::backdrop
    The user can set the backdrop property of the Viewport class to define a backdrop effect
    for the 3d environment.  This backdrop is defined as an Effect object, and can take on any of
    the effects supported.

    By default no backdrop effect is defined.

    \sa Effect, backgroundColor
*/
Effect *Viewport::backdrop() const
{
    return d->backdrop;
}

void Viewport::setBackdrop(Effect *value)
{
    if (d->backdrop != value) {
        if (d->backdrop) {
            disconnect(d->backdrop, SIGNAL(effectChanged()),
                       this, SLOT(update3d()));
        }
        d->backdrop = value;
        if (d->backdrop) {
            connect(d->backdrop, SIGNAL(effectChanged()),
                    this, SLOT(update3d()));
            d->backdrop->setUseLighting(false);
        }
        emit viewportChanged();
    }
}

/*!
    \qmlproperty color Viewport::backgroundColor

    The background color for the viewport, which is used if
    backdrop is not specified.  The default color is black.

    Setting this property to \c{"transparent"} will result in no
    background color being set, so that items behind this viewport
    will be visible through the viewport.

    \sa backdrop
*/
QColor Viewport::backgroundColor() const
{
    return d->backgroundColor;
}

void Viewport::setBackgroundColor(const QColor &value)
{
    if (d->backgroundColor != value) {
        d->backgroundColor = value;
        emit viewportChanged();
    }
}

/*!
    \internal
    The main paint function for the Viewport class.  It takes a  QPainter \a p, which performs the
    painting of objects in the 3d environment.

    The paint function is responsible for setting up the viewing transform, as well as other display
    options, before calling the draw function to perform the actual drawing of the scene.

    Note, currently \a style and \a widget are unused, but are reserved for later development.
*/
void Viewport::paint(QPainter *p, const QStyleOptionGraphicsItem * style, QWidget *widget)
{
    Q_UNUSED(style);

    if (!d->viewWidget)
        d->viewWidget = widget;
    d->needsPick = true;

    QGLPainter painter;
    if (!painter.begin(p)) {
        if (widget) {
            // Probably running with a plain QDeclarativeView (e.g. qmlviewer).
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
        qWarning("GL graphics system is not active; cannot use 3D items");
        return;
    }

    // Initialize the objects in the scene if this is the first paint.
    if (!d->itemsInitialized) {
        initialize(0);
        initializeGL(&painter);
    }

    // If this Viewport is surrounded by a StereoView item,
    // then fetch the eye to be rendered from it.
    StereoView *stereoView = StereoView::findView(this);
    if (stereoView)
        painter.setEye(stereoView->eye());
    else
        painter.setEye(QGL::NoEye);

    // Modify the GL viewport to only cover the extent of this QDeclarativeItem.
    QTransform transform = p->combinedTransform();
    QRect viewport(qRound(transform.dx()), qRound(transform.dy()),
                   width(), height());
    QGLSubsurface surface(painter.currentSurface(), viewport);
    painter.pushSurface(&surface);

    // Perform early drawing operations.
    earlyDraw(&painter);

    // Set up the camera the way QGLView would if we were using it.
    if (d->camera) {
        painter.setCamera(d->camera);
    } else {
        QGLCamera defCamera;
        painter.setCamera(&defCamera);
    }

    // Draw the Item3D children.
    draw(&painter);
    painter.setPicking(false);
    painter.popSurface();

    // Disable the effect to return control to the GL paint engine.
    painter.disableEffect();

    // Try to restore the GL state to something paint-engine compatible.
    glDisable(GL_CULL_FACE);
    d->setDefaults(&painter);
    glDisable(GL_DEPTH_TEST);

}

/*!
  \internal
  Some elements of the image are drawn as an "early" draw activity.  Essentially it performs clearing
  and drawing of backdrop effects in preparation for primary drawing activities using \a painter.
*/
void Viewport::earlyDraw(QGLPainter *painter)
{
    // If are running with the regular qml viewer, then assume that it
    // has cleared the background for us, and just clear the depth buffer.
    if (!d->view && parentItem() && !d->showPicking) {
        glClear(GL_DEPTH_BUFFER_BIT);
    } else {
        if (d->showPicking)
            painter->setClearColor(Qt::black);
        else
            painter->setClearColor(d->backgroundColor);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // Force the effect to be updated.  The GL paint engine
    // has left the GL state in an unknown condition.
    painter->disableEffect();

    // If we have a scene backdrop, then draw it now.
    if (d->backdrop) {
        painter->projectionMatrix().setToIdentity();
        painter->modelViewMatrix().setToIdentity();
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDepthMask(GL_FALSE);

        // Select the effect and draw the backdrop quad.
        d->backdrop->enableEffect(painter);
        painter->clearAttributes();
        painter->setVertexBundle(d->backdropVertices);
        painter->draw(QGL::TriangleFan, 4);
        d->backdrop->disableEffect(painter);

        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
    } else if (d->backgroundColor.alpha() != 0 || !parentItem()) {
        painter->projectionMatrix().setToIdentity();
        painter->modelViewMatrix().setToIdentity();
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDepthMask(GL_FALSE);

        painter->setStandardEffect(QGL::FlatColor);
        painter->setColor(d->backgroundColor);
        painter->clearAttributes();
        painter->setVertexBundle(d->backdropVertices);
        painter->draw(QGL::TriangleFan, 4);

        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
    }

#ifdef GL_RESCALE_NORMAL
    glEnable(GL_RESCALE_NORMAL);
#endif

    // Set the default effect for the scene.
    painter->setStandardEffect(QGL::LitMaterial);
    painter->setFaceColor(QGL::AllFaces, Qt::white);
}

/*!
  \internal
  The draw function for the viewport sets up all of the lighting parameters for the scene before
  iterating through the top level items in the scene and drawing them using \a painter.

  As a matter of course each item will draw its child items, and so on, until the entire image has
  been populated.
*/
void Viewport::draw(QGLPainter *painter)
{
    // Set up the initial depth, blend, and other options.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
#if defined(QT_OPENGL_ES)
    glDepthRangef(0.0f, 1.0f);
#else
    glDepthRange(0.0f, 1.0f);
#endif
    if (d->blending)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
    if (painter->hasOpenGLFeature(QOpenGLFunctions::BlendColor))
        painter->glBlendColor(0, 0, 0, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (painter->hasOpenGLFeature(QOpenGLFunctions::BlendEquation))
        painter->glBlendEquation(GL_FUNC_ADD);
    else if (painter->hasOpenGLFeature(QOpenGLFunctions::BlendEquationSeparate))
        painter->glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glDisable(GL_CULL_FACE);
    if (!d->view)
        painter->setPicking(d->showPicking);

    painter->setObjectPickId(-1);
    QObjectList list = QObject::children();    
    painter->setMainLight(d->light, QMatrix4x4());
    painter->setLightModel(d->lightModel);
    foreach (QObject *child, list) {
        Item3D *item = qobject_cast<Item3D *>(child);
        if (item)
            item->draw(painter);
    }
}

/*!
  \internal
  The initialize function, as its name suggests, peforms all top level initialisation for the viewport.

  This includes setting up the camera, as well as initialising all of the

  If \a view is null, then we are running on a standard QFxView canvas widget.  If \a view is not null,
  then we are running on a QGLView canvas widget.
*/

void Viewport::initialize(QGLView *view)
{
    // Remember which view we are associated with, if any.
    d->view = view;

    // Set up the QGLView size and properties as requested by the viewport.
    if (view) {
        int w = width();
        if (w > 0)
            view->setMinimumWidth(w);
        int h = height();
        if (h > 0)
            view->setMinimumHeight(h);
        view->setOption(QGLView::ObjectPicking, picking());
        view->setOption(QGLView::ShowPicking, showPicking());
        view->setOption(QGLView::CameraNavigation, navigation());
    }

    // Apply the camera to the view.
    if (view && d->camera)
        view->setCamera(d->camera);
}

/*!
  \internal
  Initialize the GL viewport for the first time on \a painter.
*/

void Viewport::initializeGL(QGLPainter *painter)
{
    // Initialize the Item3D objects attached to this scene.
    QObjectList list = QObject::children();
    foreach (QObject *child, list) {
        Item3D *item = qobject_cast<Item3D *>(child);
        if (item)
            item->initialize(this, painter);
    }
    d->itemsInitialized = true;
}

/*!
  \internal
  Return the QGLView being used by the viewport.
*/
QGLView *Viewport::view() const
{
    return d->view;
}

/*!
  \internal
    Registers \a obj with this viewport as a pickable object and
    return its pick identifier.
*/
int Viewport::registerPickableObject(QObject *obj)
{
    int id = (d->pickId)++;
    if (d->view)
        d->view->registerObject(id, obj);
    else
        d->objects[id] = obj;
    return id;
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
  \internal
    Returns the registered object that is under the mouse position
    specified by (\a x, \a y).  This function may need to regenerate
    the contents of the pick buffer by repainting the scene.
*/
QObject *Viewport::objectForPoint(int x, int y)
{
    if (d->view)
        return d->view->objectForPoint(QPoint(x, y));
    if (!d->viewWidget)
        return 0;

    QPainter qpainter;
    QGLPainter painter;
    QGLWidget *glw = qobject_cast<QGLWidget *>(d->viewWidget);
    if (glw) {
        if (!painter.begin(glw))
            return 0;
    } else {
        qpainter.begin(d->viewWidget);
        if (!painter.begin(&qpainter))
            return 0;
    }

    int objectId = 0;

    QSize size(width(), height());
    QSize fbosize(powerOfTwo(size.width()), powerOfTwo(size.height()));
    if (!d->needsPick && d->pickFbo && d->pickFbo->size() == fbosize) {
        // The previous pick fbo contents should still be valid.
        d->pickFbo->bind();
        objectId = painter.pickObject(x, height() - 1 - y);
        d->pickFbo->release();
    } else {
        // Regenerate the pick fbo contents.
        if (d->pickFbo && d->pickFbo->size() != fbosize) {
            delete d->pickFbo;
            d->pickFbo = 0;
        }
        if (!d->pickFbo) {
            d->pickFbo = new QGLFramebufferObject
                (fbosize, QGLFramebufferObject::CombinedDepthStencil);
        }
        QGLFramebufferObjectSurface surface(d->pickFbo);
        painter.pushSurface(&surface);
        painter.setPicking(true);
        painter.clearPickObjects();
        painter.setClearColor(Qt::black);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        painter.setEye(QGL::NoEye);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
#if defined(QT_OPENGL_ES)
        glDepthRangef(0.0f, 1.0f);
#else
        glDepthRange(0.0f, 1.0f);
#endif
        glDisable(GL_BLEND);
        if (painter.hasOpenGLFeature(QOpenGLFunctions::BlendColor))
            painter.glBlendColor(0, 0, 0, 0);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        if (painter.hasOpenGLFeature(QOpenGLFunctions::BlendEquation))
            painter.glBlendEquation(GL_FUNC_ADD);
        else if (painter.hasOpenGLFeature(QOpenGLFunctions::BlendEquationSeparate))
            painter.glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glDisable(GL_CULL_FACE);
        if (d->camera) {
            painter.setCamera(d->camera);
        } else {
            QGLCamera defCamera;
            painter.setCamera(&defCamera);
        }
        draw(&painter);
        painter.setPicking(false);
        objectId = painter.pickObject(x, height() - 1 - y);
        painter.popSurface();
    }

    d->needsPick = false;
    return d->objects.value(objectId, 0);
}

/*!
  \internal
  If a QGLView is defined for this viewport then this function queues an update for that QGLView.

  If this is not defined then a normal update is called.
*/
void Viewport::update3d()
{
    if (d->view)
        d->view->update();
    else
        update();
}

/*!
    \internal
    The cameraChanged slot updates the camera in the QGLView if one exists, or simply calls the
    \l update() function otherwise.

    \sa update()
*/
void Viewport::cameraChanged()
{
    if (d->view)
        d->view->setCamera(d->camera);  // Calls update() internally.
    else
        update();
}

/*!
    \internal
*/
void Viewport::switchToOpenGL()
{
    // If there are multiple Viewport items in the QML, then it is
    // possible that another Viewport has already switched to QGLWidget.
    QList<QGraphicsView *> views = scene()->views();
    if (!views.isEmpty()) {
        QGLWidget *glw = qobject_cast<QGLWidget *>(views[0]->viewport());
        if (glw) {
            d->viewWidget = glw;
            return;
        }
    }

    QGraphicsView *view =
        qobject_cast<QGraphicsView *>(d->viewWidget->parentWidget());
    if (view) {
        d->viewWidget = new QGLWidget(view);
        view->setViewport(d->viewWidget);
    }
}

static inline void sendEnterEvent(QObject *object)
{
    QEvent event(QEvent::Enter);
    QCoreApplication::sendEvent(object, &event);
}

static inline void sendLeaveEvent(QObject *object)
{
    QEvent event(QEvent::Leave);
    QCoreApplication::sendEvent(object, &event);
}

/*!
    \internal
*/
void Viewport::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    QObject *object;
    if (!d->panning && d->picking)
        object = objectForPoint(e->pos());
    else
        object = 0;
    if (d->pressedObject) {
        // Send the press event to the pressed object.  Use a position
        // of (0, 0) if the mouse is still within the pressed object,
        // or (-1, -1) if the mouse is no longer within the pressed object.
        QMouseEvent event
            (QEvent::MouseButtonPress,
             (d->pressedObject == object) ? QPoint(0, 0) : QPoint(-1, -1),
             e->screenPos(), e->button(), e->buttons(), e->modifiers());
        QCoreApplication::sendEvent(d->pressedObject, &event);
    } else if (object) {
        // Record the object that was pressed and forward the event.
        d->pressedObject = object;
        d->enteredObject = 0;
        d->pressedButton = e->button();

        // Send a mouse press event for (0, 0).
        QMouseEvent event(QEvent::MouseButtonPress, QPoint(0, 0),
                          e->screenPos(), e->button(), e->buttons(),
                          e->modifiers());
        QCoreApplication::sendEvent(object, &event);
    } else if (d->navigation && e->button() == Qt::LeftButton) {
        d->panning = true;
        d->lastPan = d->startPan = e->pos();
        d->startEye = d->camera->eye();
        d->startCenter = d->camera->center();
        d->startUpVector = d->camera->upVector();
        d->panModifiers = e->modifiers();
#ifndef QT_NO_CURSOR
        setCursor(Qt::ClosedHandCursor);
#endif
    } else {
        QDeclarativeItem::mousePressEvent(e);
        return;
    }
    e->setAccepted(true);
}

/*!
    \internal
*/
void Viewport::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    if (d->panning && e->button() == Qt::LeftButton) {
        d->panning = false;
#ifndef QT_NO_CURSOR
        unsetCursor();
#endif
    }
    if (d->pressedObject) {
        // Notify the previously pressed object about the release.
        QObject *object = objectForPoint(e->pos());
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
                 e->screenPos(), e->button(), e->buttons(), e->modifiers());
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
                 e->screenPos(), e->button(), e->buttons(), e->modifiers());
            QCoreApplication::sendEvent(pressed, &event);
        }
        e->setAccepted(true);
    } else {
        QDeclarativeItem::mouseReleaseEvent(e);
    }
}

/*!
    \internal
*/
void Viewport::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e)
{
    if (d->picking) {
        QObject *object = objectForPoint(e->pos());
        if (object) {
            // Simulate a double click event for (0, 0).
            QMouseEvent event
                (QEvent::MouseButtonDblClick, QPoint(0, 0),
                 e->screenPos(), e->button(), e->buttons(), e->modifiers());
            QCoreApplication::sendEvent(object, &event);
            e->setAccepted(true);
            return;
        }
    }
    QDeclarativeItem::mouseDoubleClickEvent(e);
}

/*!
    \internal
*/
void Viewport::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
    if (d->panning) {
        QPointF delta = e->pos() - d->startPan;
        if (e->modifiers() == d->panModifiers) {
            d->camera->setEye(d->startEye);
            d->camera->setCenter(d->startCenter);
            d->camera->setUpVector(d->startUpVector);
        } else {
            d->startPan = d->lastPan;
            delta = e->pos() - d->startPan;
            d->startEye = d->camera->eye();
            d->startCenter = d->camera->center();
            d->startUpVector = d->camera->upVector();
            d->panModifiers = e->modifiers();
        }
        d->lastPan = e->pos();
        if ((e->modifiers() & Qt::ControlModifier) != 0)
            wheel(delta.y() * -60);
        else if ((e->modifiers() & Qt::ShiftModifier) != 0)
            pan(delta.x(), delta.y());
        else
            rotate(delta.x(), delta.y());
    } else if (d->picking) {
        QObject *object = objectForPoint(e->pos());
        if (d->pressedObject) {
            // Send the move event to the pressed object.  Use a position
            // of (0, 0) if the mouse is still within the pressed object,
            // or (-1, -1) if the mouse is no longer within the pressed object.
            QMouseEvent event
                (QEvent::MouseMove,
                 (d->pressedObject == object) ? QPoint(0, 0) : QPoint(-1, -1),
                 e->screenPos(), e->button(), e->buttons(), e->modifiers());
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
                 e->screenPos(), e->button(), e->buttons(), e->modifiers());
            QCoreApplication::sendEvent(object, &event);
        } else if (d->enteredObject) {
            sendLeaveEvent(d->enteredObject);
            d->enteredObject = 0;
        } else {
            QDeclarativeItem::mouseMoveEvent(e);
            return;
        }
    } else {
        QDeclarativeItem::mouseMoveEvent(e);
        return;
    }
    e->setAccepted(true);
}

/*!
    \internal
*/
void Viewport::hoverEnterEvent(QGraphicsSceneHoverEvent *e)
{
    if (hoverEvent(e))
        e->setAccepted(true);
    else
        QDeclarativeItem::hoverEnterEvent(e);
}

/*!
    \internal
*/
void Viewport::hoverMoveEvent(QGraphicsSceneHoverEvent *e)
{
    if (hoverEvent(e))
        e->setAccepted(true);
    else
        QDeclarativeItem::hoverMoveEvent(e);
}

/*!
    \internal
*/
void Viewport::hoverLeaveEvent(QGraphicsSceneHoverEvent *e)
{
    if (!d->pressedObject && d->enteredObject) {
        sendLeaveEvent(d->enteredObject);
        d->enteredObject = 0;
        e->setAccepted(true);
    } else {
        QDeclarativeItem::hoverLeaveEvent(e);
    }
}

/*!
    \internal
*/
void Viewport::wheelEvent(QGraphicsSceneWheelEvent *e)
{
    if (d->navigation) {
        wheel(e->delta());
        e->setAccepted(true);
    } else {
        QDeclarativeItem::wheelEvent(e);
    }
}

/*!
    \internal
*/
void Viewport::keyPressEvent(QKeyEvent *e)
{
    qreal sep;

    if (!d->navigation) {
        QDeclarativeItem::keyPressEvent(e);
        return;
    }

    switch (e->key()) {

    case Qt::Key_Left:
    {
        if ((e->modifiers() & Qt::ShiftModifier) != 0) {
            pan(-10, 0);
        } else if ((e->modifiers() & Qt::ControlModifier) != 0) {
            sep = d->camera->eyeSeparation();
            sep -= (sep / 10.0f);
            if (sep < 0.0f)
                sep = 0.0f;
            d->camera->setEyeSeparation(sep);
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
            sep = d->camera->eyeSeparation();
            sep += (sep / 10.0f);
            d->camera->setEyeSeparation(sep);
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

    default:
        QDeclarativeItem::keyPressEvent(e);
        return;
    }

    e->setAccepted(true);
}

/*!
    \internal
*/
bool Viewport::hoverEvent(QGraphicsSceneHoverEvent *e)
{
    if (!d->panning && d->picking) {
        QObject *object = objectForPoint(e->pos());
        if (d->pressedObject) {
            // Send the move event to the pressed object.  Use a position
            // of (0, 0) if the mouse is still within the pressed object,
            // or (-1, -1) if the mouse is no longer within the pressed object.
            QMouseEvent event
                (QEvent::MouseMove,
                 (d->pressedObject == object) ? QPoint(0, 0) : QPoint(-1, -1),
                 e->screenPos(), Qt::NoButton, Qt::NoButton, e->modifiers());
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
                 e->screenPos(), Qt::NoButton, Qt::NoButton, e->modifiers());
            QCoreApplication::sendEvent(object, &event);
        } else if (d->enteredObject) {
            sendLeaveEvent(d->enteredObject);
            d->enteredObject = 0;
        }
        return true;
    }
    return false;
}

// Zoom in and out according to the change in wheel delta.
void Viewport::wheel(qreal delta)
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
void Viewport::pan(qreal deltax, qreal deltay)
{
    QPointF delta = viewDelta(deltax, deltay);
    QVector3D t = d->camera->translation(delta.x(), -delta.y(), 0.0f);

    // Technically panning the eye left should make the object appear to
    // move off to the right, but this looks weird on-screen where the user
    // actually thinks they are picking up the object and dragging it rather
    // than moving the eye.  We therefore apply the inverse of the translation
    // to make it "look right".
    d->camera->setEye(d->camera->eye() - t);
    d->camera->setCenter(d->camera->center() - t);
}

// Rotate about the object being viewed.
void Viewport::rotate(qreal deltax, qreal deltay)
{
    QRectF rect = boundingRect();
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
    qreal anglex = deltax * 90.0f / rect.width();
    qreal angley = deltay * 90.0f / rect.height();
    QQuaternion q = d->camera->pan(-anglex);
    q *= d->camera->tilt(-angley);
    d->camera->rotateCenter(q);
}

// Convert deltas in the X and Y directions into percentages of
// the view width and height.
QPointF Viewport::viewDelta(qreal deltax, qreal deltay)
{
    QRectF rect = boundingRect();
    qreal w = rect.width();
    qreal h = rect.height();
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
        scaleX = scaleFactor * h / w;
        scaleY = scaleFactor;
    } else {
        scaleFactor = 2.0f / viewSize.height();
        scaleX = scaleFactor;
        scaleY = scaleFactor * w / h;
    }
    return QPointF(deltax * scaleX / w, deltay * scaleY / h);
}

QT_END_NAMESPACE
