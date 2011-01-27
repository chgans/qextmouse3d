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

#ifndef VIEWPORT_SG_H
#define VIEWPORT_SG_H

#include <QtDeclarative/qdeclarative.h>

#if defined(QML_VERSION) && QML_VERSION >= 0x020000

#include <QtDeclarative/qsgitem.h>
#include "qdeclarativeitem3d.h"
#include "qdeclarativeviewport_p.h"
#include "qglnamespace.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class ViewportSGPrivate;
class QGLCamera;
class QGLLightModel;
class QGLLightParameters;
class QDeclarativeEffect;
class QGLAbstractSurface;

class ViewportSG : public QSGItem, public QDeclarativeViewport
{
    Q_OBJECT
    Q_PROPERTY(bool picking READ picking WRITE setPicking NOTIFY viewportChanged)
    Q_PROPERTY(bool showPicking READ showPicking WRITE setShowPicking NOTIFY viewportChanged)
    Q_PROPERTY(bool navigation READ navigation WRITE setNavigation NOTIFY viewportChanged)
    Q_PROPERTY(bool blending READ blending WRITE setBlending NOTIFY viewportChanged)
    Q_PROPERTY(QGLCamera *camera READ camera WRITE setCamera)
    Q_PROPERTY(QGLLightParameters *light READ light WRITE setLight NOTIFY viewportChanged)
    Q_PROPERTY(QGLLightModel *lightModel READ lightModel WRITE setLightModel NOTIFY viewportChanged)
public:
    ViewportSG(QSGItem *parent = 0);
    ~ViewportSG();

    bool picking() const;
    void setPicking(bool value);

    bool showPicking() const;
    void setShowPicking(bool value);

    bool navigation() const;
    void setNavigation(bool value);

    bool blending() const;
    void setBlending(bool value);

    QGLCamera *camera() const;
    void setCamera(QGLCamera *value);

    QGLLightParameters *light() const;
    void setLight(QGLLightParameters *value);

    QGLLightModel *lightModel() const;
    void setLightModel(QGLLightModel *value);

    bool isDirty() const;
    void paint(QGLAbstractSurface *surface, QGL::Eye eye);

    int registerPickableObject(QObject *obj);

    Q_INVOKABLE QObject *objectForPoint(qreal x, qreal y);

Q_SIGNALS:
    void viewportChanged();

public Q_SLOTS:
    void update3d();

private Q_SLOTS:
    void cameraChanged();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

    Node *updatePaintNode(Node *, UpdatePaintNodeData *);

private:
    ViewportSGPrivate *d;

    void earlyDraw(QGLPainter *painter);
    void draw(QGLPainter *painter);
    void initializeGL(QGLPainter *painter);

    bool hoverEvent(QGraphicsSceneHoverEvent *event);

    QObject *objectForPoint(const QPointF &pos)
    {
        return objectForPoint(pos.x(), pos.y());
    }

    void wheel(qreal delta);
    void pan(qreal deltax, qreal deltay);
    void rotate(qreal deltax, qreal deltay);
    QPointF viewDelta(qreal deltax, qreal deltay);

    bool isViewportRoot() const;
};

QML_DECLARE_TYPE(ViewportSG)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QML_VERSION >= 0x020000

#endif
