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
#include <QtGui/qtransform.h>
#include <QtGui/qmatrix4x4.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class Q_QT3D_EXPORT QVector2DArray : public QDataArray<QVector2D>
{
public:
    QVector2DArray();
    QVector2DArray(int size, const QVector2D& value = QVector2D());
    QVector2DArray(const QDataArray<QVector2D>& other);

    void append(qreal x, qreal y);
    void append(const QPointF& point);
    void append(const QPoint& point);

    void translate(const QVector2D& value);
    void translate(qreal x, qreal y);
    void translate(const QPointF& point);
    void translate(const QPoint& point);

    QDataArray<QVector2D> translated(const QVector2D& value) const;
    QDataArray<QVector2D> translated(qreal x, qreal y) const;
    QDataArray<QVector2D> translated(const QPointF& point) const;
    QDataArray<QVector2D> translated(const QPoint& point) const;

    void transform(const QTransform& matrix);
    void transform(const QMatrix4x4& matrix);

    QDataArray<QVector2D> transformed(const QTransform& matrix) const;
    QDataArray<QVector2D> transformed(const QMatrix4x4& matrix) const;

#if !defined(Q_NO_USING_KEYWORD)
    using QDataArray<QVector2D>::append;
#else
    inline void append(const QVector2D& value)
        { QDataArray<QVector2D>::append(value); }
    inline void append(const QVector2D& value1, const QVector2D& value2)
        { QDataArray<QVector2D>::append(value1, value2); }
    inline void append(const QVector2D& value1, const QVector2D& value2, const QVector2D& value3)
        { QDataArray<QVector2D>::append(value1, value2, value3); }
    inline void append(const QVector2D& value1, const QVector2D& value2, const QVector2D& value3, const QVector2D& value4)
        { QDataArray<QVector2D>::append(value1, value2, value3, value4); }
    inline void append(const QVector2D *values, int count)
        { QDataArray<QVector2D>::append(values, count); }
    inline void append(const QDataArray<QVector2D>& other)
        { QDataArray<QVector2D>::append(other); }
#endif
};

class Q_QT3D_EXPORT QVector3DArray : public QDataArray<QVector3D>
{
public:
    QVector3DArray();
    QVector3DArray(int size, const QVector3D& value = QVector3D());
    QVector3DArray(const QDataArray<QVector3D>& other);

    void append(qreal x, qreal y, qreal z);

    void translate(const QVector3D& value);
    void translate(qreal x, qreal y, qreal z);

    QDataArray<QVector3D> translated(const QVector3D& value) const;
    QDataArray<QVector3D> translated(qreal x, qreal y, qreal z) const;

    void transform(const QMatrix4x4& matrix);
    QDataArray<QVector3D> transformed(const QMatrix4x4& matrix) const;

#if !defined(Q_NO_USING_KEYWORD)
    using QDataArray<QVector3D>::append;
#else
    inline void append(const QVector3D& value)
        { QDataArray<QVector3D>::append(value); }
    inline void append(const QVector3D& value1, const QVector3D& value2)
        { QDataArray<QVector3D>::append(value1, value2); }
    inline void append(const QVector3D& value1, const QVector3D& value2, const QVector3D& value3)
        { QDataArray<QVector3D>::append(value1, value2, value3); }
    inline void append(const QVector3D& value1, const QVector3D& value2, const QVector3D& value3, const QVector3D& value4)
        { QDataArray<QVector3D>::append(value1, value2, value3, value4); }
    inline void append(const QVector3D *values, int count)
        { QDataArray<QVector3D>::append(values, count); }
    inline void append(const QDataArray<QVector3D>& other)
        { QDataArray<QVector3D>::append(other); }
#endif
};

class Q_QT3D_EXPORT QVector4DArray : public QDataArray<QVector4D>
{
public:
    QVector4DArray();
    QVector4DArray(int size, const QVector4D& value = QVector4D());
    QVector4DArray(const QDataArray<QVector4D>& other);

    void append(qreal x, qreal y, qreal z, qreal w);

    void translate(const QVector4D& value);
    void translate(qreal x, qreal y, qreal z, qreal w);

    QDataArray<QVector4D> translated(const QVector4D& value) const;
    QDataArray<QVector4D> translated
        (qreal x, qreal y, qreal z, qreal w) const;

    void transform(const QMatrix4x4& matrix);
    QDataArray<QVector4D> transformed(const QMatrix4x4& matrix) const;

#if !defined(Q_NO_USING_KEYWORD)
    using QDataArray<QVector4D>::append;
#else
    inline void append(const QVector4D& value)
        { QDataArray<QVector4D>::append(value); }
    inline void append(const QVector4D& value1, const QVector4D& value2)
        { QDataArray<QVector4D>::append(value1, value2); }
    inline void append(const QVector4D& value1, const QVector4D& value2, const QVector4D& value3)
        { QDataArray<QVector4D>::append(value1, value2, value3); }
    inline void append(const QVector4D& value1, const QVector4D& value2, const QVector4D& value3, const QVector4D& value4)
        { QDataArray<QVector4D>::append(value1, value2, value3, value4); }
    inline void append(const QVector4D *values, int count)
        { QDataArray<QVector4D>::append(values, count); }
    inline void append(const QDataArray<QVector4D>& other)
        { QDataArray<QVector4D>::append(other); }
#endif
};

inline QVector2DArray::QVector2DArray() {}

inline QVector2DArray::QVector2DArray(int size, const QVector2D& value)
    : QDataArray<QVector2D>(size, value) {}

inline QVector2DArray::QVector2DArray(const QDataArray<QVector2D>& other)
    : QDataArray<QVector2D>(other) {}

inline void QVector2DArray::append(qreal x, qreal y)
    { QDataArray<QVector2D>::append(QVector2D(x, y)); }

inline void QVector2DArray::append(const QPointF& point)
    { QDataArray<QVector2D>::append(QVector2D(point)); }

inline void QVector2DArray::append(const QPoint& point)
    { QDataArray<QVector2D>::append(QVector2D(point)); }

inline void QVector2DArray::translate(qreal x, qreal y)
    { translate(QVector2D(x, y)); }

inline void QVector2DArray::translate(const QPointF& value)
    { translate(QVector2D(value)); }

inline void QVector2DArray::translate(const QPoint& value)
    { translate(QVector2D(value)); }

inline QDataArray<QVector2D> QVector2DArray::translated(qreal x, qreal y) const
    { return translated(QVector2D(x, y)); }

inline QDataArray<QVector2D> QVector2DArray::translated(const QPointF& point) const
    { return translated(QVector2D(point)); }

inline QDataArray<QVector2D> QVector2DArray::translated(const QPoint& point) const
    { return translated(QVector2D(point)); }

inline QVector3DArray::QVector3DArray() {}

inline QVector3DArray::QVector3DArray(int size, const QVector3D& value)
    : QDataArray<QVector3D>(size, value) {}

inline QVector3DArray::QVector3DArray(const QDataArray<QVector3D>& other)
    : QDataArray<QVector3D>(other) {}

inline void QVector3DArray::append(qreal x, qreal y, qreal z)
    { QDataArray<QVector3D>::append(QVector3D(x, y, z)); }

inline void QVector3DArray::translate(qreal x, qreal y, qreal z)
    { translate(QVector3D(x, y, z)); }

inline QDataArray<QVector3D> QVector3DArray::translated(qreal x, qreal y, qreal z) const
    { return translated(QVector3D(x, y, z)); }

inline QVector4DArray::QVector4DArray() {}

inline QVector4DArray::QVector4DArray(int size, const QVector4D& value)
    : QDataArray<QVector4D>(size, value) {}

inline QVector4DArray::QVector4DArray(const QDataArray<QVector4D>& other)
    : QDataArray<QVector4D>(other) {}

inline void QVector4DArray::append(qreal x, qreal y, qreal z, qreal w)
    { QDataArray<QVector4D>::append(QVector4D(x, y, z, w)); }

inline void QVector4DArray::translate(qreal x, qreal y, qreal z, qreal w)
    { translate(QVector4D(x, y, z, w)); }

inline QDataArray<QVector4D> QVector4DArray::translated
        (qreal x, qreal y, qreal z, qreal w) const
    { return translated(QVector4D(x, y, z, w)); }

QT_END_NAMESPACE

QT_END_HEADER

#endif
