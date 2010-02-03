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

#ifndef QVECTORARRAY_H
#define QVECTORARRAY_H

#include "qarray.h"
#include <QtGui/qvector2d.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>
#include <QtGui/qtransform.h>
#include <QtGui/qmatrix4x4.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class Q_QT3D_EXPORT QVector2DArray : public QArray<QVector2D>
{
public:
    QVector2DArray();
    QVector2DArray(int size, const QVector2D& value = QVector2D());
    QVector2DArray(const QArray<QVector2D>& other);

    void append(qreal x, qreal y);
    void append(const QPointF& point);
    void append(const QPoint& point);

    void scale(qreal s);
    QVector2DArray scaled(qreal s) const;

    void translate(const QVector2D& value);
    void translate(qreal x, qreal y);
    void translate(const QPointF& point);
    void translate(const QPoint& point);

    QArray<QVector2D> translated(const QVector2D& value) const;
    QArray<QVector2D> translated(qreal x, qreal y) const;
    QArray<QVector2D> translated(const QPointF& point) const;
    QArray<QVector2D> translated(const QPoint& point) const;

    void transform(const QTransform& matrix);
    void transform(const QMatrix4x4& matrix);

    QArray<QVector2D> transformed(const QTransform& matrix) const;
    QArray<QVector2D> transformed(const QMatrix4x4& matrix) const;

#if !defined(Q_NO_USING_KEYWORD)
    using QArray<QVector2D>::append;
#else
    inline void append(const QVector2D& value)
        { QArray<QVector2D>::append(value); }
    inline void append(const QVector2D& value1, const QVector2D& value2)
        { QArray<QVector2D>::append(value1, value2); }
    inline void append(const QVector2D& value1, const QVector2D& value2, const QVector2D& value3)
        { QArray<QVector2D>::append(value1, value2, value3); }
    inline void append(const QVector2D& value1, const QVector2D& value2, const QVector2D& value3, const QVector2D& value4)
        { QArray<QVector2D>::append(value1, value2, value3, value4); }
    inline void append(const QVector2D *values, int count)
        { QArray<QVector2D>::append(values, count); }
    inline void append(const QArray<QVector2D>& other)
        { QArray<QVector2D>::append(other); }
#endif
};

class Q_QT3D_EXPORT QVector3DArray : public QArray<QVector3D>
{
public:
    QVector3DArray();
    QVector3DArray(int size, const QVector3D& value = QVector3D());
    QVector3DArray(const QArray<QVector3D>& other);

    void append(qreal x, qreal y, qreal z);

    void scale(qreal s);
    QVector3DArray scaled(qreal s) const;

    void translate(const QVector3D& value);
    void translate(qreal x, qreal y, qreal z);

    QArray<QVector3D> translated(const QVector3D& value) const;
    QArray<QVector3D> translated(qreal x, qreal y, qreal z) const;

    void transform(const QMatrix4x4& matrix);
    QArray<QVector3D> transformed(const QMatrix4x4& matrix) const;

#if !defined(Q_NO_USING_KEYWORD)
    using QArray<QVector3D>::append;
#else
    inline void append(const QVector3D& value)
        { QArray<QVector3D>::append(value); }
    inline void append(const QVector3D& value1, const QVector3D& value2)
        { QArray<QVector3D>::append(value1, value2); }
    inline void append(const QVector3D& value1, const QVector3D& value2, const QVector3D& value3)
        { QArray<QVector3D>::append(value1, value2, value3); }
    inline void append(const QVector3D& value1, const QVector3D& value2, const QVector3D& value3, const QVector3D& value4)
        { QArray<QVector3D>::append(value1, value2, value3, value4); }
    inline void append(const QVector3D *values, int count)
        { QArray<QVector3D>::append(values, count); }
    inline void append(const QArray<QVector3D>& other)
        { QArray<QVector3D>::append(other); }
#endif
};

class Q_QT3D_EXPORT QVector4DArray : public QArray<QVector4D>
{
public:
    QVector4DArray();
    QVector4DArray(int size, const QVector4D& value = QVector4D());
    QVector4DArray(const QArray<QVector4D>& other);

    void append(qreal x, qreal y, qreal z, qreal w);

    void scale(qreal s);
    QVector4DArray scaled(qreal s) const;

    void translate(const QVector4D& value);
    void translate(qreal x, qreal y, qreal z, qreal w);

    QArray<QVector4D> translated(const QVector4D& value) const;
    QArray<QVector4D> translated
        (qreal x, qreal y, qreal z, qreal w) const;

    void transform(const QMatrix4x4& matrix);
    QArray<QVector4D> transformed(const QMatrix4x4& matrix) const;

#if !defined(Q_NO_USING_KEYWORD)
    using QArray<QVector4D>::append;
#else
    inline void append(const QVector4D& value)
        { QArray<QVector4D>::append(value); }
    inline void append(const QVector4D& value1, const QVector4D& value2)
        { QArray<QVector4D>::append(value1, value2); }
    inline void append(const QVector4D& value1, const QVector4D& value2, const QVector4D& value3)
        { QArray<QVector4D>::append(value1, value2, value3); }
    inline void append(const QVector4D& value1, const QVector4D& value2, const QVector4D& value3, const QVector4D& value4)
        { QArray<QVector4D>::append(value1, value2, value3, value4); }
    inline void append(const QVector4D *values, int count)
        { QArray<QVector4D>::append(values, count); }
    inline void append(const QArray<QVector4D>& other)
        { QArray<QVector4D>::append(other); }
#endif
};

inline QVector2DArray::QVector2DArray() {}

inline QVector2DArray::QVector2DArray(int size, const QVector2D& value)
    : QArray<QVector2D>(size, value) {}

inline QVector2DArray::QVector2DArray(const QArray<QVector2D>& other)
    : QArray<QVector2D>(other) {}

inline void QVector2DArray::append(qreal x, qreal y)
    { QArray<QVector2D>::append(QVector2D(x, y)); }

inline void QVector2DArray::append(const QPointF& point)
    { QArray<QVector2D>::append(QVector2D(point)); }

inline void QVector2DArray::append(const QPoint& point)
    { QArray<QVector2D>::append(QVector2D(point)); }

inline void QVector2DArray::translate(qreal x, qreal y)
    { translate(QVector2D(x, y)); }

inline void QVector2DArray::translate(const QPointF& value)
    { translate(QVector2D(value)); }

inline void QVector2DArray::translate(const QPoint& value)
    { translate(QVector2D(value)); }

inline QArray<QVector2D> QVector2DArray::translated(qreal x, qreal y) const
    { return translated(QVector2D(x, y)); }

inline QArray<QVector2D> QVector2DArray::translated(const QPointF& point) const
    { return translated(QVector2D(point)); }

inline QArray<QVector2D> QVector2DArray::translated(const QPoint& point) const
    { return translated(QVector2D(point)); }

inline QVector3DArray::QVector3DArray() {}

inline QVector3DArray::QVector3DArray(int size, const QVector3D& value)
    : QArray<QVector3D>(size, value) {}

inline QVector3DArray::QVector3DArray(const QArray<QVector3D>& other)
    : QArray<QVector3D>(other) {}

inline void QVector3DArray::append(qreal x, qreal y, qreal z)
    { QArray<QVector3D>::append(QVector3D(x, y, z)); }

inline void QVector3DArray::translate(qreal x, qreal y, qreal z)
    { translate(QVector3D(x, y, z)); }

inline QArray<QVector3D> QVector3DArray::translated(qreal x, qreal y, qreal z) const
    { return translated(QVector3D(x, y, z)); }

inline QVector4DArray::QVector4DArray() {}

inline QVector4DArray::QVector4DArray(int size, const QVector4D& value)
    : QArray<QVector4D>(size, value) {}

inline QVector4DArray::QVector4DArray(const QArray<QVector4D>& other)
    : QArray<QVector4D>(other) {}

inline void QVector4DArray::append(qreal x, qreal y, qreal z, qreal w)
    { QArray<QVector4D>::append(QVector4D(x, y, z, w)); }

inline void QVector4DArray::translate(qreal x, qreal y, qreal z, qreal w)
    { translate(QVector4D(x, y, z, w)); }

inline QArray<QVector4D> QVector4DArray::translated
        (qreal x, qreal y, qreal z, qreal w) const
    { return translated(QVector4D(x, y, z, w)); }

#ifndef QT_NO_DEBUG_STREAM
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QVector2DArray &ary);
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QVector3DArray &ary);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif
