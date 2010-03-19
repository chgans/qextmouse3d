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

#ifndef QGLVIEWPORT_H
#define QGLVIEWPORT_H

#include "qglpainter.h"
#include "qglcamera.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLViewportPrivate;

class Q_QT3D_EXPORT QGLViewport : public QObject
{
    Q_OBJECT
    Q_ENUMS(Option)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY viewportChanged)
    Q_PROPERTY(QRect rect READ rect WRITE setRect NOTIFY viewportChanged)
    Q_PROPERTY(int layer READ layer WRITE setLayer NOTIFY viewportChanged)
    Q_PROPERTY(QGLCamera *camera READ camera WRITE setCamera NOTIFY viewportChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY viewportChanged)
public:
    explicit QGLViewport(QObject *parent = 0);
    ~QGLViewport();

    bool isEnabled() const;
    void setEnabled(bool enable);

    QRect rect() const;
    void setRect(const QRect &rect);

    int layer() const;
    void setLayer(int layer);

    QGLCamera *camera() const;
    void setCamera(QGLCamera *camera);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor& color);

    enum Eye
    {
        NoEye,
        LeftEye,
        RightEye
    };

    virtual void prepareGL(QGLPainter *painter, QGLViewport::Eye eye,
                           QPaintDevice *window, const QRect &windowRect);
    virtual void paintGL(QGLPainter *painter) = 0;
    virtual bool needsPickUpdate();

public Q_SLOTS:
    void update();

Q_SIGNALS:
    void viewportChanged();

private:
    QScopedPointer<QGLViewportPrivate> d_ptr;

    Q_DECLARE_PRIVATE(QGLViewport)
    Q_DISABLE_COPY(QGLViewport)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
