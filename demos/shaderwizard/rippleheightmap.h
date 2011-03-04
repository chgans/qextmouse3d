/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MYHEIGHTMAP_H
#define MYHEIGHTMAP_H

#include <qglheightmap.h>
#include <QtCore/qmath.h>

class RippleHeightMap : public QGLHeightMap
{
public:
    RippleHeightMap(int xPoints, int yPoints) : QGLHeightMap(xPoints,yPoints) {}

    float height(float s, float t) const
    {
        const float numberOfCrests = 3.0f;
        const float scalingFactor = 0.1f;

        float distance_from_center = sqrt((s - 0.5) * (s - 0.5)
                                            + (t - 0.5) * (t - 0.5));
        return sin( numberOfCrests * 2.0
                    * 2.0 * M_PI * distance_from_center ) // radial ripple
                * scalingFactor
                * (1.0 - distance_from_center) // it takes a power of 4 to
                * (1.0 - distance_from_center) // make a nice drop off that
                * (1.0 - distance_from_center) // looks right in the small
                * (1.0 - distance_from_center);// space.
    }
};

#endif // MYHEIGHTMAP_H
