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

// Used to generate cube.obj.

#include <stdio.h>

static float const normals[6 * 3] = {
    -1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, -1.0f
};

static float const vertices[8 * 3] = {
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, -0.5f
};

static float const texCoords[4 * 2] = {
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f
};

static unsigned short const indices[6 * 4] = {
    0, 1, 2, 3, 3, 2, 6, 7, 7, 6, 5, 4,
    4, 5, 1, 0, 5, 6, 2, 1, 7, 4, 0, 3
};

int main(int argc, char *argv[])
{
    printf("s 0\n");
    for (int vert = 0; vert < 8; ++vert) {
        printf("v %.1f %.1f %.1f\n",
               vertices[vert * 3 + 0],
               vertices[vert * 3 + 1],
               vertices[vert * 3 + 2]);
    }
    for (int norm = 0; norm < 6; ++norm) {
        printf("vn %.1f %.1f %.1f\n",
               normals[norm * 3 + 0],
               normals[norm * 3 + 1],
               normals[norm * 3 + 2]);
    }
    for (int tex = 0; tex < 4; ++tex) {
        printf("vt %.1f %.1f\n",
               texCoords[tex * 2 + 0],
               texCoords[tex * 2 + 1]);
    }
    for (int face = 0; face < 6; ++face) {
        printf("f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
               indices[face * 4 + 0] + 1, 1, face + 1,
               indices[face * 4 + 1] + 1, 2, face + 1,
               indices[face * 4 + 2] + 1, 3, face + 1,
               indices[face * 4 + 3] + 1, 4, face + 1);
    }
    return 0;
}
