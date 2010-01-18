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

#ifndef Item3d_H
#define Item3d_H

#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qgraphicstransform.h>
#include <QtDeclarative/qml.h>
#include <QtDeclarative/qmllist.h>
#include <QtDeclarative/private/qmlstate_p.h>
#include <QtDeclarative/private/qmltransition_p.h>
#include <QtDeclarative/qmlparserstatus.h>
#include "qglpainter.h"
#include "mesh.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Item3dPrivate;
class Effect;
class Qml3dView;
class Viewport;
class QGLSceneNode;

class Item3d : public QObject, public QmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QmlParserStatus)
    Q_ENUMS(CullFace)
    Q_FLAGS(CullFaces)
    Q_FLAGS(Mode)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY positionChanged)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY positionChanged)
    Q_PROPERTY(qreal z READ z WRITE setZ NOTIFY positionChanged)
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QmlList<QGraphicsTransform *>* transform READ transform DESIGNABLE false FINAL)
    Q_PROPERTY(Mesh *mesh READ mesh WRITE setMesh NOTIFY meshChanged)
    Q_PROPERTY(Effect *effect READ effect WRITE setEffect NOTIFY effectChanged)    
    Q_PROPERTY(QmlList<QObject *>* resources READ resources DESIGNABLE false)
    Q_PROPERTY(QmlList<QObject *>* data READ data DESIGNABLE false)
    Q_PROPERTY(QmlList<QmlState *>* states READ states DESIGNABLE false)
    Q_PROPERTY(QmlList<QmlTransition *>* transitions READ transitions DESIGNABLE false)
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(CullFaces cullFaces READ cullFaces WRITE setCullFaces NOTIFY meshChanged)
    Q_PROPERTY(QString meshNode READ meshNode WRITE setMeshNode NOTIFY meshNodeChanged)
    Q_PROPERTY(bool inheritEvents READ inheritEvents WRITE setInheritEvents NOTIFY inheritEventsChanged)
    Q_PROPERTY(bool isVisible READ isVisible WRITE setIsVisible NOTIFY isVisibleChanged)
	
    Q_CLASSINFO("DefaultProperty", "data")
public:
    Item3d(QObject *parent = 0);
    ~Item3d();

    enum CullFace
    {
        CullDisabled                = 0,
        CullFrontFaces              = 0x0404, // GL_FRONT
        CullBackFaces               = 0x0405, // GL_BACK
        CullAllFaces                = 0x0408, // GL_FRONT_AND_BACK
        CullClockwise               = 0x10000
    };
    Q_DECLARE_FLAGS(CullFaces, CullFace)

    QVector3D position() const;
    void setPosition(const QVector3D& value);

    qreal x() const;
    void setX(qreal value);
    qreal y() const;
    void setY(qreal value);
    qreal z() const;
    void setZ(qreal value);

    qreal scale() const;
    void setScale(qreal value);

    QmlList<QGraphicsTransform *> *transform();

    Mesh *mesh() const;
    void setMesh(Mesh* value);

    bool inheritEvents() const;
    void setInheritEvents(bool inherit);

    Effect *effect() const;
    void setEffect(Effect *value);

    QmlList<Item3d *> *children();
    QmlList<QObject *> *resources();
    QmlList<QObject *> *data();

    QmlList<QmlState *>* states();
    QmlState *findState(const QString &name) const;

    QmlList<QmlTransition *>* transitions();

    QString state() const;
    void setState(const QString &);

    CullFaces cullFaces() const;
    void setCullFaces(CullFaces value);

    QString name() const;
    void setName(QString nameString);

    QString meshNode() const;					
    void setMeshNode(const QString &);			

    QGLSceneNode * meshObject();
    void setMeshObject(QGLSceneNode *object);

    bool isVisible() const;
    void setIsVisible(bool visibility);

    bool isInitialized() const;
    void setIsInitialized();

    int mainBranchId() const;
    void setMainBranchId(int objectID);

    virtual void draw(QGLPainter *painter);
    virtual void initialize(Viewport *viewport, QGLPainter *painter);

    QGLSceneObject *getSceneObject(QGLSceneObject::Type type, const QString& name) const;

    void componentComplete();   

public Q_SLOTS:
    void update();

protected:
    virtual void drawItem(QGLPainter *painter);
    bool event(QEvent *e);

Q_SIGNALS:
    void positionChanged();
    void scaleChanged();
    void rotationChanged();
    void meshChanged();
    void meshNodeChanged();
    void effectChanged();
    void stateChanged(const QString &);
    void clicked();
    void doubleClicked();
    void pressed();
    void released();
    void hoverEnter();
    void hoverLeave();
    void nameChanged();
    void inheritEventsChanged();
    void isVisibleChanged();

private:
    Item3dPrivate *d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Item3d::CullFaces)

QML_DECLARE_TYPE(Item3d)

QT_END_NAMESPACE

QT_END_HEADER

#endif
