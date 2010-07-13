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

#include "triplane.h"

#include "qglvertexbuffer.h"
#include "qglpainter.h"
#include "qline3d.h"
#include "qplane3d.h"

/*!
    \class TriPlane
    \brief The TriPlane class holds a 3D model of a large flat plane.
    When drawing a large flat plane, such as a wall or a floor, it is desirable
    to decompose it into smaller units to make the shading effects look correct.
    This class composes a plane out of a configurable number of triangles.
    The plane works no matter what the orientation or size.
*/

/*!
    Construct a new TriPlane with the specified parameters.
    The TriPlane lies in the infinite plane specified by \a plane.  This
    means that the TriPlane has the same normal everywhere as \a plane, and
    the sides of the TriPlane are determined relative to the origin of \a plane.

    The \a side vector specifies the orientation of the rectangular TriPlane,
    and the \a size specifies the TriPlanes dimensions.  The default size
    is 100.0f x 100.0f.  The resulting TriPlane is a rectangle, \a size
    in dimension, centered on the origin of the \a plane, and aligned to the
    \a side vector.  The side vector is projected onto the plane first in case
    it does not lie in the plane.  Specifying a side vector that is aligned to
    the normal of \a plane is an error and leads to undefined behaviour.

    The side vector can be understood this way: if the TriPlane is a vertical
    wall, then the side vector is an "up" vector, parallel to the two vertical
    edges of the wall.  The TriPlane will be divided up by halving lengthwise
    and widthwise \a level times.  Level is clamped to a max of 8 (256
    divisions) and min of 1 (2 divisions).

    The TriPlane sets its boundingBox() so that redundant drawing operations
    are elided.
*/
TriPlane::TriPlane(const QPlane3D &plane, const QVector3D &side,
                     QSizeF size, int level)
    : mNormal(plane.normal().normalized())
{
    if (level > 8)
        level = 8;
    if (level < 1)
        level = 1;
    int divisions = 2;
    for ( ; level--; divisions *= 2) {}  // integer 2**n
    QSizeF div = size / float(divisions);
    QSizeF half = size / 2.0f;
    QVector3D sideInPlane = plane.project(plane.origin() + side) - plane.origin();
    sideInPlane.normalize();
    QVector3D baseInPlane = QVector3D::crossProduct(sideInPlane, plane.normal());
    baseInPlane.normalize();
    // sideInPlane/baseInPlane now form a coordinate system centered at the
    // planes origin.  From there, create the tiled GL_QUADS
    QVector3D bottomLeft = plane.origin() - baseInPlane * half.width()
                           - sideInPlane * half.height();
    QVector3D topRight = plane.origin() + baseInPlane * half.width()
                         + sideInPlane * half.height();
    setBoundingBox(QBox3D(bottomLeft, topRight));
    QLine3D baseLine(bottomLeft, baseInPlane);
    QLine3D sideLine(bottomLeft, sideInPlane);
    QLine3D nxSideLine(sideLine);
    // in the loop below, climb up these two lines like a ladder: for each
    // column (ladder) create the quads, then move the two lines across (using
    // pointers to minimize copies) to do the next column
    QLine3D *l0 = &sideLine;
    QLine3D *l1 = &nxSideLine;
    QVector3DArray vertices;
    for (int col = 0; col < divisions; ++col)
    {
        // ternary ops here are avoiding fmuls & rounding errors at bounds
        float s0 = (col == 0) ? 0.0f : float(col) * div.width();
        float s1 = (col == divisions-1) ? div.width() : s0 + div.width();
        l1->setOrigin(baseLine.point(s1));
        QGLGeometry *g = new QGLGeometry();
        g->setDrawingMode(QGL::TriangleStrip);
        for (int row = 0; row <= divisions; ++row)
        {
            float t = row == 0 ? 0.0f : (row == divisions ?
                                         size.height() : float(row) * div.height());
            vertices.append(l0->point(t));
            vertices.append(l1->point(t));
        }
        qSwap(l0, l1);
        QGLVertexBuffer buffer;
        buffer.addAttribute(QGL::Position, vertices);
        g->setVertexBuffer(buffer);
        mStrips.append(g);
    }

    // Material for the floor
    mat = new QGLMaterial();
    mat->setAmbientColor(QColor(32, 34, 32));
    mat->setDiffuseColor(QColor(128, 36, 36));
    mat->setSpecularColor(QColor::fromRgbF(0.4, 0.4, 0.4, 1.0));
}

TriPlane::~TriPlane()
{
    qDeleteAll(mStrips);
    delete mat;
}

void TriPlane::draw(QGLPainter *painter)
{
    painter->setFaceMaterial(QGL::AllFaces, mat);

    painter->setCommonNormal(mNormal);
    QVector<QGLGeometry*>::iterator it = mStrips.begin();
    for ( ; it != mStrips.end(); ++it)
    {
        QGLGeometry *g = *it;
        g->draw(painter);
    }
}

bool TriPlane::upload()
{
    QVector<QGLGeometry*>::iterator it = mStrips.begin();
    for ( ; it != mStrips.end(); ++it)
    {
        QGLGeometry *g = *it;
        if (!g->upload())
            return false;
    }
    return true;
}
