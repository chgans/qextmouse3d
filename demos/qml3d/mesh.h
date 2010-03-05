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

#ifndef MESH_H
#define MESH_H

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativeparserstatus.h>
#include "qglpainter.h"
#include "qglsceneobject.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class MeshPrivate;
class QGLAbstractScene;
class QGLMaterialParameters;
class QGLSceneObject;

class Mesh : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY dataChanged)
    Q_PROPERTY(QString meshName READ meshName WRITE setMeshName NOTIFY dataChanged)

public:
    Mesh(QObject *parent = 0);
    ~Mesh();

    QUrl source() const;
    void setSource(const QUrl& value);

    QString meshName() const;    
    void setMeshName(const QString& value);

    virtual void draw(QGLPainter *painter, int branchId);

    //The following functions relate to allocating the scene and its
    //respective objects, and acessing these nodes.
    void setScene(QGLAbstractScene *scene);
    void initSceneObjectList();
    QStringList getSceneObjectNames();
    QGLSceneObject *getSceneObject();
    QGLSceneObject *getSceneObject(const QString &name);
    QGLSceneObject *getSceneObject(QGLSceneObject::Type type, const QString& name) const;
    
    //The following functions relate to splitting the main scene into sub-branches
    int nextSceneBranchId() const;    
    int createSceneBranch(QString nodeName, QObject *parent = 0);
    int addSceneBranch(QGLSceneObject *rootSceneObject, QObject *previousParent=NULL);
    void restoreSceneBranch(int branchId);
    void resetSceneBranches();
    QGLSceneObject *getSceneBranch(int branchId) const;
    
    void ref();
    bool deref();

    Q_INVOKABLE QObject *material(const QString& name);

    void componentComplete();


Q_SIGNALS:
    void dataChanged();
    void loaded();

private Q_SLOTS:
    void dataRequestFinished();
    
private:
    MeshPrivate *d;

};





QML_DECLARE_TYPE(Mesh)

QT_END_NAMESPACE

QT_END_HEADER

#endif
