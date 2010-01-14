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
    MeshPrivate() : dataReply(0), scene(0), refCount(1), nextObjectId(0),
                    completed(false), loaded(false){}
    ~MeshPrivate()
    {
        delete scene;
    }

    struct meshObject {
        QObject * previousParent;
        QGLSceneObject * sceneObject;
    };

    QUrl data;
    QString meshName;
    Mesh *parentMesh;
    QNetworkReply *dataReply;
    QGLAbstractScene *scene;
    QMap<int, QGLSceneObject *>meshObjects;     //These are the objects actually appearing in the mesh.
    int nextObjectId;
    QList<QGLSceneObject *>sceneObjects;    //These are all of the possible objects in the scene.
    int refCount;
    QList<QGLMaterialParameters *> connected;
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
            insertObject = d->scene->defaultObject(QGLSceneObject::Main);
        else
            insertObject = d->scene->object(QGLSceneObject::Mesh, value);
        if (!insertObject && !value.isEmpty()) {
            qWarning() << "could not find" << value << "available:"
                       << d->scene->objectNames(QGLSceneObject::Mesh);
        } else if (!insertObject) {
            qWarning() << "could not find main object in scene!";
        }

        //Add a null object to make the object non-drawable, but still able
        //to run.
        addMeshObject(NULL);
        
        emit dataChanged();
    }
}

void Mesh::dataRequestFinished()
{
    setScene(QGLAbstractScene::loadScene(d->dataReply, d->data));
    d->dataReply->deleteLater();
    d->dataReply = 0;
}

QGLSceneObject *Mesh::getSceneObject(QGLSceneObject::Type type, const QString& name) const
{
	if (d->scene)
		return d->scene->object(type, name);
	else
		qWarning() << "No scene initialised - attempt to get scene object failed.";
	return NULL;
}

void Mesh::setScene(QGLAbstractScene *scene)
{
    delete d->scene;
    resetMeshObjects();
    d->scene = scene;
    if (!d->scene) {
        qWarning("Could not load %s (possibly plugin could not be found)",
                 d->data.toString().toLatin1().constData());    
    } 
    else {
        QGLSceneObject *insertObject;
        if (d->meshName.isEmpty())
            insertObject = d->scene->defaultObject(QGLSceneObject::Main);
        else
            insertObject = d->scene->object(QGLSceneObject::Mesh, d->meshName);        
        //check if we found a valid/useful object to use based on the node name, 
        //otherwise just output a list of valid names
        if (!insertObject && !d->meshName.isEmpty()) {
            qWarning() << "could not find" << d->meshName << "available:"
                       << d->scene->objectNames(QGLSceneObject::Mesh);
        } else if (!insertObject) {
            qWarning() << "could not find main object in scene!";
        }
        //in either case we still need to add an object to the scene, so simply 
        //add a null value to indicate that this object is non-drawable.
        addMeshObject(insertObject);        
    }
    emit dataChanged(); 
    d->loaded = true;
    if (d->completed)
        emit loaded();
}

int Mesh::nextMeshObjectId() const
{
    return d->nextObjectId;
}

int Mesh::addMeshObject(QGLSceneObject *sceneObject)
{
    d->meshObjects.insert(d->nextObjectId, sceneObject);
    return ++d->nextObjectId;    
}

void Mesh::deleteMeshObject(int objectId)
{
    if (!d->meshObjects.remove(objectId)) {
        qWarning() << "Unable to remove mesh object "<<objectId<<" from Mesh... ignoring.";
    }
}

QGLSceneObject *Mesh::getMeshObject(int objectId) const
{
    return d->meshObjects.value(objectId, NULL);
}

void Mesh::resetMeshObjects()
{
    d->meshObjects.clear();
    d->nextObjectId = 0;
}

void Mesh::draw(QGLPainter *painter, int objectID)
{
    QGLSceneObject * targetObject = d->meshObjects.value(objectID, NULL);
    if (!targetObject) {
        qWarning() << "Unable to find valid scene object with ID: " << objectID;
    } else {
        targetObject->apply(painter);
        targetObject->draw(painter);
        targetObject->cleanup(painter);
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

QObject *Mesh::material(const QString& name) const
{    
    if (!d->scene)
        return 0;
    QGLSceneNode *node = qobject_cast<QGLSceneNode *>(getMeshObject(0));
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

int Mesh::splitSceneMesh(QString nodeName, QObject *parent)
{
    if (!d->scene) {
        qWarning() << "Unable to split mesh: no scene initialised - attempt to add scene object failed.";
        return -1;
    }
    else {
        int indexValue = nextMeshObjectId();
        QGLSceneObject *sceneObj = d->scene->object(QGLSceneObject::Mesh, nodeName);
        if (sceneObj) {
            parent ? sceneObj->setParent(parent)  : sceneObj->setParent(this);
            addMeshObject(sceneObj);
            return indexValue;
        }
        else {
            qWarning() << "Warning: Unable to find specified node in given mesh.  Available:"
                       << d->scene->objectNames(QGLSceneObject::Mesh);
            return -1;
        }
    }
}

QT_END_NAMESPACE
