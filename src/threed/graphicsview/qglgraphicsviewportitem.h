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

#ifndef QGLGRAPHICSVIEWPORTITEM_H
#define QGLGRAPHICSVIEWPORTITEM_H

#include "qglnamespace.h"
#include <QtGui/qgraphicsitem.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLGraphicsViewportItemPrivate;
class QGLPainter;
class QGLCamera;

class Q_QT3D_EXPORT QGLGraphicsViewportItem : public QGraphicsItem
{
public:
    QGLGraphicsViewportItem(QGraphicsItem *parent = 0);
    QGLGraphicsViewportItem(const QRectF& rect, QGraphicsItem *parent = 0);
    QGLGraphicsViewportItem(qreal x, qreal y, qreal w, qreal h,
                            QGraphicsItem *parent = 0);
    ~QGLGraphicsViewportItem();

    enum Option
    {
        ObjectPicking       = 0x0001,
        ShowPicking         = 0x0002,
        CameraNavigation    = 0x0004
    };
    Q_DECLARE_FLAGS(Options, Option)

    QGLGraphicsViewportItem::Options options() const;
    void setOptions(QGLGraphicsViewportItem::Options value);
    void setOption(QGLGraphicsViewportItem::Option option, bool value);

    QRectF rect() const;
    void setRect(const QRectF &rect);
    inline void setRect(qreal x, qreal y, qreal w, qreal h);

    QRectF boundingRect() const;

    QGLCamera *camera() const;
    void setCamera(QGLCamera *camera);

    bool clearDepthBuffer() const;
    void setClearDepthBuffer(bool value);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor& color);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    virtual QObject *objectForPosition(const QPointF& pos) const;

protected:
    virtual void paintGL(QGLPainter *painter) = 0;

private:
    QScopedPointer<QGLGraphicsViewportItemPrivate> d_ptr;

    Q_DECLARE_PRIVATE(QGLGraphicsViewportItem)
    Q_DISABLE_COPY(QGLGraphicsViewportItem)
};

void QGLGraphicsViewportItem::setRect(qreal x, qreal y, qreal w, qreal h)
{
    setRect(QRectF(x, y, w, h));
}

Q_DECLARE_OPERATORS_FOR_FLAGS(QGLGraphicsViewportItem::Options)

QT_END_NAMESPACE

QT_END_HEADER

#endif