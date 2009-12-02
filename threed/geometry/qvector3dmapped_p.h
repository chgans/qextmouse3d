/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#ifndef QVECTOR3DMAPPED_P_H
#define QVECTOR3DMAPPED_P_H

#include <QtGui/qvector3d.h>
#include "qgldisplaylist.h"
#include "qglsection.h"
#include "qgeometrydata_p.h"

class QVector3DMapped
{
public:
    typedef QVector<QVector3DMapped> VList;

    QVector3DMapped(const QVector3D* vec);
    inline bool isNull() const;
    operator QVector3D () const;
    inline bool operator<(const QVector3DMapped &rhs) const;
    inline bool operator==(const QVector3D &rhs) const;
    inline bool findNormal(const QVector3D &norm,
                           const VList &list) const;
    inline void appendNormal(int, QGeometryData *, QVector3DMapped::VList &);

    QString toString() const {
        const QVector3D &v = vecPtr ? *vecPtr : store->vertices.at(vecIx);
        QString vs = QString("QVector3D( %1, %2, %3 )").arg(v.x()).arg(v.y()).arg(v.z());
        return vs + "\n" + QString("[QVector3DMapped - vecIx: %1 - vecPtr: %2 - normPtr: %3 - store %4]\n").arg(vecIx).arg((int)vecPtr).arg(normPtr).arg((int)store);
    }

    const QVector3D *vecPtr;
    int normPtr;
};

inline QVector3DMapped::QVector3DMapped(const QVector3D* vec)
    : vecPtr(vec)
    , normPtr(-1)
{
}

QVector3DMapped::operator QVector3D () const
{
    Q_ASSERT(!isNull());
    return *vecPtr;
}


inline bool QVector3DMapped::isNull() const
{
    return (vecPtr == NULL);
}

inline bool QVector3DMapped::operator<(const QVector3DMapped &rhs) const
{
    Q_ASSERT(!isNull());
    Q_ASSERT(!rhs.isNull());
    const QVector3D &a = *vecPtr;
    const QVector3D &b = *rhs.vecPtr;
    if (qFuzzyCompare(a.x(), b.x()))
    {
        if (qFuzzyCompare(a.y(), b.y()))
        {
            if (qFuzzyCompare(a.z(), b.z()))
            {
                return false;
            }
            else
            {
                return a.z() < b.z();
            }
        }
        else
        {
            return a.y() < b.y();
        }
    }
    else
    {
        return a.x() < b.x();
    }
}

inline bool QVector3DMapped::operator==(const QVector3D &rhs) const
{
    Q_ASSERT(!isNull());
    Q_ASSERT(!rhs.isNull());
    return qFuzzyCompare(*vecPtr, rhs);
}

#endif // QVECTOR3DMAPPED_P_H
