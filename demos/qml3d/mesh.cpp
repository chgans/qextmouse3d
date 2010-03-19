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

#include "mesh.h"
#include "qglabstractscene.h"
#include "qglsceneobject.h"
#include "qglscenenode.h"
#include "qglmaterial.h"
#include "qglmaterialcollection.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtCore/qlist.h>

/*!
    \class Mesh
    \brief The Mesh class provides a means of abstracting 3d mesh mesh/geometry representations in
    a way that can be used in QML.  The Mesh class contains necessary properties and methods for
    display and manipulation of 3d objects of this type.
    \since 4.6.?
    \ingroup qt3d
    \ingroup qt3d::qml3d

    \section1 Underlying Architecture

    QML/3d Mesh objects are a QML enabled means of representing any scene class based
    on the \l QGLAbstractScene class.

    In cases where the scene is composed of multiple nodes (for example, in a \i .3ds file)
    the mesh allows users to split away branches of the tree and manipulate them in
    isolation from the original scene graph.

    In order to achieve this the mesh object contains a map correlating numeric identifiers
    for individual branches with specific \l QGLSceneObject objects which are taken from
    the original scene.

    More details on achieving this can be found in the \l Item3d class.

    \section1 Specifying Simple Mesh Objects

    Like other objects in QML/3d, Mesh objects are created by the standard QML syntax for
    objects.  Meshes should always specify an appropriate file containing the data for
    the Mesh,

    \code
    Effect {}
    \endcode

    More complex effects use the usual QML syntax for accessing and updating properties.  In order to specify
    a texture, for example, the following could be used:

    \code
    Effect {
        id: myTextureEffect
        texture: "C:\textures\texture.png"
    }
    \endcode
*/

QT_BEGIN_NAMESPACE

//QML_DEFINE_TYPE(Qt,4,6,Mesh,Mesh)

class MeshPrivate
{
public:
    MeshPrivate() : dataReply(0), scene(0), nextSceneBranchId(0),
                    mainSceneObject(NULL), refCount(1), completed(false), loaded(false){}
    ~MeshPrivate()
    {
        delete scene;
    }

    struct branchObject {
        QObject *previousParent;
        QGLSceneObject *rootSceneObject;
    };

    QUrl data;
    QString meshName;
    Mesh *parentMesh;
    QNetworkReply *dataReply;
    QGLAbstractScene *scene;
    int nextSceneBranchId;
    QMap<int, branchObject> sceneBranches;
    QList<QGLSceneObject *>sceneObjects;
    QGLSceneObject *mainSceneObject;
    QList<QGLMaterial *> connected;
    int refCount;
    bool completed;
    bool loaded;
};

/*!
    Construct a \l Mesh object with \a parent as its parent.
*/
Mesh::Mesh(QObject *parent)
    : QObject(parent)
{
    d = new MeshPrivate();
}

/*!
    Destroy the \l Mesh object and free any unneeded memory.
*/
Mesh::~Mesh()
{
    delete d;
}

/*!
    \property Mesh::source
    \brief Each mesh requires a set of data that describes (among other things) vertices
    transformations, textures, and so on.  This data is generally stored in files and
    specified via the \c source property.

    Source files can be of any type supported by Qt/3d.  The types of file currently
    supported can be found in the \c sceneFormat plugins, with \i .3ds, \i .bez, \i. obj
    files currently being supported.
*/

QUrl Mesh::source() const
{
    return d->data;
}

void Mesh::setSource(const QUrl& value)
{
    if (d->data == value)
        return;
    d->data = value;
//#define QT_NO_LOCALFILE_OPTIMIZED_QML
#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
    if (d->data.scheme() == QLatin1String("file")) {
        setScene(QGLAbstractScene::loadScene(d->data.toLocalFile()));
    } else
#endif
    {
        if (d->dataReply)
            d->dataReply->deleteLater();
        QNetworkRequest req(d->data);
        req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
        d->dataReply = qmlEngine(this)->networkAccessManager()->get(req);
        QObject::connect(d->dataReply, SIGNAL(finished()),
                         this, SLOT(dataRequestFinished()));
    }
}

/*!
    \property Mesh::meshName
    \brief Users can associate a \l QString name with a mesh to facilitate easy finding or
    description of a mesh.
*/
QString Mesh::meshName() const
{
    return d->meshName;
}

void Mesh::setMeshName(const QString& value)
{
    d->meshName = value;
    if (d->loaded && d->scene) {
        QGLSceneObject *insertObject;
        if (value.isEmpty())            
            insertObject = getSceneObject();
        else
            insertObject = getSceneObject(value);
        if (!insertObject && !value.isEmpty()) {
            qWarning() << "could not find" << value << "available:" << getSceneObjectNames();
        } else if (!insertObject) {
            qWarning() << "could not find main object in scene!";
        }

        //Add the object: if null it will make the object non-drawable, but still able
        //to run.
        addSceneBranch(insertObject);
        
        emit dataChanged();
    }
}


/*!
  Once the request for data has been finished the \l Mesh class is now free to load
  the scene.
*/
void Mesh::dataRequestFinished()
{
    setScene(QGLAbstractScene::loadScene(d->dataReply, d->data));
    d->dataReply->deleteLater();
    d->dataReply = 0;
}

/*!
    Because the branches of the overall scene are essentially /i moveable, and the
    standard methods of getting objects/names from a scene rely on a single tree,
    the \l Mesh class supports its own list of mesh objects in the scene which is
    taken on startup from the normal \l QGLAbstractScene based class for the current
    mesh.

    This means that object lookup can be performed at any time without having to
    reconstitute the original scene or maintain logical links between split branches.

    As an added advantage it also eliminates the (minor) additional cost of having
    to traverse the scene-graph/tree in order to locate objects.

    \sa QGLAbstractScene
*/
void Mesh::initSceneObjectList()
{
    d->sceneObjects.clear();
    if (d->scene)
        d->sceneObjects = d->scene->objects(QGLSceneObject::Mesh);

    d->mainSceneObject = d->scene->defaultObject(QGLSceneObject::Main);
}

/*!
    Get the main scene object for the \l QGLAbstractScene associated with this mesh.
*/
QGLSceneObject *Mesh::getSceneObject()
{
    //This variant of the function gets the main scene object for the scene
    if (!d->mainSceneObject)
        initSceneObjectList();

    return d->mainSceneObject;
}

/*!
    Get the scene object called \a name, specified as a QString, and retrieve the
    scene object in this mesh which corresponds to it.
*/
QGLSceneObject *Mesh::getSceneObject(const QString &name)
{    
    //This variant of the function gets the mesh scene object for a scene
    //based on the name of the object.
    if (d->sceneObjects.empty())
        initSceneObjectList();

    foreach (QGLSceneObject *object, d->sceneObjects) {
        if (object && object->objectName()==name)
        {
            return object;
        }
    }

    return NULL;
}

/*!
    Used mainly for diagnostic purposes this function returns a QStringList containing
    all of the object names for the given mesh.
*/
QStringList Mesh::getSceneObjectNames()
{
    //Get a string list of all mesh object names in the scene.
    if (d->sceneObjects.empty())
        initSceneObjectList();

    QStringList names;
    foreach (QGLSceneObject *object, d->sceneObjects) {
        if (object) {
            QString name = object->objectName();
            if (!name.isEmpty())
                names += name;
        }
    }
    return names;
}

/*!
    Sometimes we want objects in a scene other than meshes - this is usually at start-up or
    similar and does not generally need a whole mesh tree, this function, then, is a generic
    mesh tree searcher for a given scene object \a name of a specific \a type.

    This works by traversing the main trunk of the scene object, and so should generally be
    applied before the splitting of a mesh into sub-branches.  Failure to adhere to this
    will result in only those sections of the scene graph being checked which had not been
    previously split away.
*/

QGLSceneObject *Mesh::getSceneObject(QGLSceneObject::Type type, const QString& name) const
{    
	if (d->scene)
		return d->scene->object(type, name);
	else
		qWarning() << "No scene initialised - attempt to get scene object failed.";
	return NULL;
}

/*!
    Set the \a scene associated with this mesh.

    The function attempts to load a meaningful scene if one exists, and will attempt to
    locate the section of the scene to which this mesh applies based on mesh-name.

    Failure to find an appropriate scene will generate the warning messages.

    It should be noted that for successful operation of this function, the appropriate plugin
    file reader should be built and installed.  Failure to do so will result in a warning and
    failure to display the scene.

    \sa QGLAbstractScene
*/
void Mesh::setScene(QGLAbstractScene *scene)
{
    delete d->scene;
    resetSceneBranches();
    d->scene = scene;
    if (!d->scene) {
        qWarning("Could not load %s (possibly plugin could not be found)",
                 d->data.toString().toLatin1().constData());    
    } 
    else {
        QGLSceneObject *insertObject;
        initSceneObjectList();
        if (d->meshName.isEmpty())
            insertObject = getSceneObject();
        else
            insertObject = getSceneObject(d->meshName);
        //check if we found a valid/useful object to use based on the node name, 
        //otherwise just output a list of valid names
        if (!insertObject && !d->meshName.isEmpty()) {
            qWarning() << "could not find" << d->meshName << "available:"
                       << getSceneObjectNames();
        } else if (!insertObject) {
            qWarning() << "could not find main object in scene!";
        }
        //in either case we still need to add an object to the scene, so if we fail
        //we simply add a null value to indicate that this object is non-drawable 
        addSceneBranch(insertObject);        

    }
    emit dataChanged(); 
    d->loaded = true;
    if (d->completed)
        emit loaded();
}

/*!
    When a scene has been deconstructed into several branches, each one being a separately
    manipulated sub-branch of the whole, the branches taken are identified numerically.  This
    function returns the next scene branch ID number
*/
int Mesh::nextSceneBranchId() const
{
    //Retrieve the next unused scene branch ID
    return d->nextSceneBranchId;
}

/*!
    This function "prunes" a specific branch away from the main scene-tree/graph and adds it to the
    list of independent branches drawn by the mesh.  This facilitates animation & picking of specific
    sub-branches of the larger tree.

    The user specifies a sceneobject/node \a nodeName, which will form the "root" of the new branch,
    as well as a \a parent to which this branch shall be attached.  Normally the branch will simply
    be given the original scene as its parent, so that all standard object hierarchy manipulation/
    destruction rules apply, however this \a parent parameter gives the user extra flexibility if
    required.
*/
int Mesh::createSceneBranch(QString nodeName, QObject *parent)
{    
    if (!d->scene) {
        qWarning() << "Unable to split mesh: no scene initialised - attempt to add scene object failed.";
        return -1;
    }
    else {
        int branchId = nextSceneBranchId();        
        QGLSceneObject *sceneObj = getSceneObject(nodeName);        
        QGLSceneNode *sceneNode = qobject_cast<QGLSceneNode *>(sceneObj);
        if (sceneObj) {
            QGLSceneNode *parentNode = qobject_cast<QGLSceneNode *>(sceneNode->parent());
            
            QObject *prevParent=parentNode;//sceneObj->parent();            
            parentNode->removeNode(sceneNode);  //this becomes irrelevant.
            
            //sceneNode->setParent(d->scene);     //TODO: currently this fails as sceneNode changes make problems.
            //If no specific parent is nominated, use the scene specified by the mesh
            parent ? sceneObj->setParent(parent)  : sceneObj->setParent(d->scene);
            addSceneBranch(sceneObj, prevParent);
            
            return branchId;
        }
        else {
            qWarning() << "Warning: Unable to find node " << nodeName << " in given mesh.  Available nodes:"
                       << getSceneObjectNames();
            return -1;
        }
    }
}

/*!
     Given a scene object \a rootSceneObject, this function adds a pointer to the object to an internally
     maintained map of scene branches.

     The \a previousParent of \a rootSceneObject is also stored so that the object can be restored to its
     previous parent if necessary.
*/
int Mesh::addSceneBranch(QGLSceneObject *rootSceneObject, QObject *previousParent)
{
    //when adding a new object to a mesh we also store the previous parent information
    //in case we wish to 'reattach' it to the parent at a later stage.
    MeshPrivate::branchObject newBranch;
    newBranch.previousParent=previousParent;
    newBranch.rootSceneObject = rootSceneObject;

    d->sceneBranches.insert(d->nextSceneBranchId, newBranch);
    
    return ++d->nextSceneBranchId;    
}

/*!
    When an object is deleted or the user otherwise calls this function, the scene branch identified by
    \a branchId is reattached to the parent node from which it was originally pruned.

    If a problem occurs or the parent node is undefined (eg. null), an attempt is made to reattach it instead
    to the primary/default scene object/node, which corresponds to item 0 in the map.

    If the item selected is already the default item, and no parent can be found, it is deleted.
*/
void Mesh::restoreSceneBranch(int branchId)
{
    
    if (d->sceneBranches.contains(branchId)) {
        qWarning() <<"Mesh does not contain branch " << branchId<<".  Ignoring.\n";
        return;
    }
        
    MeshPrivate::branchObject targetBranch = d->sceneBranches.value(branchId);

    if (!targetBranch.previousParent && branchId!=0) {
        targetBranch.rootSceneObject->setParent(getSceneObject());
    }
    else if (!targetBranch.previousParent){
        qWarning() << "Unable to find a parent to reattach default scene object to. Skipping.";        
        targetBranch.rootSceneObject->setParent(d->scene);                
    } else {
        targetBranch.rootSceneObject->setParent(targetBranch.previousParent);
    }

    if (!d->sceneBranches.remove(branchId)) {
        qWarning() << "Unable to remove branch "<<branchId<<" from Mesh. Ignoring.";
    }
}

/*!
    Return a pointer to the scene branch identified by \a branchId.
*/
QGLSceneObject *Mesh::getSceneBranch(int branchId) const
{
    if (!d->sceneBranches.contains(branchId)) return NULL;    
    MeshPrivate::branchObject targetBranch = d->sceneBranches.value(branchId);
    return targetBranch.rootSceneObject;    
}


/*!
    This function iterates through the list of scene branches in the internal map and attempts to
    restore them to their original position in the object hierarchy of the scene.  It then clears the
    map ready for reuse.
*/
void Mesh::resetSceneBranches()
{
    //Delete all "hanging" object entries in the scene branches list and return them
    //to their rightful place in their parent scene if possible.
    for (int i=1; i<d->nextSceneBranchId; i++) {
        restoreSceneBranch(i);
    }

    d->sceneBranches.clear();
    d->nextSceneBranchId = 0;
}

/*!
    Core "draw" function for the mesh draws scene branch \a branchId using \a painter.
*/
void Mesh::draw(QGLPainter *painter, int branchId)
{
    if (!d->sceneBranches.contains(branchId)) {
        qWarning() << "Unable to find valid scene object with ID: " << branchId;
    } else {
        MeshPrivate::branchObject targetBranch = d->sceneBranches.value(branchId);
        targetBranch.rootSceneObject->apply(painter);
        targetBranch.rootSceneObject->draw(painter);
        targetBranch.rootSceneObject->cleanup(painter);
    }
}

/*!
    Reference counting increment.
*/
void Mesh::ref()
{
    ++(d->refCount);
}

/*!
    Reference counting decrement; returns true if there are still outstanding references
    to the class.
*/
bool Mesh::deref()
{
    --(d->refCount);
    return d->refCount > 0;
}


/*!
    Assign the mesh the material \a name from the \l QGLMaterialCollection class.
*/
QObject *Mesh::material(const QString& nodeName, const QString& materialName)
{    
    if (!d->scene)
        return 0;
    
    QGLSceneObject *sceneObject;
    
    if (nodeName.isEmpty())
        sceneObject = getSceneObject();
    else
        sceneObject = getSceneObject(nodeName);
    
    if(!sceneObject || materialName.isEmpty()) {
        qWarning()  << "Attempt to get material data " <<materialName << " from scene node " 
                    << nodeName <<" failed.";        
        return NULL;
    }

    QGLSceneNode *node = qobject_cast<QGLSceneNode *>(sceneObject);   
    
    QGLMaterialCollection *p = node->palette();
    
    QGLMaterial *params =  p->material(materialName);
    if (params && !d->connected.contains(params)) {
        d->connected.append(params);
        connect(params, SIGNAL(materialChanged()), this, SIGNAL(dataChanged()));
    }
    return params;
}

/*!
    Checks that all loading and initialisation has been finished, and emits the  loaded() signal if
    the component is complete.

    \sa loaded()
*/
void Mesh::componentComplete()
{
    d->completed = true;
    if (d->loaded)
        emit loaded();
}

/*!
  \fn void Mesh::dataChanged();

  Signals a change to one of the properties of the class, or the scene represented  by the class.

*/

/*!
  \fn void Mesh::loaded()

  Signals that loading of the mesh is complete.
*/

QT_END_NAMESPACE
