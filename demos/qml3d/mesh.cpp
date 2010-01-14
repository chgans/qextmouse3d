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

#include "mesh.h"
#include "qglabstractscene.h"
#include "qglsceneobject.h"
#include "qglscenenode.h"
#include "qglmaterialparameters.h"
#include "qglmaterialcollection.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtDeclarative/qmlengine.h>
#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,Mesh,Mesh)

class MeshPrivate
{
public:
    MeshPrivate() : dataReply(0), scene(0), refCount(1), nextSceneBranchId(0),
                    mainSceneObject(NULL), completed(false), loaded(false){}
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
    QList<QGLMaterialParameters *> connected;
    int refCount;
    bool completed;
    bool loaded;
};

Mesh::Mesh(QObject *parent)
    : QObject(parent)
{
    d = new MeshPrivate();
}

Mesh::~Mesh()
{
    delete d;
}

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

void Mesh::dataRequestFinished()
{
    setScene(QGLAbstractScene::loadScene(d->dataReply, d->data));
    d->dataReply->deleteLater();
    d->dataReply = 0;
}

void Mesh::initSceneObjectList()
{
    //Because the branches of the overall scene are moveable, and the 
    //standard methods of getting objects/names from a scene rely on
    //a single tree, the Mesh class supports its own list of mesh objects
    //in the scene.
    d->sceneObjects.clear();
    if (d->scene)
        d->sceneObjects = d->scene->objects(QGLSceneObject::Mesh);

    d->mainSceneObject = d->scene->defaultObject(QGLSceneObject::Main);
}

QGLSceneObject *Mesh::getSceneObject()
{
    //This variant of the function gets the main scene object for the scene
    if (!d->mainSceneObject)
        initSceneObjectList();

    return d->mainSceneObject;
}

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

QGLSceneObject *Mesh::getSceneObject(QGLSceneObject::Type type, const QString& name) const
{
    //Sometimes we want objects other than meshes - this is usually at start-up or similar,
    //and does not generally need a whole mesh tree, this function, then, is a generic 
    //mesh tree searcher for a given named scene object of a specific type.
	if (d->scene)
		return d->scene->object(type, name);
	else
		qWarning() << "No scene initialised - attempt to get scene object failed.";
	return NULL;
}

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

int Mesh::nextSceneBranchId() const
{
    //Retrieve the next unused scene branch ID
    return d->nextSceneBranchId;
}

int Mesh::createSceneBranch(QString nodeName, QObject *parent)
{
    //This function "prunes" a specific branch away from the main scene-tree/graph and adds it to the
    //list of independent branches drawn by the mesh.  This facilitates animation & picking of specific
    //sub-branches of the larger tree.
    if (!d->scene) {
        qWarning() << "Unable to split mesh: no scene initialised - attempt to add scene object failed.";
        return -1;
    }
    else {
        int branchId = nextSceneBranchId();        
        QGLSceneObject *sceneObj = getSceneObject(nodeName);        
        if (sceneObj) {
            QObject *prevParent=sceneObj->parent();
            parent ? sceneObj->setParent(parent)  : sceneObj->setParent(this);
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

void Mesh::restoreSceneBranch(int branchId)
{
    //When we delete an object we first attempt to reattach the underlying QGLSceneObject
    //to its original parent node.  If the parent node is specified as "NULL" we attempt
    //to attach it to the main scene node of the object (ie. item 0 in the map).
    
    if (d->sceneBranches.contains(branchId)) {
        qWarning() <<"Mesh does not contain branch " << branchId<<".  Ignoring.\n";
        return;
    }
        
    MeshPrivate::branchObject targetBranch = d->sceneBranches.value(branchId);

    if (!targetBranch.previousParent) {
        targetBranch.rootSceneObject->setParent(getSceneObject());
    } else {
        targetBranch.rootSceneObject->setParent(targetBranch.previousParent);
    }

    if (!d->sceneBranches.remove(branchId)) {
        qWarning() << "Unable to remove branch "<<branchId<<" from Mesh. Ignoring.";
    }
}

QGLSceneObject *Mesh::getSceneBranch(int branchId) const
{
    if (!d->sceneBranches.contains(branchId)) return NULL;    
    MeshPrivate::branchObject targetBranch = d->sceneBranches.value(branchId);
    return targetBranch.rootSceneObject;    
}

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

void Mesh::ref()
{
    ++(d->refCount);
}

bool Mesh::deref()
{
    --(d->refCount);
    return d->refCount > 0;
}

QObject *Mesh::material(const QString& name)
{    
    if (!d->scene)
        return 0;
    
    QGLSceneObject *sceneObject;
    
    if (name.isEmpty())
        sceneObject = getSceneObject();
    else
        sceneObject = getSceneObject(name);
    
    if(!sceneObject) {
        qWarning()  << "Attempt to get material data from scene node " 
                    << name <<" failed.";
        return NULL;
    }

    QGLSceneNode *node = qobject_cast<QGLSceneNode *>(sceneObject);
    QGLMaterialCollection *p = node->geometry()->palette();
    int index = p->materialIndexByName(name);
    if (index == -1)
        return 0;
    QGLMaterialParameters *params = p->materialByIndex(index);
    if (params && !d->connected.contains(params)) {
        d->connected.append(params);
        connect(params, SIGNAL(materialChanged()), this, SIGNAL(dataChanged()));
    }
    return params;
}

void Mesh::componentComplete()
{
    d->completed = true;
    if (d->loaded)
        emit loaded();
}

QT_END_NAMESPACE
