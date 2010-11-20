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

#ifndef QDECLARATIVEITEM3D_H
#define QDECLARATIVEITEM3D_H

#include "qt3dquickglobal.h"

#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>
#include <QtGui/qvector3d.h>
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/qdeclarativelist.h>
#include <QtDeclarative/qdeclarativeparserstatus.h>

#include "qglscenenode.h"
#include "qglpainter.h"
#include "qgraphicstransform3d.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QDeclarativeItem3DPrivate;
class QDeclarativeMesh;
class QDeclarativeEffect;
class QDeclarativeViewport;
class QDeclarativeState;
class QDeclarativeTransition;

class Q_QT3D_QUICK_EXPORT QDeclarativeItem3D : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_ENUMS(CullFace)
    Q_FLAGS(CullFaces)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY positionChanged)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY positionChanged)
    Q_PROPERTY(qreal z READ z WRITE setZ NOTIFY positionChanged)
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)   
    Q_PROPERTY(QDeclarativeListProperty<QGraphicsTransform3D> transform READ transform DESIGNABLE false FINAL)
    Q_PROPERTY(QDeclarativeListProperty<QGraphicsTransform3D> pretransform READ pretransform DESIGNABLE false FINAL)
    Q_PROPERTY(QDeclarativeMesh *mesh READ mesh WRITE setMesh NOTIFY meshChanged)
    Q_PROPERTY(QDeclarativeEffect *effect READ effect WRITE setEffect NOTIFY effectChanged)    
    Q_PROPERTY(QGLLightParameters *light READ light WRITE setLight NOTIFY lightChanged)    
    Q_PROPERTY(QDeclarativeListProperty<QObject> resources READ resources DESIGNABLE false)
    Q_PROPERTY(QDeclarativeListProperty<QObject> data READ data DESIGNABLE false)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeItem3D> children READ itemChildren DESIGNABLE false NOTIFY childrenChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeState> states READ states DESIGNABLE false)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeTransition> transitions READ transitions DESIGNABLE false)
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(CullFaces cullFaces READ cullFaces WRITE setCullFaces NOTIFY meshChanged)
    Q_PROPERTY(QString meshNode READ meshNode WRITE setMeshNode NOTIFY meshNodeChanged)
    Q_PROPERTY(bool inheritEvents READ inheritEvents WRITE setInheritEvents NOTIFY inheritEventsChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QDeclarativeItem3D *parent READ parentItem WRITE setParentItem NOTIFY parentChanged DESIGNABLE false FINAL)
    Q_CLASSINFO("DefaultProperty", "data")
public:
    QDeclarativeItem3D(QObject *parent = 0);
    ~QDeclarativeItem3D();

    QDeclarativeItem3D *parentItem() const;
    void setParentItem(QDeclarativeItem3D *parent);

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

    QDeclarativeMesh *mesh() const;
    void setMesh(QDeclarativeMesh* value);

    bool inheritEvents() const;
    void setInheritEvents(bool inherit);

    QDeclarativeEffect *effect() const;
    void setEffect(QDeclarativeEffect *value);

    QGLLightParameters *light() const;
    void setLight(QGLLightParameters *value);

    QDeclarativeListProperty<QObject> data();
    QDeclarativeListProperty<QObject> resources();
    QDeclarativeListProperty<QDeclarativeItem3D> itemChildren();

    QDeclarativeListProperty<QGraphicsTransform3D> transform();
    QDeclarativeListProperty<QGraphicsTransform3D> pretransform();

    QDeclarativeListProperty<QDeclarativeState> states();

    QDeclarativeListProperty<QDeclarativeTransition> transitions();
    
    QString state() const;
    void setState(const QString &);

    CullFaces cullFaces() const;
    void setCullFaces(CullFaces value);

    QString meshNode() const;					
    void setMeshNode(const QString &);			

    bool isEnabled() const;
    void setEnabled(bool value);

    virtual void draw(QGLPainter *painter);
    virtual void initialize(QGLPainter *painter);

    void classBegin();   
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
    void lightChanged();
    void stateChanged(const QString &);
    void clicked();
    void doubleClicked();
    void pressed();
    void released();
    void hoverEnter();
    void hoverLeave();
    void inheritEventsChanged();
    void enabledChanged();
    void childrenChanged();
    void parentChanged();

private:
    QDeclarativeItem3DPrivate *d;

    friend class QDeclarativeItem3DPrivate;
    friend class QDeclarativeViewport;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QDeclarativeItem3D::CullFaces)

QML_DECLARE_TYPE(QDeclarativeItem3D)

QT_END_NAMESPACE

QT_END_HEADER

#endif
