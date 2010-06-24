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

#include "quadplane.h"

#include "qgloperation.h"

/*!
    \class QuadPlane
    \brief The QuadPlane class holds a 3D model of a large flat plane.
    When drawing a large flat plane, such as a wall or a floor, it is desirable
    to decompose it into smaller units to make the shading effects look correct.
    This class composes a plane out of a configurable number of quads.
*/

/*!
    Construct a new QuadPlane with \a size, subdivided \a level times.  By default
    the plane is 100.0f x 100.0f, and is subdivided 3 times - that is into an
    8 x 8 grid.

    It is centered on the origin, and lies in the z = 0 plane.
*/
QuadPlane::QuadPlane(QObject *parent, QGLMaterialCollection *materials,
                     QSizeF size, int level)
                         : QGLDisplayList(parent, materials)
{
    setObjectName("QuadPlane");
    if (level > 8)
        level = 8;
    if (level < 1)
        level = 1;
    int divisions = 1;
    for ( ; level--; divisions *= 2) {}  // integer 2**n
    QSizeF div = size / float(divisions);
    QSizeF half = size / 2.0f;
    newSection();
    QGLPrimitive zip;
    QGLPrimitive zip2;
    for (int yy = 0; yy <= divisions; ++yy)
    {
        qreal y = half.height() - float(yy) * div.height();
        qreal texY = float(yy) / divisions;
        for (int xx = 0; xx <= divisions; ++xx)
        {
            qreal x = half.width() - float(xx) * div.width();
            qreal texX = float(xx) / divisions;
            zip.appendVertex(QVector3D(x, y, 0));
            zip.appendTexCoord(QVector2D(1.0f - texX, 1.0f - texY));
        }
        if (yy > 0)
            addQuadsZipped(zip, zip2);
        zip2 = zip;
        zip.clear();
    }
}
