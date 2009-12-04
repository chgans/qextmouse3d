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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtGui/qvector3d.h>
#include "qgldisplaylist.h"
#include "qglsection.h"
#include "qgeometrydata_p.h"

class QVector3DMapped
{
public:
    typedef QVector<QVector3DMapped> VList;

    QVector3DMapped(const QVector3D* vec = 0);
    inline bool isNull() const;
    inline operator QVector3D () const;
    inline bool operator<(const QVector3DMapped &rhs) const;
    inline bool operator==(const QVector3D &rhs) const;
    bool findNormal(const QVector3D &norm,
                    const VList &list) const;
    void appendNormal(QVector3D *, QVector3DMapped::VList &);

    QString toString() const {
        const QVector3D &v = vecPtr ? *vecPtr : QVector3D(99.9f, 99.9f, 99.9f);
        QString vs = QString("QVector3D( %1, %2, %3 )").arg(v.x()).arg(v.y()).arg(v.z());
        return vs + "\n" + QString("[QVector3DMapped - vecPtr: (%1, %2, %3) - normPtr: %4]\n")
                .arg(vecPtr->x()).arg(vecPtr->y()).arg(vecPtr->z()).arg(normPtr);
    }

    const QVector3D *vecPtr;
    int normPtr;
};

inline QVector3DMapped::QVector3DMapped(const QVector3D* vec)
    : vecPtr(vec)
    , normPtr(-1)
{
}

inline QVector3DMapped::operator QVector3D () const
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
