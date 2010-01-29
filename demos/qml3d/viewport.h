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

#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QtDeclarative/qmlgraphicsitem.h>
#include "item3d.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class ViewportPrivate;
class QGLCamera;
class QGLLightModel;
class QGLLightParameters;
class Effect;

class Viewport : public QmlGraphicsItem
{
    Q_OBJECT
    Q_PROPERTY(bool picking READ picking WRITE setPicking)
    Q_PROPERTY(bool showPicking READ showPicking WRITE setShowPicking)
    Q_PROPERTY(bool navigation READ navigation WRITE setNavigation)
    Q_PROPERTY(QGLCamera *camera READ camera WRITE setCamera)
    Q_PROPERTY(QGLLightModel *lightModel READ lightModel WRITE setLightModel)
    Q_PROPERTY(Effect *backdrop READ backdrop WRITE setBackdrop)
public:
    Viewport(QmlGraphicsItem *parent = 0);
    ~Viewport();

    bool picking() const;
    void setPicking(bool value);

    bool showPicking() const;
    void setShowPicking(bool value);

    bool navigation() const;
    void setNavigation(bool value);

    QGLCamera *camera() const;
    void setCamera(QGLCamera *value);

    QGLLightModel *lightModel() const;
    void setLightModel(QGLLightModel *value);

    Effect *backdrop() const;
    void setBackdrop(Effect *value);

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

    void earlyDraw(QGLPainter *painter);
    void draw(QGLPainter *painter);
    void initialize(Qml3dView *view, QGLPainter *painter);

    Qml3dView *view() const;

public Q_SLOTS:
    void update3d();

private Q_SLOTS:
    void cameraChanged();

private:
    ViewportPrivate *d;
};

QML_DECLARE_TYPE(Viewport)
QML_DECLARE_TYPE(QGLLightModel)
QML_DECLARE_TYPE(QGLLightParameters)
QML_DECLARE_TYPE(QGLCamera)

QT_END_NAMESPACE

QT_END_HEADER

#endif
