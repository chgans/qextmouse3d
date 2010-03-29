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
#include "qml3dview.h"
#include "qgldepthbufferoptions.h"
#include "qglblendoptions.h"
#include "qgllightmodel.h"
#include "qgllightparameters.h"
#include "qglcamera.h"
#include <QtGui/qpainter.h>

/*!
    \class Viewport
    \brief The Viewport class defines the logical viewport for the 3d scene.  It includes all necessary
    references and parameters for the contents of the scene, as well as drawing and painting functions
    \since 4.6.q
    \ingroup qt3d
    \ingroup qt3d::qml3d

    \section1 Properties


    \section1 Usage in QML/3d

    \code
    \endcode
*/


QT_BEGIN_NAMESPACE

//QML_DEFINE_TYPE(Qt,4,6,Viewport,Viewport)
//QML_DEFINE_TYPE(Qt,4,6,LightModel,QGLLightModel)
//QML_DEFINE_TYPE(Qt,4,6,Light,QGLLightParameters)
//QML_DEFINE_TYPE(Qt,4,6,Camera,QGLCamera)


class ViewportPrivate
{
public:
    ViewportPrivate();

    bool picking;
    bool showPicking;
    bool navigation;
    bool itemsInitialized;
    QGLCamera *camera;
    QGLDepthBufferOptions depthBufferOptions;
    QGLBlendOptions blendOptions;
    QGLLightModel *lightModel;
    Effect *backdrop;
    //QGLVertexArray backdropVertices;
    QGLVertexBuffer backdropVertices;
    Qml3dView *view;
};

ViewportPrivate::ViewportPrivate()
    : picking(true)
    , showPicking(false)
    , navigation(true)
    , itemsInitialized(false)
    , camera(0)
    , lightModel(0)
    , backdrop(0)
    //, backdropVertices(QGL::Position, 2, QGL::TextureCoord0, 2)
    , backdropVertices()
    , view(0)
{
    depthBufferOptions.setEnabled(true);
    depthBufferOptions.setFunction(QGLDepthBufferOptions::Less);

    blendOptions.setEnabled(true);
    blendOptions.setSourceColorFactor(QGLBlendOptions::SrcAlpha);
    blendOptions.setSourceAlphaFactor(QGLBlendOptions::SrcAlpha);
    blendOptions.setDestinationColorFactor(QGLBlendOptions::OneMinusSrcAlpha);
    blendOptions.setDestinationAlphaFactor(QGLBlendOptions::OneMinusSrcAlpha);

    // Construct the vertices for a quad with (0, 0) as the
    // texture co-ordinate for the bottom-left of the screen
    // and (1, 1) as the texture co-ordinate for the top-right.
    backdropVertices.setPackingHint(QGLVertexBuffer::Append);

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

/*!
    Construct the class and assign it a \a parent QDeclarativeItem.
*/
Viewport::Viewport(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
{
    d = new ViewportPrivate();
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

/*!
    Class destruction and cleanup.
*/
Viewport::~Viewport()
{
    delete d;
}

/*!
    \property Viewport::picking
    \brief User interaction in QML/3d is handled through the concept of object picking.  Each
    item has a unique picking id which s queried for a given screen click position when the
    mouse is clicked.

    If the \a picking property is set to \c true, picking will be supported for this viewport,
    while if the property is \c false, no picking will b applied.

    The default value for this property is \c false.

    \sa showPicking()
*/
bool Viewport::picking() const
{
    return d->picking;
}

void Viewport::setPicking(bool value)
{
    d->picking = value;
    update3d();
}

/*!
    \property Viewport::showPicking
    \brief The underlying mechanism for picking is based on painting an off-screen buffer with a flat
    coloured image containing all of the objects with a unique color value.

    Setting the \a showPicking value to true will display this flat-colour picking representation
    in the viewport.

    The default value for this property is \c false.

    \sa picking()
*/

bool Viewport::showPicking() const
{
    return d->showPicking;
}

void Viewport::setShowPicking(bool value)
{
    d->showPicking = value;
    update3d();
}

/*!
    \property Viewport::navigation
    \brief The navigation property is used to set or unset camera navigation in for the viewport.
    Camera navigation allows the user to move the camera position around using the moose.

    By default, camera navigation is set to \c true.
*/
bool Viewport::navigation() const
{
    return d->navigation;
}

void Viewport::setNavigation(bool value)
{
    d->navigation = value;
    update3d();
}

/*!
    \property Viewport::camera
    \brief The camera property of the Viewport class sets the camera parameters which will be used
    for the appropriate viewing transforms in OpenGL.  The camera is specified as a \l QGLCamera,
    and like all properties in QML/3d it can be updated or changed at any time.

    By default the camera is undefined.

    \sa QGLCamera
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
    \property Viewport::lightModel
    \brief The user is able to set a lighting model for the 3d environment through the use of the
    lightModel property.  This is defined using a \l QGLLightModel.

    By default the light model is undefined.

    \sa QGLLightModel
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
        update3d();
    }
}

/*!
    \property Viewport::backdrop
    \brief The user can set the backdrop property of the Viewport class to define a backdrop effect
    for the 3d environment.  This backdrop is defined as an \l Effect class, and can take on any of
    the effects supported (eg. fog effects, etc).

    By default no backdrop effect is defined.

    \sa Effect
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
        }
        update3d();
    }
}

/*!
    The main paint function for the Viewport class.  It takes a  QPainter \a p, which performs the
    painting of objects in the 3d environment.

    The paint function is responsible for setting up the viewing transform, as well as other display
    options, before calling the draw function to perform the actual drawing of the scene.

    Note, currently \a style and \a widget are unused, but are reserved for later development.
*/
void Viewport::paint(QPainter *p, const QStyleOptionGraphicsItem * style, QWidget *widget)
{
    Q_UNUSED(style);
    Q_UNUSED(widget);
    QGLPainter painter;
    if (!painter.begin(p)) {
        qDebug("GL graphics system is not active; cannot use 3D items");
        return;
    }

    // Initialize the objects in the scene if this is the first paint.
    if (!d->itemsInitialized)
        initialize(0, &painter);

    // Modify the GL viewport to only cover the extent of this QDeclarativeItem.
    QTransform transform = p->transform();
    painter.setViewport(QRect(qRound(transform.dx()), qRound(transform.dy()),
                              width(), height()));

    // Perform early drawing operations.
    earlyDraw(&painter);

    // Set up the scene the way Qml3dView would if we were using it.
    d->depthBufferOptions.apply(&painter);
    d->blendOptions.apply(&painter);
    painter.setCullFaces(QGL::CullDisabled);
    if (d->camera) {
        painter.setCamera(d->camera);
    } else {
        QGLCamera defCamera;
        painter.setCamera(&defCamera);
    }

    // Draw the Item3d children.
    draw(&painter);
}

/*!
  Some elements of the image are drawn as an "early" draw activity.  Essentially it performs clearing
  and drawing of backdrop effects in preparation for primary drawing activities using \a painter.
*/
void Viewport::earlyDraw(QGLPainter *painter)
{
    // If we have a QDeclarativeItem parent, then assume that it has cleared
    // the screen (e.g. Rect::color), and just clear the depth buffer.
    if (parentItem())
        painter->clear(QGL::ClearDepthBuffer);
    else
        painter->clear();

    // If we have a scene backdrop, then draw it now.
    if (d->backdrop) {
        painter->projectionMatrix().setToIdentity();
        painter->modelViewMatrix().setToIdentity();
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDepthMask(GL_FALSE);

        // Select the effect and draw the backdrop quad.
        d->backdrop->enableEffect(painter);
        //painter->setVertexArray(d->backdropVertices);
        painter->setVertexBuffer(d->backdropVertices);
        painter->draw(QGL::TriangleFan, 4);
        d->backdrop->disableEffect(painter);

        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
    }

#ifdef GL_RESCALE_NORMAL
    glEnable(GL_RESCALE_NORMAL);
#endif
}

/*!
  The draw function for the viewport sets up all of the lighting parameters for the scene before
  iterating through the top level items in the scene and drawing them using \a painter.

  As a matter of course each item will draw its child items, and so on, until the entire image has
  been populated.
*/
void Viewport::draw(QGLPainter *painter)
{
    painter->setObjectPickId(-1);
    QObjectList list = QObject::children();    
    int lightNumber = 0;
    foreach (QObject *child, list) {
        QGLLightParameters *light = qobject_cast<QGLLightParameters *>(child);
        if (light) {
            painter->setLightParameters(lightNumber, light); // XXX - non-zero lights
            painter->setLightEnabled(lightNumber, true);
            lightNumber++;
        }
    }
    painter->setLightModel(d->lightModel);
    foreach (QObject *child, list) {
        Item3d *item = qobject_cast<Item3d *>(child);
        if (item)
            item->draw(painter);
    }
    
    for (int i=0; i<lightNumber; i++)
    {
        painter->setLightParameters(i,0);
        painter->setLightEnabled(i, 0);
    }
}

/*!
  The initialize function, as its name suggests, peforms all top level initialisation for the viewport and
  sets the \a painter for the viewport.

  This includes setting up the camera, as well as initialising all of the

  If \a view is null, then we are running on a standard QFxView canvas widget.  If \a view is not null,
  then we are running on a \l Qml3dView canvas widget.

  \sa Qml3dView
*/

void Viewport::initialize(Qml3dView *view, QGLPainter *painter)
{
    // Remember which view we are associated with, if any.
    d->view = view;

    // Apply the camera to the view.
    if (view && d->camera)
        view->setCamera(d->camera);

    // Initialize the Item3d objects attached to this scene.
    QObjectList list = QObject::children();
    foreach (QObject *child, list) {
        Item3d *item = qobject_cast<Item3d *>(child);
        if (item)
            item->initialize(this, painter);
    }
    d->itemsInitialized = true;
}

/*!
  Return the \l Qml3dView being used by the viewport.

  \sa Qml3dView, initialize()
*/
Qml3dView *Viewport::view() const
{
    return d->view;
}

/*!
  If a Qml3dView is defined for this viewport then this function queues an update for that Qml3dView.

  If this is not defined then a normal update is called.

  \sa Qml3dView
*/
void Viewport::update3d()
{
    if (d->view)
        d->view->queueUpdate();
    else
        update();
}

/*!
    The cameraChanged slot updates the camera in the Qml3dView if one exists, or simply calls the
    \l update() function otherwise.

    \sa update()
*/
void Viewport::cameraChanged()
{
    if (d->view)
        d->view->setCamera(d->camera);  // Calls queueUpdate() internally.
    else
        update();
}

QT_END_NAMESPACE
