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

#ifndef QGLBEZIERPATCHES_H
#define QGLBEZIERPATCHES_H

#include "qvector2darray.h"
#include "qvector3darray.h"
#include <QtCore/qscopedpointer.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLBezierPatchesPrivate;
class QGLBuilder;

class Q_QT3D_EXPORT QGLBezierPatches
{
public:
    QGLBezierPatches();
    QGLBezierPatches(const QGLBezierPatches &other);
    virtual ~QGLBezierPatches();

    QGLBezierPatches &operator=(const QGLBezierPatches &other);

    QVector3DArray positions() const;
    void setPositions(const QVector3DArray &positions);

    QVector2DArray textureCoords() const;
    void setTextureCoords(const QVector2DArray &textureCoords);

    int subdivisionDepth() const;
    void setSubdivisionDepth(int value);

    bool compactSubdivision() const;
    void setCompactSubdivision(bool value);

private:
    QScopedPointer<QGLBezierPatchesPrivate> d_ptr;

    Q_DECLARE_PRIVATE(QGLBezierPatches)

    friend Q_QT3D_EXPORT QGLBuilder &operator<<(QGLBuilder &list, const QGLBezierPatches &patches);
};

Q_QT3D_EXPORT QGLBuilder &operator<<(QGLBuilder &list, const QGLBezierPatches &patches);

QT_END_NAMESPACE

QT_END_HEADER

#endif