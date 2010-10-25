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
#include "mesh.h"
#include "effect.h"
#include "qgllightparameters.h"
#include "qglabstractscene.h"
#include "qglscenenode.h"
#include "qglview.h"
#include "qgraphicstransform3d.h"
#include <QtGui/qevent.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/private/qdeclarativestategroup_p.h>
#include <QtDeclarative/private/qdeclarativeitem_p.h>

/*!
    \qmlclass Item3D Item3D
    \brief The Item3D item encapsulates 3D objects and contains all of the properties and methods needed for simple 3D operations.
    part of a QML/3d script.
    \since 4.8
    \ingroup qt3d::qml3d

    \section1 Simple 3D Object Definition

    The most basic use case for the Item3D class is the creation and display of a 
    single simple item in the 3D environment.

    Many such items within a 3D environment are defined as a single logical component
    mesh which is treated as a stand-alone object for the purposes of rotation, 
    scaling, and user interaction via "picking".

    Such an object can easily be defined in QML using the following code:

    \code
    Item3D {
        id: teapot
        mesh: Mesh { source: "teapot.bez" }
        effect: Effect {}
		cullFaces: "CullBackFaces"
    }
    \endcode

    This simple code will create a 3D item based on the \i teapot.bez mesh using 
    back-face culling.

    Notice that in this case the effect and mesh are defined within the body of 
    the item definition.  Where there is little complexity, this method of
    defining items simplifies the resultant code and makes modification of
    the QML easier.

    \section1 Embedding Item3D Objects

    Consider the following:

    \code
    Item3D {
        id: cup
        mesh:  Mesh { source: "teacup.bez" }
		effect: Effect {}
		cullFaces: "CullBackFaces"

        Item3D {
            id: saucer
            mesh: Mesh { source: "saucer.bez" }
            cullFaces: "CullBackFaces"
        }

        position: Qt.vector3d(10, -10, -10)
    }
    \endcode

    This demonstrates the capability of embedding one Item3D within another.  In
    this case the \i saucer item is a child of the \i cup item.

    All transformations applied to the parent item are also applied to the child, 
    so in this case both the cup and saucer will be translated based on the 
    position vector defined in the cup item's definition.

    In this case any additional transformations applied to the child item will not 
    affect the parent, and are local only to that item (and to its children if any
    exist).

    This allows a user to group together items logically so that transformations and
    user interactions can be applied to groups of objects as if they were a whole.

    \section1 Using Sub-nodes of 3D Objects

    In more complex applications the user may wish to load a complex mesh
    which is made up of a number of components or nodes which may be organised
    into a tree like structure.  In this case they may wish to interact with, 
    animate, or otherwise modify individual sub-nodes of a mesh.

    Item3D leverages the existing \bold {Qt Object Model} in order to allow QML/3d users
    this type of control over their 3D items.

    Consider the following QML script:

    \code
    Item3D {
        id: helicoptor
        mesh:  helicoptorMesh
		effect: Effect {}
		cullFaces: "CullBackFaces"

        transform: [
			Rotation3D {
				id: rotate1
				angle: 5
				axis: Qt.vector3d(1, 0, 0)
			},
			Rotation3D {
				id: rotate2
				angle: 5
				axis: Qt.vector3d(0, 1, 0)
			},
			Rotation3D {
				id: rotate3
				angle: 45
				axis: Qt.vector3d(0, 0, 1)
			}
		]

        Item3D {
			id: rotor
			property bool spin: false				
			meshNode: "rotorBladesNode"
			Item3D {meshNode: "rotorHubNode"}
            			
			transform: [
				Rotation3D {
					id: rotateBlades
					angle: 0
					axis: Qt.vector3d(0, 0, 1)
				}
			]
			
			onClicked: { rotor.spin=true }
			
            SequentialAnimation {
                running: rotor.spin
			    NumberAnimation {target: rotateBlades; property: "angle"; to : 360.0; duration: 750; easing.type:"OutQuad" }	
                onCompleted: rotor.spin = false
			}
		}

    }
        
    Mesh {
        id: helicoptorMesh
        source: "bellUH1.3ds"
    }
    \endcode

    Obviously this example is much more complex both in structure and behaviour. In 
    this case the mesh describes a \i .3ds file of a helicoptor, which is broken down 
    discrete sub-components (engine nacelles, rotor, rotor hub, etc), which the user
    may wish to modify or animate individually.

    Each child item in this case does not have a mesh explicitly defined, but rather 
    inherits the mesh from the parent.  However each child item does define a mesh node 
    which is part of the parent mesh.

    All transformations carried out on the parent item will also be applied to the child.

    Child items can, as shown here, have their own \i local transformations and user
    interactions applied.  These will be applied only to the node of the mesh which
    is defined for that item.  In cases where the mesh is defined heirarchically as a
    tree of nodes, this transformation will therefore be applied to all items in that
    tree which are children of the defined node.

    Likewise if the user explicitly declares a child item, such as has been done here with 
    the \i rotorHubNode, then the transformations will apply to this item as well (and 
    its children, and so on).

    It should be noted that no support is currently provided for skeleton animation or
    kinematic control of items.  This is left to the user to implement as required.
*/



QT_BEGIN_NAMESPACE

class Item3DPrivate
{
public:
    Item3DPrivate(Item3D *_item)
        : item(_item)
        , viewport(0)
        , position(0.0f, 0.0f, 0.0f)
        , pivot(0.0f,0.0f,0.0f)
        , usePivot(false)
        , scale(1.0f)
        , mesh(0)
        , effect(0)
        , light(0)
        , objectPickId(-1)
        , cullFaces(Item3D::CullDisabled)
        , _stateGroup(0)
        , inheritEvents(false)
        , isVisible(true)
        , isInitialized(false)
    {
    }

    Item3D *item;
    Viewport *viewport;
    QVector3D position;
    QVector3D pivot;
    bool usePivot;
    qreal scale;
    Mesh *mesh;
    Effect *effect;    
    QGLLightParameters *light;
    int objectPickId;
    Item3D::CullFaces cullFaces;
    QDeclarativeStateGroup *states();
    QDeclarativeStateGroup *_stateGroup;

    bool inheritEvents;
    bool isVisible;
    bool isInitialized;
    int mainBranchId;
    QString name;
    QString meshNode;

    // data property
    static void data_append(QDeclarativeListProperty<QObject> *, QObject *);
    
    // resources property
    static QObject *resources_at(QDeclarativeListProperty<QObject> *, int);
    static void resources_append(QDeclarativeListProperty<QObject> *, QObject *);
    static int resources_count(QDeclarativeListProperty<QObject> *);

    //// children property
    static Item3D *children_at(QDeclarativeListProperty<Item3D> *, int);
    static void children_append(QDeclarativeListProperty<Item3D> *, Item3D *);
    static int children_count(QDeclarativeListProperty<Item3D> *);

    // transform property
    static int transform_count(QDeclarativeListProperty<QGraphicsTransform3D> *list);
    static void transform_append(QDeclarativeListProperty<QGraphicsTransform3D> *list, QGraphicsTransform3D *);
    static QGraphicsTransform3D *transform_at(QDeclarativeListProperty<QGraphicsTransform3D> *list, int);
    static void transform_clear(QDeclarativeListProperty<QGraphicsTransform3D> *list);
    QList<QGraphicsTransform3D *> transforms;


    // pretransform property
    static int pretransform_count(QDeclarativeListProperty<QGraphicsTransform3D> *list);
    static void pretransform_append(QDeclarativeListProperty<QGraphicsTransform3D> *list, QGraphicsTransform3D *);
    static QGraphicsTransform3D *pretransform_at(QDeclarativeListProperty<QGraphicsTransform3D> *list, int);
    static void pretransform_clear(QDeclarativeListProperty<QGraphicsTransform3D> *list);
    QList<QGraphicsTransform3D *> pretransforms;
};


int Item3DPrivate::transform_count(QDeclarativeListProperty<QGraphicsTransform3D> *list)
{  
    Item3D *object = qobject_cast<Item3D *>(list->object);
    if (object) {
        return object->d->transforms.count();
    } else {
        qWarning()<<"Warning: could not find Item3D to query for transformation count.";
        return 0;
    }
}

void Item3DPrivate::transform_append(QDeclarativeListProperty<QGraphicsTransform3D> *list, QGraphicsTransform3D *item)
{
    Item3D *object = qobject_cast<Item3D *>(list->object);
    QList<QGraphicsTransform3D *> *ptrans;
    if (object)
    {
        ptrans = &object->d->transforms;

        //We now need to connect the underlying transform so that any change will update the graphical item.
        if (!ptrans->contains(item)) {
            ptrans->append(item);            
            QObject::connect(item, SIGNAL(transformChanged()),
                             object, SLOT(update()));
        }
    }
    else
        qWarning()<<"Warning: could not find Item3D to add transformation to.";
}

QGraphicsTransform3D *Item3DPrivate::transform_at(QDeclarativeListProperty<QGraphicsTransform3D> *list, int idx)
{    
   Item3D *object = qobject_cast<Item3D *>(list->object);
    if (object) {
        return object->d->transforms.at(idx);
    } else {
        qWarning()<<"Warning: could not find Item3D to query for transformations";
        return 0;
    }
    return 0;
}

void Item3DPrivate::transform_clear(QDeclarativeListProperty<QGraphicsTransform3D> *list)
{

    Item3D *object = qobject_cast<Item3D *>(list->object);
    if (object) {
        object->d->transforms.clear();
        object->update();
    }
    else
        qWarning()<<"Warning: could not find Item3D to clear of transformations";
}

int Item3DPrivate::pretransform_count(QDeclarativeListProperty<QGraphicsTransform3D> *list)
{  
    Item3D *object = qobject_cast<Item3D *>(list->object);
    if (object) {
        return object->d->pretransforms.count();
    } else {
        qWarning()<<"Warning: could not find Item3D to query for transformation count.";
        return 0;
    }
}

void Item3DPrivate::pretransform_append(QDeclarativeListProperty<QGraphicsTransform3D> *list, QGraphicsTransform3D *item)
{
    Item3D *object = qobject_cast<Item3D *>(list->object);
    QList<QGraphicsTransform3D *> *ptrans;
    if (object)
    {
        ptrans = &object->d->pretransforms;

        //We now need to connect the underlying transform so that any change will update the graphical item.
        if (!ptrans->contains(item)) {
            ptrans->append(item);            
            QObject::connect(item, SIGNAL(transformChanged()),
                             object, SLOT(update()));
        }
    }
    else
        qWarning()<<"Warning: could not find Item3D to add transformation to.";
}

QGraphicsTransform3D *Item3DPrivate::pretransform_at(QDeclarativeListProperty<QGraphicsTransform3D> *list, int idx)
{    
   Item3D *object = qobject_cast<Item3D *>(list->object);
    if (object) {
        return object->d->pretransforms.at(idx);
    } else {
        qWarning()<<"Warning: could not find Item3D to query for transformations";
        return 0;
    }
    return 0;
}

void Item3DPrivate::pretransform_clear(QDeclarativeListProperty<QGraphicsTransform3D> *list)
{

    Item3D *object = qobject_cast<Item3D *>(list->object);
    if (object) {
        object->d->pretransforms.clear();
        object->update();
    }
    else
        qWarning()<<"Warning: could not find Item3D to clear of transformations";
}



void Item3DPrivate::data_append(QDeclarativeListProperty<QObject> *prop, QObject *o)
{
    Item3D *i = qobject_cast<Item3D *>(o);
    if (i) 
        i->setParent(static_cast<Item3D *>(prop->object));
    else
        o->setParent(static_cast<Item3D *>(prop->object));
}


QObject *Item3DPrivate::resources_at(QDeclarativeListProperty<QObject> *prop, int index)
{
    QObjectList children = prop->object->children();
    if (index < children.count())
        return children.at(index);
    else
        return 0;
}

void Item3DPrivate::resources_append(QDeclarativeListProperty<QObject> *prop, QObject *o)
{
    o->setParent(prop->object);
}

int Item3DPrivate::resources_count(QDeclarativeListProperty<QObject> *prop)
{
    return prop->object->children().count();
}

Item3D *Item3DPrivate::children_at(QDeclarativeListProperty<Item3D> *prop, int index)
{
    QObjectList thechildren = static_cast<Item3D*>(prop->object)->children();

    if (index < thechildren.count())
        return qobject_cast<Item3D *>(thechildren.at(index));
    else
        return 0;
}

void Item3DPrivate::children_append(QDeclarativeListProperty<Item3D> *prop, Item3D *i)
{
    if (i)
    {
        i->setParent(static_cast<QObject*>(prop->object));

        //Because this is now a static function, we can no longer apply the following :-(
        //however it is still carried out in the "initialize" function of Item3D.
        /*
        if (inheritEvents) {
            QObject::connect(i, SIGNAL(clicked()), item, SIGNAL(clicked()));
            QObject::connect(i, SIGNAL(doubleClicked()), item, SIGNAL(doubleClicked()));
            QObject::connect(i, SIGNAL(pressed()), item, SIGNAL(pressed()));
            QObject::connect(i, SIGNAL(released()), item, SIGNAL(released()));
            QObject::connect(i, SIGNAL(hoverEnter()), item, SIGNAL(hoverEnter()));
            QObject::connect(i, SIGNAL(hoverLeave()), item, SIGNAL(hoverLeave()));
        }
        */
    }
}

int Item3DPrivate::children_count(QDeclarativeListProperty<Item3D> *prop)
{
    return static_cast<Item3D*>(prop->object)->children().count();
}


QDeclarativeStateGroup *Item3DPrivate::states()
{
    if (!_stateGroup) {
        _stateGroup = new QDeclarativeStateGroup(item);
        QObject::connect(_stateGroup, SIGNAL(stateChanged(QString)),
                         item, SIGNAL(stateChanged(QString)));
    }

    return _stateGroup;
}


/*!
    \internal
    Constructs an \l Item3D with the default properties and
    attaches it to \a parent.
*/
Item3D::Item3D(QObject *parent)
    : QObject(parent)
{
    d = new Item3DPrivate(this);
}

/*!
    \internal
    Destroys this \l Item3D object.
*/
Item3D::~Item3D()
{
    delete d;
}



/*!
    \qmlproperty vector3D Item3D::position

    The position in 3D space of this item.  The default value for this
    property is (0, 0, 0).

    \sa x, y, z
*/

QVector3D Item3D::position() const
{
    return d->position;
}

void Item3D::setPosition(const QVector3D& value)
{
    d->position = value;
    emit positionChanged();
    update();
}

/*!
    \qmlproperty real Item3D::x

    The x position of this item in 3D space.  The default value for this
    property is 0.

    \sa position, y, z
*/

qreal Item3D::x() const
{
    return d->position.x();
}

void Item3D::setX(qreal value)
{
    d->position.setX(value);
    emit positionChanged();
    update();
}

/*!
    \qmlproperty real Item3D::y

    The y position of this item in 3D space.  The default value for this
    property is 0.

    \sa position, x, z
*/

qreal Item3D::y() const
{
    return d->position.y();
}

void Item3D::setY(qreal value)
{
    d->position.setY(value);
    emit positionChanged();
    update();
}

/*!
    \qmlproperty real Item3D::z

    The z position of this item in 3D space.  The default value for
    this property is 0.

    \sa position, x, y
*/

qreal Item3D::z() const
{
    return d->position.z();
}

void Item3D::setZ(qreal value)
{
    d->position.setZ(value);
    emit positionChanged();
    update();
}

/*!
    \qmlproperty real Item3D::scale

    The scaling factor to apply to the item after the transformations
    from the Item3D::transform property.  The default value for this
    property is 1.

    \sa transform
*/

qreal Item3D::scale() const
{
    return d->scale;
}

void Item3D::setScale(qreal value)
{
    d->scale = value;
    emit scaleChanged();
    update();
}

/*!
    \qmlproperty list<Transform> Item3D::transform

    Generally objects in 3D space will have undergone some number
    of 3D transformation prior to display.  Examples of such transformations
    include rotations about the x, y, and z axes, translation, and so on.

    Each Item3D maintains a list of transforms to apply to it through this 
    property.  In scripting terms a transform can be applied as follows:

    \code
    Item3D {
        id: teapot
        mesh: Mesh { source: "teapot.bez" }
        transform: [
            Rotation3D {
                id: teapot_rotate1
                angle: 0
                axis: Qt.vector3d(0, 1, 0)
            },
            Rotation3D {
                id: teapot_rotate2
                angle: 0
                axis: Qt.vector3d(0, 0, 1)
            }
        ]
    }   
    \endcode

    In this example we have two transformations in our list - a rotation around
    the y axis (\c {teapot_rotate1}), and a rotation about the z axis (\c {teapot_rotate2}).

    These transformations can be accessed via standard QML scripting methods to achieve
    animations and other effects.

    By default this list of transformations is empty.

    \sa Rotation3D, Scale3D, Translation3D, scale, position, pretransform
*/


QDeclarativeListProperty<QGraphicsTransform3D> Item3D::transform()
{
    return QDeclarativeListProperty<QGraphicsTransform3D>(this, 0, d->transform_append, d->transform_count,
                                               d->transform_at, d->transform_clear);
}

/*!
    \qmlproperty list<Transform> Item3D::pretransform

    The transformations to apply before all others.

    When a model is loaded from an external source such as a 3D
    modeling package, it is usually in an unconventional orientation
    and position.  The first step is to rotate, scale, and translate
    it to make it suitable for use as a QML object.

    The purpose of the \c pretransform property is to perform such
    "model correction" transformations before \c scale, \c transform,
    and \c position are applied to place the model in its final
    orientation and position in the QML application.

    By default this list of transformations is empty.

    \sa transform, scale, position
*/

QDeclarativeListProperty<QGraphicsTransform3D> Item3D::pretransform()
{
    return QDeclarativeListProperty<QGraphicsTransform3D>(this, 0, d->pretransform_append, d->pretransform_count,
                                               d->pretransform_at, d->pretransform_clear);
}

/*!
    \qmlproperty bool Item3D::inheritEvents

    Users are able to interact with 3d items in a scene through (for example) the
    use of the mouse.  These, and other, Qt events can be captured by an \l Item3D using the
    same underlying QObject architecture shared by all of Qt.

    Often a user will only want an item to capture mouse events for itself, leaving
    child items to handle their mouse events locally.  Under many circumstances, however, it 
    is necessary for a parent object to collect all mouse events for itself and its child 
    items.  Usually this inheritance of events is only defined at initialisation for an \l Item3D
    
    The inheritEvents property, however, is a simple boolean property which provides a mechanism
    for both initialisation time and programmatic modification of this.

    Setting the property to true connects the signals for all child items to the appropriate
    signals for the item itself.  Conversely setting the property to false disconnects the 
    events.

    The default value for this property is false.
*/
bool Item3D::inheritEvents() const
{
    return d->inheritEvents;
}

void Item3D::setInheritEvents(bool inherit)
{
    d->inheritEvents = inherit;

    //Generally we would only want to 
    QObjectList list = QObject::children();
    if (inherit)
    {
        foreach (QObject *child, list) {
		    Item3D *subItem =qobject_cast<Item3D *>(child);	
            if (subItem)
            {   
                // Proxy the mouse event signals to the parent so that
                // the parent can trap the signal for a group of children.
                QObject::connect(subItem, SIGNAL(clicked()), this, SIGNAL(clicked()));
                QObject::connect(subItem, SIGNAL(doubleClicked()), this, SIGNAL(doubleClicked()));
                QObject::connect(subItem, SIGNAL(pressed()), this, SIGNAL(pressed()));
                QObject::connect(subItem, SIGNAL(released()), this, SIGNAL(released()));
                QObject::connect(subItem, SIGNAL(hoverEnter()), this, SIGNAL(hoverEnter()));
                QObject::connect(subItem, SIGNAL(hoverLeave()), this, SIGNAL(hoverLeave()));
            }   
        }
    }
    else
    {
        foreach (QObject *child, list) {
		    Item3D *subItem =qobject_cast<Item3D *>(child);	
            if (subItem)
            {   
                // Proxy the mouse event signals to the parent so that
                // the parent can trap the signal for a group of children.
                QObject::disconnect(subItem, SIGNAL(clicked()), this, SIGNAL(clicked()));
                QObject::disconnect(subItem, SIGNAL(doubleClicked()), this, SIGNAL(doubleClicked()));
                QObject::disconnect(subItem, SIGNAL(pressed()), this, SIGNAL(pressed()));
                QObject::disconnect(subItem, SIGNAL(released()), this, SIGNAL(released()));
                QObject::disconnect(subItem, SIGNAL(hoverEnter()), this, SIGNAL(hoverEnter()));
                QObject::disconnect(subItem, SIGNAL(hoverLeave()), this, SIGNAL(hoverLeave()));
            }  
        }
    }
}

/*!
    \qmlproperty Mesh Item3D::mesh

    Objects in most 3D environments are almost invariably defined as meshes - sets of 
    vertices which when linked as polygons form a recognisable 3D object.  Qt3d currently 
    supports a number of these \i {scene formats}, including \i {.obj} file, bezier patches 
    \i {(.bez)}, and \i {.3ds} files.

    These meshes are abstracted into the \l Mesh class, which is defined for
    an \l Item3D through this property.    

    The default value for this property is null, so a mesh must be defined in
    order for the item to be displayed

    \sa effect
*/

Mesh *Item3D::mesh() const
{
    return d->mesh;
}

void Item3D::setMesh(Mesh *value)
{
    if (d->mesh) {
        if (!d->mesh->deref())
            delete d->mesh;
    }

    d->mesh = value;
    //always start off pointing to the default scene mesh object.
    d->mainBranchId = 0;  

    if (value) {
        d->mesh->ref();
        connect(value, SIGNAL(dataChanged()), this, SIGNAL(meshChanged()));
        connect(value, SIGNAL(dataChanged()), this, SLOT(update()));
    }

    emit meshChanged();
    
    update();
}

/*!
    \qmlproperty Effect Item3D::effect

    QML 3D items support the use of effects for modifying the display
    of items - texture effects, fog effects, material effects, and so on.

    The exact effects correlated with an item are set using this property.

    The default value for this propertly is null, and so an effect - even an
    empty one - must be defined if the mesh does not contain its own effects.

    \sa Effect, mesh
*/
Effect *Item3D::effect() const
{
    return d->effect;
}

void Item3D::setEffect(Effect *value)
{
    if (d->effect == value)
        return;
    if (d->effect)
        disconnect(d->effect, SIGNAL(effectChanged()), this, SLOT(update()));
    d->effect = value;
    if (d->effect)
        connect(d->effect, SIGNAL(effectChanged()), this, SLOT(update()));
    emit effectChanged();
    update();
}

/*!
    \qmlproperty Light Item3D::light

    This property defines an item-specific light that will be used
    intead of Viewport::light for rendering this item and its children
    if the value is not null.

    \sa Viewport::light
*/

QGLLightParameters *Item3D::light() const
{
    return d->light;
}

void Item3D::setLight(QGLLightParameters *value)
{
    if (d->light != value) {
        if (d->light) {
            disconnect(d->light, SIGNAL(lightChanged()),
                       this, SLOT(update()));
        }
        d->light = value;
        if (d->light) {
            connect(d->light, SIGNAL(lightChanged()),
                    this, SLOT(update()));
        }
        emit lightChanged();
        update();
    }
}

/*!
    \qmlproperty list<Item3D> Item3D::children
    \qmlproperty list<Object> Item3D::resources

    The children property contains a list of all 3D child items for
    this item.  This provides logical grouping of items in a 3D scene.
    Transformations that are applied to this item will also affect
    child items.

    The resources property holds all other children that do not
    directly inherit from Item3D, such as effects, meshes, and
    other supporting objects.

    Normally it isn't necessary to assign to the children or resources
    properties directly as the QML syntax will take care of the
    assignment depending upon the object's type.

    \sa transform
*/
QDeclarativeListProperty<Item3D> Item3D::fxChildren()
{
    return QDeclarativeListProperty<Item3D>(this, 0, Item3DPrivate::children_append,
                                                     Item3DPrivate::children_count, 
                                                     Item3DPrivate::children_at); 
}

QDeclarativeListProperty<QObject> Item3D::resources()
{
    return QDeclarativeListProperty<QObject>(this, 0, Item3DPrivate::resources_append, 
                                             Item3DPrivate::resources_count, 
                                             Item3DPrivate::resources_at); 
}



/*!
    \qmlproperty list<Object> Item3D::data

    This property exists to allow future expansion of the Item3D class to
    include additional data and resources.  Currently there is no underlying
    implementation for this.
*/
QDeclarativeListProperty<QObject> Item3D::data() 
{
    return QDeclarativeListProperty<QObject>(this, 0, Item3DPrivate::data_append);
}

/*!
    \qmlproperty list<State> Item3D::states

    QML allows users to define any number of states for objects, including \l Item3D objects.
    These states are arbitrarily assigned and can be used to represent anything the user desires.
    An \l Item3D representing a door, for example, may have an \i "open" and \i "closed" state.

    States can be connected by transitions, which define an animation or other transitional change
    which occurs during the change from one state to another.  In our open and closed door state,
    for example, the transition may define the rotation of the opening door when it swings open and
    swings back closed.

    By default the list of valid states for the item is empty.

    \sa state, transitions
*/
QDeclarativeListProperty<QDeclarativeState> Item3D::states()
{    
    return d->states()->statesProperty();
}

/*!
    \internal
    Occasionally it is necessary to find a given state based on its \a name as expressed by a 
    string.  This function allows users to do so by providing a QString name, and returning
    a corresponding QDeclarativeState.
*/
QDeclarativeState *Item3D::findState(const QString &name) const
{
    if (!d->_stateGroup)
        return 0;
    else
        return d->_stateGroup->findState(name);
}

/*!
    \qmlproperty list<Transition> Item3D::transitions

    When an object in QML moves from one state to another its behavior during this 
    change can be defined using transitions.  These transitions may define changes to one or
    more objects or properties, and may include animations such as rotations, scaling, 
    translation, and so on.

    Users can define a set of transitional procedures using this property.
    
    As with states, by default there are no transitions defined.
    
    \sa state, states
    
*/
QDeclarativeListProperty<QDeclarativeTransition> Item3D::transitions()
{    
    return d->states()->transitionsProperty();
}


/*!
    \qmlproperty string Item3D::state

    This property describes the current state of the \l Item3D as defined in the list of
    allowable states for this item.

    By default an item's state is undefined and irrelevant.  It is only when the states property 
    has been modified that a current state makes any sense.

    \sa states
*/

QString Item3D::state() const
{
    if (!d->_stateGroup)
        return QString();
    else
        return d->_stateGroup->state();
}

void Item3D::setState(const QString &state)
{
    d->states()->setState(state);
}


/*!
    \qmlproperty enumeration Item3D::cullFaces

    This property defines the culling method to be use on fragments
    within the item's mesh.  Culling of an item in 3D space can be
    carried out in a number of ways:

    \list
    \o CullDisabled Do not use culling.  This is the default value.
    \o CullFrontFaces Cull the front faces of the object.
    \o CullBackFaces Cull the back faces of the object.
    \o CullAllFaces Cull all faces of the object.
    \o CullClockwise Cull faces based on clockwise winding of vertices.
    \endlist
*/
Item3D::CullFaces Item3D::cullFaces() const
{
    return d->cullFaces;
}

void Item3D::setCullFaces(Item3D::CullFaces value)
{
    if (d->cullFaces != value) {
        d->cullFaces = value;
        emit meshChanged();
    }
}

/*!
    \qmlproperty string Item3D::name

    A simple string name for the object.  This is useful for debugging, but also as a
    meaningful way of describing items in a manner which can be easily displayed/printed.
*/
QString Item3D::name() const
{
    return d->name;
}

void Item3D::setName(QString nameString)
{
    d->name = nameString;
    if (objectName().isEmpty())
        setObjectName(nameString);
}

/*!
    \internal
    This function allows the user to get a scene object from the underlying mesh which forms
    an \l Item3D.  

    By passing the \a type of node (defined as a \l QGLSceneNode::Type enumeration), along
    with the \a name of the scene object, the user can access the corresponding \l QGLSceneNode
    for that name.

    Failure to find the scene object will return \i null.
*/
QGLSceneNode *Item3D::getSceneObject(QGLSceneNode::Type type, const QString& name) const
{
    return mesh()->getSceneObject(type, name);
}

/*!
    \internal
    Performs the actual drawing of the Item3D using \a painter.

    If the item is set to object picking mode this includes all of the infrastructure needed
    to support picking of objects.

    The basic premise of the draw function should be familiar to users of OpenGL or similar
    graphics libraries.  Essentially it is a stepwise progress through the following stages:

    \list
    \i 1. Iterate through the child objects of the item and set all lighting parameters found.
    \i 2. Set up culling mode in the painter.
    \i 3. Set effects if they exist.
    \i 4. Set all local model view transformations for this item.
    \i 5. Draw this item.
    \i 6. Iterate through the child objects of the item and draw all child items.
    \i 7. Unset the appropriate parameters and states.
    \endlist

    \sa drawItem()
*/

void Item3D::draw(QGLPainter *painter)
{
    int prevId = painter->objectPickId();
    painter->setObjectPickId(d->objectPickId);
    QObjectList list = QObject::children();

    //Lighting
    const QGLLightParameters *currentLight = 0;
    QMatrix4x4 currentLightTransform;
    if (d->light) {
        currentLight = painter->mainLight();
        currentLightTransform = painter->mainLightTransform();
        painter->setMainLight(d->light);
    }

    //Culling
    if ((d->cullFaces & ~CullClockwise) == CullDisabled) {
        glDisable(GL_CULL_FACE);
    } else if (d->cullFaces & CullClockwise) {
        glFrontFace(GL_CW);
        glCullFace(GLenum(d->cullFaces & ~CullClockwise));
        glEnable(GL_CULL_FACE);
    } else {
        glFrontFace(GL_CCW);
        glCullFace(GLenum(d->cullFaces));
        glEnable(GL_CULL_FACE);
    }

    // Blending change for the effect.
    bool viewportBlend = d->viewport ? d->viewport->blending() : false;
    bool effectBlend = d->effect ? d->effect->blending() : viewportBlend;
    if (viewportBlend != effectBlend) {
        if (effectBlend)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
    }

    //Effects
    if (d->effect)
        d->effect->enableEffect(painter);

    //Local and Global transforms

    //1) Item Transforms
    painter->modelViewMatrix().push();
    painter->modelViewMatrix().translate(d->position);
    int transformCount = d->transforms.count();
    if (transformCount>0) {
        // The transformations are applied in reverse order of their
        // lexical appearance in the QML file.
        QMatrix4x4 m = painter->modelViewMatrix();
        for (int index = transformCount - 1; index >= 0; --index) {
            
            d->transforms.at(index)->applyTo(&m);
        }
            
        painter->modelViewMatrix() = m;
    }
    if (d->scale != 1.0f)
        painter->modelViewMatrix().scale(d->scale);
    transformCount = d->pretransforms.count();
    if (transformCount>0) {
        // Pre-transforms for orienting the model.
        QMatrix4x4 m = painter->modelViewMatrix();
        for (int index = transformCount - 1; index >= 0; --index) {
            
            d->pretransforms.at(index)->applyTo(&m);
        }
        painter->modelViewMatrix() = m;
    }

    //Drawing
    if (d->isVisible ) drawItem(painter);
    foreach (QObject *child, list) {
        Item3D *item = qobject_cast<Item3D *>(child);
        if (item)
            item->draw(painter);
    }

    //Unset parameters for transforms, effects etc.
    painter->modelViewMatrix().pop();

    if (d->effect)
        d->effect->disableEffect(painter);
    if (viewportBlend != effectBlend) {
        if (effectBlend)
            glDisable(GL_BLEND);
        else
            glEnable(GL_BLEND);
    }
    if (d->cullFaces != CullDisabled)
        glDisable(GL_CULL_FACE);
    if (d->light)
        painter->setMainLight(currentLight, currentLightTransform);
    painter->setObjectPickId(prevId);
}

/*!
    \internal
    The process of initialising an /l Object3d is a critical step, particularly in 
    complex scenes.  This function initialises the item in \a viewport, and using \a painter.

    During the initialisation process objects are registered as being \i pickable (ie. able
    to be clicked on with the mouse.

    Additionally, in the case of \l Item3D objects which refer to sub-nodes of a mesh, this
    function performs all of the splitting of meshes into sub-branches ready for local
    control by the item.
*/
void Item3D::initialize(Viewport *viewport, QGLPainter *painter)
{
    if (d->isInitialized) return;

    d->viewport = viewport;
    d->objectPickId = viewport->registerPickableObject(this);

    if (mesh() && !meshNode().isEmpty()) {
        int branchNumber = mesh()->createSceneBranch(meshNode());
        if (branchNumber>=0) {
            d->mainBranchId = branchNumber;
        }
        else {
            qWarning()<< "3D item initialization failed: unable to find the specified mesh-node. Defaulting to default node.";
            d->mainBranchId = 0;
        }
    }

    QObjectList list = QObject::children();
    foreach (QObject *child, list) {
        Item3D *item = qobject_cast<Item3D *>(child);
        if (item) {   
            //Event inheritance is generally only declared at initialization, but can also be done at runtime
            //if the user wishes (though not recommended).
            if (inheritEvents()) {
                // Proxy the mouse event signals to the parent so that
                // the parent can trap the signal for its children.
                QObject::connect(item, SIGNAL(clicked()), this, SIGNAL(clicked()));
                QObject::connect(item, SIGNAL(doubleClicked()), this, SIGNAL(doubleClicked()));
                QObject::connect(item, SIGNAL(pressed()), this, SIGNAL(pressed()));
                QObject::connect(item, SIGNAL(released()), this, SIGNAL(released()));
                QObject::connect(item, SIGNAL(hoverEnter()), this, SIGNAL(hoverEnter()));
                QObject::connect(item, SIGNAL(hoverLeave()), this, SIGNAL(hoverLeave()));
            }
            //if the item has no mesh of its own and no meshnode is declared we give it the mesh from the current item.
             if (!item->mesh() && !item->meshNode().isEmpty()) {				 
                item->setMesh(mesh());
			 }
		
            item->initialize(viewport, painter);
        }
    }
    d->isInitialized = true;
}

/*!
    \internal
    Each \l Item3D which has a mesh associated with it contains an index value into that \l Mesh
    object which indicates which \i "branch" of the scene is being drawn by the item.

    This is generally relevant when \l Mesh objects have been split across a number of \l Item3D
    objects at specific mesh nodes.  In this case the mesh will have a number of drawable branches, 
    only one of which will corrsepond to the item currently being drawn.

    These branch IDs are set up at initialisation of the \l Item3D.

    Returns the ID number of the main branch for this item.

    \sa initialize()
*/
int Item3D::mainBranchId() const
{
    return d->mainBranchId;
}


/*!
    \internal
    Sets the ID number of the main scene branch in the item to \a objectID.
*/
void Item3D::setMainBranchId(int objectID)
{
    d->mainBranchId = objectID;
}

/*!
    \internal
*/
void Item3D::classBegin()
{
    d->states()->classBegin();
}

/*!
    \internal
*/
void Item3D::componentComplete()
{
    d->states()->componentComplete();
}

/*!
    \internal
    The \c drawItem function performs the actual drawing of the mesh branch which corresponds
    to the section of the mesh being drawn by the \l Item3D to a specific \a painter.
*/
void Item3D::drawItem(QGLPainter *painter)
{
    if (d->mesh)
        d->mesh->draw(painter, d->mainBranchId);
}

/*!
    \internal
    This function handles the standard mouse events for the item as contained in \a e.

    Returns the boolean value of the regular QObject::event() function.oo
*/
bool Item3D::event(QEvent *e)
{
    // Convert the raw event into a signal representing the user's action.
    if (e->type() == QEvent::MouseButtonPress) {
        QMouseEvent *me = (QMouseEvent *)e;
        if (me->button() == Qt::LeftButton)
            emit pressed();
    } else if (e->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *me = (QMouseEvent *)e;
        if (me->button() == Qt::LeftButton) {
            emit released();
            if (me->x() >= 0)   // Positive: inside object, Negative: outside.
                emit clicked();
        }
    } else if (e->type() == QEvent::MouseButtonDblClick) {
        emit doubleClicked();
    } else if (e->type() == QEvent::Enter) {
        emit hoverEnter();
    } else if (e->type() == QEvent::Leave) {
        emit hoverLeave();
    }
    return QObject::event(e);
}

/*!
    \qmlproperty string Item3D::meshNode

    This property is a simple string which refers to the node in the \l Mesh object which
    is associated with this \l Item3D.

    \sa mesh
*/
QString Item3D::meshNode() const
{
    return d->meshNode;
}

void Item3D::setMeshNode(const QString &node)
{
    //the actual instantiation of the node as the mesh itself is undertaken in the initialize function.
    d->meshNode = node;
}

/*!
    \internal
    Update the \l Viewport with which this item is associated.
*/
void Item3D::update()
{
    if (d->viewport)
        d->viewport->update3d();
}

/*!
    \qmlproperty bool Item3D::isVisible

    A simple boolean property indicating whether the item is visible (and so
    should be drawn), or not visible.  The default value for this property is
    true.

    \sa mesh
*/
bool Item3D::isVisible() const
{
    return d->isVisible;
}

void Item3D::setIsVisible(bool visibility)
{
    d->isVisible = visibility;
}

/*!
  \qmlsignal Item3D::onClicked()

  This signal is emitted when the item is clicked.  Picking must be enabled for this to have any effect.
*/


/*!
  \qmlsignal Item3D::onDoubleClicked()

  This signal is emitted when the item is double clicked.  Picking must be enabled for this to have any effect.

*/


/*!
  \qmlsignal Item3D::onPressed()

  This signal is emitted when the item detects a mouse-button-down event.  Picking must be enabled
  for this to have any effect.
*/


/*!
  \qmlsignal Item3D::onReleased()

  This signal is emitted when the item detects a mouse-button-released event.  Picking must be enabled
  for this to have any effect.
*/


/*!
  \qmlsignal Item3D::onHoverEnter()

  This signal is emitted when a mouseover of the item is detected.  It relies on object picking to be
  of use.
*/


/*!
  \qmlsignal Item3D::onHoverLeave()

  This signal is emitted when the mouseover of the item ceases.  It relies on object picking to be
  used.
*/

QT_END_NAMESPACE
