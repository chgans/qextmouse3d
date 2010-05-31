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

#ifndef QGLMATRIXSTACK_H
#define QGLMATRIXSTACK_H

#include "qglnamespace.h"
#include <QtGui/qmatrix4x4.h>
#include <QtCore/qscopedpointer.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLMatrixStackPrivate;

class Q_QT3D_EXPORT QGLMatrixStack
{
public:
    QGLMatrixStack();
    ~QGLMatrixStack();

    enum Type
    {
        ProjectionMatrix,
        ModelViewMatrix,
        UserMatrix
    };

    QGLMatrixStack::Type type() const;

    const QMatrix4x4 &top() const;

    void push();
    void pop();

    void setToIdentity();

    void ortho(const QRect& rect, qreal nearPlane = -1.0f, qreal farPlane = 1.0f);

    void translate(qreal x, qreal y, qreal z);
    void translate(const QVector3D& vector);
    void scale(qreal x, qreal y, qreal z);
    void scale(qreal factor);
    void scale(const QVector3D& vector);
    void rotate(qreal angle, qreal x, qreal y, qreal z);
    void rotate(qreal angle, const QVector3D& vector);
    void rotate(const QQuaternion &quaternion);

    QGLMatrixStack& operator=(const QMatrix4x4& matrix);
    QGLMatrixStack& operator*=(const QMatrix4x4& matrix);

    operator const QMatrix4x4 &() const;

    static QMatrix4x4 readServerMatrix(QGLMatrixStack::Type type);

private:
    Q_DISABLE_COPY(QGLMatrixStack)
    Q_DECLARE_PRIVATE(QGLMatrixStack)

    QGLMatrixStack(QGLMatrixStack::Type type);

    QScopedPointer<QGLMatrixStackPrivate> d_ptr;

    friend class QGLPainterPrivate;
    friend class QGLPainter;

    void markDirty();
    bool needsUpdate();
    void updateServer();
};

inline QGLMatrixStack::operator const QMatrix4x4 &() const
{
    return top();
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
