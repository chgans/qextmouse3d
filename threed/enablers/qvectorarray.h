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

#ifndef QVECTORARRAY_H
#define QVECTORARRAY_H

#include "qdataarray.h"
#include <QtGui/qvector2d.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class Q_QT3D_EXPORT QVector2DArray : public QDataArray<QVector2D>
{
public:
    QVector2DArray();
    QVector2DArray(int size, const QVector2D& value = QVector2D());
    QVector2DArray(const QDataArray<QVector2D>& other);

    void append(const QVector2D& value);
    void append(qreal x, qreal y);
};

class Q_QT3D_EXPORT QVector3DArray : public QDataArray<QVector3D>
{
public:
    QVector3DArray();
    QVector3DArray(int size, const QVector3D& value = QVector3D());
    QVector3DArray(const QDataArray<QVector3D>& other);

    void append(const QVector3D& value);
    void append(qreal x, qreal y, qreal z);
};

class Q_QT3D_EXPORT QVector4DArray : public QDataArray<QVector4D>
{
public:
    QVector4DArray();
    QVector4DArray(int size, const QVector4D& value = QVector4D());
    QVector4DArray(const QDataArray<QVector4D>& other);

    void append(const QVector2D& value);
    void append(qreal x, qreal y, qreal z, qreal w);
};

inline QVector2DArray::QVector2DArray() {}

inline QVector2DArray::QVector2DArray(int size, const QVector2D& value)
    : QDataArray<QVector2D>(size, value) {}

inline QVector2DArray::QVector2DArray(const QDataArray<QVector2D>& other)
    : QDataArray<QVector2D>(other) {}

inline void QVector2DArray::append(const QVector2D& value)
    { QDataArray<QVector2D>::append(value); }

inline void QVector2DArray::append(qreal x, qreal y)
    { QDataArray<QVector2D>::append(QVector2D(x, y)); }

inline QVector3DArray::QVector3DArray() {}

inline QVector3DArray::QVector3DArray(int size, const QVector3D& value)
    : QDataArray<QVector3D>(size, value) {}

inline QVector3DArray::QVector3DArray(const QDataArray<QVector3D>& other)
    : QDataArray<QVector3D>(other) {}

inline void QVector3DArray::append(const QVector3D& value)
    { QDataArray<QVector3D>::append(value); }

inline void QVector3DArray::append(qreal x, qreal y, qreal z)
    { QDataArray<QVector3D>::append(QVector3D(x, y, z)); }

inline QVector4DArray::QVector4DArray() {}

inline QVector4DArray::QVector4DArray(int size, const QVector4D& value)
    : QDataArray<QVector4D>(size, value) {}

inline QVector4DArray::QVector4DArray(const QDataArray<QVector4D>& other)
    : QDataArray<QVector4D>(other) {}

inline void QVector4DArray::append(const QVector2D& value)
    { QDataArray<QVector4D>::append(value); }

inline void QVector4DArray::append(qreal x, qreal y, qreal z, qreal w)
    { QDataArray<QVector4D>::append(QVector4D(x, y, z, w)); }

QT_END_NAMESPACE

QT_END_HEADER

#endif
