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

#ifndef SUBITEM3D_H
#define SUBITEM3D_H

#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qgraphicstransform.h>
#include <QtDeclarative/qml.h>
#include "qglpainter.h"
#include "mesh.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class SubItem3dPrivate;
class QGLSceneNode;

class SubItem3d : public QObject
{
    Q_OBJECT    
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY positionChanged)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY positionChanged)
    Q_PROPERTY(qreal z READ z WRITE setZ NOTIFY positionChanged)
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QString meshNode READ meshNode WRITE setMeshNode NOTIFY meshNodeChanged)
public:
	
    SubItem3d(QObject *parent = 0);
    ~SubItem3d();
    
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

    QString meshNode() const;					
    void setMeshNode(const QString &);			

    QGLSceneNode * meshObject();
    void setMeshObject(QGLSceneNode *object);

    void performTransform();

public Q_SLOTS:
    void transform();
    void update();

protected:

Q_SIGNALS:
    void positionChanged();
    void scaleChanged();
    void meshNodeChanged();
private:
    SubItem3dPrivate *d;

};

QML_DECLARE_TYPE(SubItem3d)

QT_END_NAMESPACE

QT_END_HEADER

#endif
