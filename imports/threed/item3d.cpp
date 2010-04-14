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
#include "qglsceneobject.h"
#include "qglscenenode.h"
#include "qglview.h"
#include <QtGui/qevent.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/private/qdeclarativestategroup_p.h>
#include <QtDeclarative/private/qdeclarativeitem_p.h>

/*!
    \class Item3d
    \brief The Item3d class encapsulates 3d objects or \i items, as they are known.
    It contains all of the properties and methods needed for simple 3d operations.
    part of a QML/3d script.
    \since 4.6.q
    \ingroup qt3d
    \ingroup qt3d::qml3d

    \section1 Simple 3d Object Definition

    The most basic use case for the Item3d class is the creation and display of a 
    single simple item in the 3d environment.

    Many such items within a 3d environment are defined as a single logical component
    mesh which is treated as a stand-alone object for the purposes of rotation, 
    scaling, and user interaction via "picking".

    Such an object can easily be defined in QML/3d using the following code:

    \code
    Item3d {
        id: teapot
        mesh: Mesh { source: "teapot.bez" }
        effect: Effect {}
		cullFaces: "CullBackFaces"
    }
    \endcode

    This simple code will create a 3d item based on the \i teapot.bez mesh using 
    back-face culling.

    Notice that in this case the effect and mesh are defined within the body of 
    the item definition.  Where there is little complexity, this method of
    defining items simplifies the resultant code and makes modification of
    the QML easier.

    \section1 Embedding Item3d Objects

    Consider the following:

    \code
    Item3d {
        id: cup
        mesh:  Mesh { source: "teacup.bez" }
		effect: Effect {}
		cullFaces: "CullBackFaces"

        Item3d {
            id: saucer
            mesh: Mesh { source: "saucer.bez" }
            cullFaces: "CullBackFaces"
        }

        position: Qt.vector3d(10, -10, -10)
    }
    \endcode

    This demonstrates the capability of embedding one Item3d within another.  In
    this case the \i saucer item is a child of the \i cup item.

    All transformations applied to the parent item are also applied to the child, 
    so in this case both the cup and saucer will be translated based on the 
    position vector defined in the cup item's definition.

    In this case any additional transformations applied to the child item will not 
    affect the parent, and are local only to that item (and to its children if any
    exist).

    This allows a user to group together items logically so that transformations and
    user interactions can be applied to groups of objects as if they were a whole.

    \section1 Using Sub-nodes of 3d Objects

    In more complex applications the user may wish to load a complex mesh
    which is made up of a number of components or nodes which may be organised
    into a tree like structure.  In this case they may wish to interact with, 
    animate, or otherwise modify individual sub-nodes of a mesh.

    Item3d leverages the existing \bold {Qt Object Model} in order to allow QML/3d users
    this type of control over their 3d items.

    Consider the following QML/3d script:

    \code
    Item3d {
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

        Item3d {
			id: rotor
			property bool spin: false				
			meshNode: "rotorBladesNode"
			Item3d {meshNode: "rotorHubNode"}
            			
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

//QML_DEFINE_TYPE(Qt,4,6,Item3d,Item3d)

class Item3dPrivate
{
public:
    Item3dPrivate(Item3d *_item)
        : item(_item)
        , viewport(0)
        , position(0.0f, 0.0f, 0.0f)
        , pivot(0.0f,0.0f,0.0f)
        , usePivot(false)
        , scale(1.0f)
        , mesh(0)
        , effect(0)
        , objectPickId(-1)
        , cullFaces(Item3d::CullDisabled)
        , _stateGroup(0)
        , inheritEvents(false)
        , isVisible(true)
        , isInitialized(false)
    {
    }

    Item3d *item;
    Viewport *viewport;
    QVector3D position;
    QVector3D pivot;
    bool usePivot;
    qreal scale;
    Mesh *mesh;
    Effect *effect;    
    int objectPickId;
    Item3d::CullFaces cullFaces;
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
    static Item3d *children_at(QDeclarativeListProperty<Item3d> *, int);
    static void children_append(QDeclarativeListProperty<Item3d> *, Item3d *);
    static int children_count(QDeclarativeListProperty<Item3d> *);

    // transform property
    static int transform_count(QDeclarativeListProperty<QGraphicsTransform> *list);
    static void transform_append(QDeclarativeListProperty<QGraphicsTransform> *list, QGraphicsTransform *);
    static QGraphicsTransform *transform_at(QDeclarativeListProperty<QGraphicsTransform> *list, int);
    static void transform_clear(QDeclarativeListProperty<QGraphicsTransform> *list);
    QList<QGraphicsTransform *> transforms;


};


int Item3dPrivate::transform_count(QDeclarativeListProperty<QGraphicsTransform> *list)
{  
    QGraphicsObject *object = qobject_cast<QGraphicsObject *>(list->object);
    if (object) {
        QGraphicsItemPrivate *d = QGraphicsItemPrivate::get(object);
        return d->transformData ? d->transformData->graphicsTransforms.size() : 0;
    } else {
        qWarning()<<"Warning: could not find Item3d to query for transformation count.";
        return 0;
    }
}

void Item3dPrivate::transform_append(QDeclarativeListProperty<QGraphicsTransform> *list, QGraphicsTransform *item)
{
    Item3d *object = qobject_cast<Item3d *>(list->object);
    QList<QGraphicsTransform *> *ptrans;
    if (object)
    {
        ptrans = &object->d->transforms;

        //We now need to connect the underlying transform so that any change will update the graphical item.
        if (!ptrans->contains(item)) {
            ptrans->append(item);            
            if (qobject_cast<QGraphicsScale *>(item)) {
                QObject::connect(item, SIGNAL(originChanged()),
                                 object, SLOT(update()));
                QObject::connect(item, SIGNAL(scaleChanged()),
                                 object, SLOT(update()));
            } else if (qobject_cast<QGraphicsRotation *>(item)) {
                QObject::connect(item, SIGNAL(originChanged()),
                                 object, SLOT(update()));
                QObject::connect(item, SIGNAL(angleChanged()),
                                 object, SLOT(update()));
                QObject::connect(item, SIGNAL(axisChanged()),
                                 object, SLOT(update()));
            }
        }
    }
    else
        qWarning()<<"Warning: could not find Item3d to add transformation to.";
}

QGraphicsTransform *Item3dPrivate::transform_at(QDeclarativeListProperty<QGraphicsTransform> *list, int idx)
{    
   Item3d *object = qobject_cast<Item3d *>(list->object);
    if (object) {
        return object->d->transforms.at(idx);
    } else {
        qWarning()<<"Warning: could not find Item3d to query for transformations";
        return 0;
    }
    return 0;
}

void Item3dPrivate::transform_clear(QDeclarativeListProperty<QGraphicsTransform> *list)
{

    Item3d *object = qobject_cast<Item3d *>(list->object);
    if (object) {
        object->d->transforms.clear();
        object->update();
    }
    else
        qWarning()<<"Warning: could not find Item3d to clear of transformations";
}



void Item3dPrivate::data_append(QDeclarativeListProperty<QObject> *prop, QObject *o)
{
    Item3d *i = qobject_cast<Item3d *>(o);
    if (i) 
        i->setParent(static_cast<Item3d *>(prop->object));
    else
        o->setParent(static_cast<Item3d *>(prop->object));
}


QObject *Item3dPrivate::resources_at(QDeclarativeListProperty<QObject> *prop, int index)
{
    QObjectList children = prop->object->children();
    if (index < children.count())
        return children.at(index);
    else
        return 0;
}

void Item3dPrivate::resources_append(QDeclarativeListProperty<QObject> *prop, QObject *o)
{
    o->setParent(prop->object);
}

int Item3dPrivate::resources_count(QDeclarativeListProperty<QObject> *prop)
{
    return prop->object->children().count();
}

Item3d *Item3dPrivate::children_at(QDeclarativeListProperty<Item3d> *prop, int index)
{
    QObjectList thechildren = static_cast<Item3d*>(prop->object)->children();

    if (index < thechildren.count())
        return qobject_cast<Item3d *>(thechildren.at(index));
    else
        return 0;
}

void Item3dPrivate::children_append(QDeclarativeListProperty<Item3d> *prop, Item3d *i)
{
    if (i)
    {
        i->setParent(static_cast<QObject*>(prop->object));

        //Because this is now a static function, we can no longer apply the following :-(
        //however it is still carried out in the "initialize" function of Item3d.
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

int Item3dPrivate::children_count(QDeclarativeListProperty<Item3d> *prop)
{
    return static_cast<Item3d*>(prop->object)->children().count();
}


QDeclarativeStateGroup *Item3dPrivate::states()
{
    if (!_stateGroup) {
        _stateGroup = new QDeclarativeStateGroup(item);
        _stateGroup->classBegin();
        QObject::connect(_stateGroup, SIGNAL(stateChanged(QString)),
                         item, SIGNAL(stateChanged(QString)));
    }

    return _stateGroup;
}


/*!
    Constructs an \l Item3d with the default properties and
    attaches it to \a parent.
*/
Item3d::Item3d(QObject *parent)
    : QObject(parent)
{
    d = new Item3dPrivate(this);
}

/*!
    Destroys this \l Item3d object.
*/
Item3d::~Item3d()
{
    delete d;
}



/*!
    \property Item3d::position
    \brief This defines the \l Item3d position in 3d space as a QVector3d.

    The default value for this property is \c (0.0,0.0,0.0)

    \sa x(), y(), z()
*/
QVector3D Item3d::position() const
{
    return d->position;
}

void Item3d::setPosition(const QVector3D& value)
{
    d->position = value;
    emit positionChanged();
    update();
}

/*!
    \property Item3d::x
    \brief The x position of the \l Item3d in 3d space.

    The default value for this property is \c 0.0

    \sa position(), y(), z()
*/
qreal Item3d::x() const
{
    return d->position.x();
}

void Item3d::setX(qreal value)
{
    d->position.setX(value);
    emit positionChanged();
    update();
}

/*!
    \property Item3d::y
    \brief The y position of the \l Item3d in 3d space.

    The default value for this property is \c 0.0

    \sa position(), x(), z()
*/
qreal Item3d::y() const
{
    return d->position.y();
}

void Item3d::setY(qreal value)
{
    d->position.setY(value);
    emit positionChanged();
    update();
}

/*!
    \property Item3d::z
    \brief The z position of the \l Item3d in 3d space.

    The default value for this property is \c 0.0

    \sa position(), x(), y()
*/
qreal Item3d::z() const
{
    return d->position.z();
}

void Item3d::setZ(qreal value)
{
    d->position.setZ(value);
    emit positionChanged();
    update();
}

/*!
    \property Item3d::scale
    \brief The scaling factor for representing the \l Item3d in the 3d scene.
    
    The default value for this property is \c 1.0.
*/
qreal Item3d::scale() const
{
    return d->scale;
}

void Item3d::setScale(qreal value)
{
    d->scale = value;
    emit scaleChanged();
    update();
}

/*!
    \property Item3d::transform
    \brief Generally objects in 3d space will have undergone some manner
    of 3d transformation prior to display.  Examples of such transformations
    include rotations about the x,y, and z axes, translation, and so on.

    Each \l Item3d maintains a list of transforms to apply to it through this 
    property.  In scripting terms a transform can be applied as follows:

    \code
    Item3d {
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
*/


QDeclarativeListProperty<QGraphicsTransform> Item3d::transform()
{
    return QDeclarativeListProperty<QGraphicsTransform>(this, 0, d->transform_append, d->transform_count,
                                               d->transform_at, d->transform_clear);
}

/*!
    \property Item3d::inheritEvents
    \brief Users are able to interact with 3d items in a scene through (for example) the
    use of the mouse.  These, and other, Qt events can be captured by an \l Item3d using the
    same underlying \l QObject architecture shared by all of Qt.

    Often a user will only want an item to capture mouse events for itself, leaving
    child items to handle their mouse events locally.  Under many circumstances, however, it 
    is necessary for a parent object to collect all mouse events for itself and its child 
    items.  Usually this inheritance of events is only defined at initialisation for an \l Item3d
    
    The \c inheritEvents property, however, is a simple boolean property which provides a mechanism
    for both initialisation time and programmatic modification of this.

    Setting the property to \c true connects the signals for all child items to the appropriate
    signals for the item itself.  Conversely setting the property to \c false disconnects the 
    events.

    The default value for this property is \c false.
*/
bool Item3d::inheritEvents() const
{
    return d->inheritEvents;
}

void Item3d::setInheritEvents(bool inherit)
{
    d->inheritEvents = inherit;

    //Generally we would only want to 
    QObjectList list = QObject::children();
    if (inherit)
    {
        foreach (QObject *child, list) {
		    Item3d *subItem =qobject_cast<Item3d *>(child);	
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
		    Item3d *subItem =qobject_cast<Item3d *>(child);	
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
    \property Item3d::mesh
    \brief Objects in most 3d environments are almost invariably defined as meshes - sets of 
    vertices which when linked as polygons form a recognisable 3d object.  Qt3d currently 
    supports a number of these \i {scene formats}, including \i {.obj} file, bezier patches 
    \i {(.bez)}, and \i {.3ds} files.

    These meshes are abstracted into the mesh class, which is defined for an \l Item3d through 
    this property.    

    By default this value is null, and so a mesh must be defined in order for the item to
    be displayed
*/
Mesh *Item3d::mesh() const
{
    return d->mesh;
}

void Item3d::setMesh(Mesh *value)
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
    \property Item3d::effect
    \brief Qt3d supports the use of effects for modulating the display of items - texture effects, 
    fog effects, material effects, and so on.

    The exact effects correlated with an item are set using this property.

    By default no effect is supplied (\c null ), and so an effect - even an empty one - must be defined.
    \sa Effect
*/
Effect *Item3d::effect() const
{
    return d->effect;
}

void Item3d::setEffect(Effect *value)
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
    \property Item3d::children
    \brief This property exists to allow declaration of specific child items for the Item3d.
*/
QDeclarativeListProperty<Item3d> Item3d::fxChildren()
{
    return QDeclarativeListProperty<Item3d>(this, 0, Item3dPrivate::children_append,
                                                     Item3dPrivate::children_count, 
                                                     Item3dPrivate::children_at); 
}

/*!
    \property Item3d::resources
    \brief This property exists to allow future expansion of the \i Item3d class to include
    additional data and resources.  Currently there is no underlying implementation for this.
*/
QDeclarativeListProperty<QObject> Item3d::resources()
{
    return QDeclarativeListProperty<QObject>(this, 0, Item3dPrivate::resources_append, 
                                             Item3dPrivate::resources_count, 
                                             Item3dPrivate::resources_at); 
}



/*!
    \property Item3d::data
    \brief This property exists to allow future expansion of the \i Item3d class to include
    additional data and resources.  Currently there is no underlying implementation for this.
*/
QDeclarativeListProperty<QObject> Item3d::data() 
{
    return QDeclarativeListProperty<QObject>(this, 0, Item3dPrivate::data_append);
}

/*!
    \property Item3d::states
    \brief QML allows users to define any number of states for objects, including \l Item3d objects.
    These states are arbitrarily assigned and can be used to represent anything the user desires.
    An \l Item3d representing a door, for example, may have an \i "open" and \i "closed" state.

    States can be connected by transitions, which define an animation or other transitional change
    which occurs during the change from one state to another.  In our open and closed door state,
    for example, the transition may define the rotation of the opening door when it swings open and
    swings back closed.

    By default the list of valid states for the item is empty.

    /sa state(), QDeclarativeState, transitions()
*/
QDeclarativeListProperty<QDeclarativeState> Item3d::states()
{    
    return d->states()->statesProperty();
}

/*!
    Occasionally it is necessary to find a given state based on its \a name as expressed by a 
    string.  This function allows users to do so by providing a QString name, and returning
    a corresponding QDeclarativeState.
*/
QDeclarativeState *Item3d::findState(const QString &name) const
{
    if (!d->_stateGroup)
        return 0;
    else
        return d->_stateGroup->findState(name);
}

/*!
    \property Item3d::transitions
    \brief When an object in QML moves from one state to another its behavior during this 
    change can be defined using transitions.  These transitions may define changes to one or
    more objects or properties, and may include animations such as rotations, scaling, 
    translation, and so on.

    Users can define a set of transitional procedures using this property.
    
    As with states, by default there are no transitions defined.
    
    /sa state(), QDeclarativeState, states()
    
*/
QDeclarativeListProperty<QDeclarativeTransition> Item3d::transitions()
{    
    return d->states()->transitionsProperty();
}


/*!
    \property Item3d::state
    \brief This property describes the current state of the \l Item3d as defined in the list of
    allowable states for this item.

    By default an item's state is undefined and irrelevant.  It is only when the states property 
    has been modified that a current state makes any sense.

    /sa states()
*/

QString Item3d::state() const
{
    if (!d->_stateGroup)
        return QString();
    else
        return d->_stateGroup->state();
}

void Item3d::setState(const QString &state)
{
    d->states()->setState(state);
}


/*!
    \enum Item3d::CullFace
    This enumeration defines the method of face culling to be used for an \l Item3d.  The 
    allowable values are:

    \value CullDisabled Do not use culling.
    \value CullFrontFaces Cull the front faces of the object.
    \value CullBackFaces Cull the back faces of the object.
    \value CullAllFaces Cull all faces of the object.
    \value CullClockwise Cull faces based on clockwise winding of vertices.
*/

/*!
    \property Item3d::cullFaces
    \brief Face culling of an item in 3d space can be carried out in a number of ways.  
    This property defines the culling method to be used based on the \l Item3d::CullFaces 
    enumeration.

    By default this value is set to \c Item3d::CullDisabled

    \sa CullFaces
*/
Item3d::CullFaces Item3d::cullFaces() const
{
    return d->cullFaces;
}

void Item3d::setCullFaces(Item3d::CullFaces value)
{
    if (d->cullFaces != value) {
        d->cullFaces = value;
        emit meshChanged();
    }
}

/*!
    \property Item3d::name
    \brief A simple string name for the object.  This is useful for debugging, but also as a
    meaningful way of describing items in a manner which can be easily displayed/printed.
*/
QString Item3d::name() const
{
    return d->name;
}

void Item3d::setName(QString nameString)
{
    d->name = nameString;
}

/*!
    This function allows the user to get a scene object from the underlying mesh which forms
    an \l Item3d.  

    By passing the \a type of node (defined as a \l QGLSceneObject::Type enumeration), along
    with the \a name of the scene object, the user can access the corresponding \l QGLSceneObject
    for that name.

    Failure to find the scene object will return \i null.
*/
QGLSceneObject *Item3d::getSceneObject(QGLSceneObject::Type type, const QString& name) const
{
    return mesh()->getSceneObject(type, name);
}

/*!
    Performs the actual drawing of the Item3d using \a painter.

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

void Item3d::draw(QGLPainter *painter)
{
    int prevId = painter->objectPickId();
    painter->setObjectPickId(d->objectPickId);
    QObjectList list = QObject::children();
    bool haveLights = false;

	//Lighting
    foreach (QObject *child, list) {
        QGLLightParameters *light = qobject_cast<QGLLightParameters *>(child);
        if (light) {
            painter->setMainLight(light);
            haveLights = true;
        }
    }

    //Culling
    painter->setCullFaces((QGL::CullFaces)(int)(d->cullFaces));

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
	
    //Drawing
    if (d->isVisible ) drawItem(painter);
    foreach (QObject *child, list) {
        Item3d *item = qobject_cast<Item3d *>(child);
        if (item)
            item->draw(painter);
    }

    //Unset parameters for transforms, effects etc.
    painter->modelViewMatrix().pop();

    if (d->effect)
        d->effect->disableEffect(painter);
    if (d->cullFaces != CullDisabled)
        painter->setCullFaces(QGL::CullDisabled);
    if (haveLights) {
        foreach (QObject *child, list) {
            QGLLightParameters *light = qobject_cast<QGLLightParameters *>(child);
            if (light)
                painter->setMainLight(0);
        }
    }
    painter->setObjectPickId(prevId);
}

/*!
    The process of initialising an /l Object3d is a critical step, particularly in 
    complex scenes.  This function initialises the item in \a viewport, and using \a painter.

    During the initialisation process objects are registered as being \i pickable (ie. able
    to be clicked on with the mouse.

    Additionally, in the case of \l Item3d objects which refer to sub-nodes of a mesh, this
    function performs all of the splitting of meshes into sub-branches ready for local
    control by the item.
*/
void Item3d::initialize(Viewport *viewport, QGLPainter *painter)
{
    if (d->isInitialized) return;

    d->viewport = viewport;
    QGLView *view = viewport->view();
    
    if (view) {
        d->objectPickId = viewport->nextPickId();
        view->registerObject(d->objectPickId, this);        
    }    

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
        Item3d *item = qobject_cast<Item3d *>(child);
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
             if (!item->mesh()) {
                item->setMesh(mesh());
            }

            item->initialize(viewport, painter);
        }
    }
    d->isInitialized = true;
}

/*!
    Each \l Item3d which has a mesh associated with it contains an index value into that \l Mesh
    object which indicates which \i "branch" of the scene is being drawn by the item.

    This is generally relevant when \l Mesh objects have been split across a number of \l Item3d
    objects at specific mesh nodes.  In this case the mesh will have a number of drawable branches, 
    only one of which will corrsepond to the item currently being drawn.

    These branch IDs are set up at initialisation of the \l Item3d.

    Returns the ID number of the main branch for this item.

    \sa initialize()
*/
int Item3d::mainBranchId() const
{
    return d->mainBranchId;
}


/*!
    Sets the ID number of the main scene branch in the item to \a objectID.
*/
void Item3d::setMainBranchId(int objectID)
{
    d->mainBranchId = objectID;
}

/*!
    The loading and instanciation of components in QML is not necessarily a directly linear 
    process.  As such the \c componentComplete() function allows update/handling of this value.
*/
void Item3d::componentComplete()
{
    d->states()->componentComplete();
}

/*!
    The \c drawItem function performs the actual drawing of the mesh branch which corresponds
    to the section of the mesh being drawn by the \l Item3d to a specific \a painter.
*/
void Item3d::drawItem(QGLPainter *painter)
{
    if (d->mesh)
        d->mesh->draw(painter, d->mainBranchId);
}

/*!
    This function handles the standard mouse events for the item as contained in \a e.

    Returns the boolean value of the regular QObject::event() function.oo
*/
bool Item3d::event(QEvent *e)
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
    \property Item3d::meshNode
    \brief This property is a simple string which refers to the node in the \l Mesh object which
    is associated with this \l Item3d.

    \sa mesh()
*/
QString Item3d::meshNode() const
{
    return d->meshNode;
}

void Item3d::setMeshNode(const QString &node)
{
    //the actual instanciation of the node as the mesh itself is undertaken in the initialize function.
    d->meshNode = node;
}

/*!
    Update the \l Viewport with which this item is associated.
*/
void Item3d::update()
{
    if (d->viewport)
        d->viewport->update3d();
}


/*!
    \property Item3d::isVisible
    \brief A simple boolean property indicating whether the item is visible (and so should be 
    drawn), or not visible.

    \sa mesh()
*/
bool Item3d::isVisible() const
{
    return d->isVisible;
}

void Item3d::setIsVisible(bool visibility)
{
    d->isVisible = visibility;
}

/*!
  \fn void Item3d::positionChanged();

  This signal is emitted when the position property changes.
*/

/*!
  \fn void Item3d::scaleChanged();

  This signal is emitted when the scale property changes.
*/


/*!
  \fn void Item3d::rotationChanged();

  This signal is emitted when the rotation property changes.
*/


/*!
  \fn void Item3d::meshChanged();

  This signal is emitted when the mesh property changes.
*/


/*!
  \fn void Item3d::meshNodeChanged();

This signal is emitted when the meshNode property changes
*/


/*!
  \fn void Item3d::effectChanged();

  This signal is emitted when the effect property changes.
*/


/*!
  \fn void Item3d::stateChanged(const QString &state);

  This signal is emitted when the stateproperty changes to a new \a state.
*/


/*!
  \fn void Item3d::clicked();

  This signal is emitted when the item is clicked.  Picking must be enabled for this to have any effect.
*/


/*!
  \fn void Item3d::doubleClicked();

  This signal is emitted when the item is double clicked.  Picking must be enabled for this to have any effect.

*/


/*!
  \fn void Item3d::pressed();

  This signal is emitted when the item detects a mouse-button-down event.  Picking must be enabled
  for this to have any effect.
*/


/*!
  \fn void Item3d::released();

  This signal is emitted when the item detects a mouse-button-released event.  Picking must be enabled
  for this to have any effect.
*/


/*!
  \fn void Item3d::hoverEnter();

  This signal is emitted when a mouseover of the item is detected.  It relies on object picking to be
  of use.
*/


/*!
  \fn void Item3d::hoverLeave();

  This signal is emitted when the mouseover of the item ceases.  It relies on object picking to be
  used.
*/


/*!
  \fn void Item3d::nameChanged();

  This signal is emitted when the name property changes.
*/


/*!
  \fn void Item3d::inheritEventsChanged();

  This signal is emitted when the inheritEvents property changes.
*/


/*!
  \fn void Item3d::isVisibleChanged();

  This signal is emitted when the isVisible property changes.
*/


QT_END_NAMESPACE
