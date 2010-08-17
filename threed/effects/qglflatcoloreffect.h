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

#ifndef QGLFLATCOLOREFFECT_H
#define QGLFLATCOLOREFFECT_H

#include "qglabstracteffect.h"
#include <QtCore/qscopedpointer.h>

QT_BEGIN_NAMESPACE

class QGLFlatColorEffectPrivate;
class QGLPerVertexColorEffectPrivate;

class Q_QT3D_EXPORT QGLFlatColorEffect : public QGLAbstractEffect
{
public:
    QGLFlatColorEffect();
    virtual ~QGLFlatColorEffect();

    QList<QGL::VertexAttribute> requiredFields() const;
    bool supportsPicking() const;
    void setActive(QGLPainter *painter, bool flag);
    void update(QGLPainter *painter, QGLPainter::Updates updates);

    void setVertexAttribute
        (QGL::VertexAttribute attribute, const QGLAttributeValue& value);

private:
    QScopedPointer<QGLFlatColorEffectPrivate> d_ptr;

    Q_DECLARE_PRIVATE(QGLFlatColorEffect)
    Q_DISABLE_COPY(QGLFlatColorEffect)
};

class Q_QT3D_EXPORT QGLPerVertexColorEffect : public QGLAbstractEffect
{
public:
    QGLPerVertexColorEffect();
    virtual ~QGLPerVertexColorEffect();

    QList<QGL::VertexAttribute> requiredFields() const;
    void setActive(QGLPainter *painter, bool flag);
    void update(QGLPainter *painter, QGLPainter::Updates updates);

    void setVertexAttribute
        (QGL::VertexAttribute attribute, const QGLAttributeValue& value);

private:
    QScopedPointer<QGLPerVertexColorEffectPrivate> d_ptr;

    Q_DECLARE_PRIVATE(QGLPerVertexColorEffect)
    Q_DISABLE_COPY(QGLPerVertexColorEffect)
};

QT_END_NAMESPACE

#endif