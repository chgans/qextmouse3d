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

#ifndef QGLCOLORMATERIAL_H
#define QGLCOLORMATERIAL_H

#include "qglabstractmaterial.h"
#include <QtGui/qcolor.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLColorMaterialPrivate;

class Q_QT3D_EXPORT QGLColorMaterial : public QGLAbstractMaterial
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGLColorMaterial)
    Q_DISABLE_COPY(QGLColorMaterial)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
public:
    explicit QGLColorMaterial(QObject *parent = 0);
    ~QGLColorMaterial();

    QColor color() const;
    void setColor(const QColor &color);

    void bind(QGLPainter *painter);
    void release(QGLPainter *painter, QGLAbstractMaterial *next);
    int compare(const QGLAbstractMaterial *other) const;

Q_SIGNALS:
    void colorChanged();

private:
    QScopedPointer<QGLColorMaterialPrivate> d_ptr;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
