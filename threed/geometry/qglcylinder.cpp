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

#include "qglcylinder.h"
#include "qglbuilder.h"
#include "qvector2darray.h"
#include "qvector3darray.h"
#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLCylinder
    \brief The QGLCylinder class represents the geometry of a simple cylinder/cone in 3D space.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::geometry

    The following example creates a cone with a top diameter of 1 unit,
    a bottom diameter of  of 2 units in diameter and height of 3 units.

    It then draws it at (10, 25, 0) in a QGLPainter:

    \code
    QGLBuilder builder;
    builder << QGLCylinder(1,2,3);
    QGLSceneNode *node = builder.finalizedSceneNode();

    painter.translate(10, 25, 0);
    node->draw(&painter);
    \endcode;

    Note that the bottom circle of the cylinder will always be centred at (0,0,0)
    unless otherwise transformed after cylinder creation.

    The QGLCylinder class specifies positions, normals and 2D texture
    co-ordinates for all of the vertices that make up the cylinder.

    The texture co-ordinates are fixed at construction time.  This
    is because constructing the sphere can involve generating additional
    vertices which need to interpolate the texture co-ordinates of their
    neighboring vertices.

    The QGLCylinder is divided into slices and layers.  The slices value
    indicate number of triangular sections into which the top and bottom
    circles of the sphere are broken into.  Consequently it also sets the
    number of facets which run the length of the cylinder.

    The layers value indicates the number of longitudinal sections the
    sphere is broken into.  Fewer layers means that the side facets of the
    cylinder will be made up of fewer, very long, triangles, while a higher
    number of layers will produce many and smaller triangles.

    Textures are wrapped around the cylinder in such a way that the texture
    may distort across the x axis if the top and bottom diameters of the
    cylinder differ (ie. the cylinder forms a truncated cone).

    \sa QGLBuilder
*/

/*!
    Destroys this sphere object.
*/
QGLCylinder::~QGLCylinder()
{
}

/*!
    \fn qreal QGLCylinder::diameterTop() const

    Returns the diameter of the top of the cylinder.

    The default value is 1.

    \sa setDiameter()
*/

/*!
    \fn void QGLCylinder::setDiameter(qreal diameter)

    Sets the diameter of the top of this cylinder to \a diameter.

    \sa diameter()
*/

/*!
    \fn qreal QGLCylinder::diameterBottom() const

    Returns the diameter of the bottom of the cylinder.

    The default value is 1.

    \sa setDiameter()
*/

/*!
    \fn void QGLCylinder::setDiameterBottom(qreal diameter)

    Sets the diameter of the bottom of this cylinder to \a diameter.

    \sa diameter()
*/

/*!
    \fn int QGLCylinder::slices() const

    Returns the number of triangular slices the cylinder is divided into
    around its polar axis.

    The default is 6.

    \sa setSlices()
*/

/*!
    \fn int QGLCylinder::setSlices(int slices)

    Sets the number of triangular slices the cylinder is divided into
    around its polar axis.

    \sa slices()
*/

/*!
    \fn int QGLCylinder::layers() const

    Returns the number of cylindrical layers the cylinder is divided into
    along its height.

    The default is 3.

    \sa setLayers()
*/

/*!
    \fn int QGLCylinder::setLayers() const

    Sets the number of cylindrical layers the cylinder is divided into
    along its height.

    \sa layers()
*/


/*!
    \relates QGLCylinder

    Builds the geometry for \a sphere within the specified
    geometry \a builder.
*/
///TBD
QGLBuilder& operator<<(QGLBuilder& builder, const QGLCylinder& cylinder)
{
  /*  ASSERT(cylinder.diameterBottom()>=0 &&
           cylinder.diameterTop()>=0 &&
           cylinder.height()>0);*/

    //Current version ignores the layers values (1 big layer only).

    qreal numSlices = qreal(cylinder.slices());
    qreal topRadius = cylinder.diameterTop()/2.0;
    qreal bottomRadius = cylinder.diameterBottom()/2.0;

    qreal angle = 0;
    qreal angleIncrement = (2.0 * M_PI) / numSlices;

    //Bottom of Cylinder
    QGeometryData bottomSurface;
    for (int i=0; i<cylinder.slices(); i++)
    {
        bottomSurface.appendVertex(QVector3D(bottomRadius * qCos(angle),
                                          bottomRadius * qSin(angle),
                                          0.0));
        angle+=angleIncrement;
    }

    bottomSurface.center();
    builder.newSection();
    builder.currentNode()->setObjectName("CylinderBase");
    QGeometryData base;
    base.appendVertex(bottomSurface.center());
    base.appendVertexArray(bottomSurface.vertices());
    builder.addTriangulatedFace(base);
    angle = 0;

    //Top of Cylinder
    QGeometryData topSurface;
    for (int i=0; i<cylinder.slices(); i++)
    {
        topSurface.appendVertex(QVector3D(topRadius * qCos(angle),
                                          topRadius * qSin(angle),
                                          0.0));
        angle+=angleIncrement;
    }

    topSurface.center();
    builder.newSection();
    builder.currentNode()->setObjectName("CylinderTop");
    QGeometryData top;
    top.appendVertex(topSurface.center());
    top.appendVertexArray(topSurface.vertices());
    builder.addTriangulatedFace(top);

    //Side facets of the cylinder
    builder.newSection();
    builder.currentNode()->setObjectName(QLatin1String("CylinderSides"));
    bottomSurface.appendVertex(bottomSurface.vertex(0)); // this vertex is doubled up to give us a texture seam
    bottomSurface.generateTextureCoordinates();
    topSurface.appendVertex(topSurface.vertex(0)); // this vertex is doubled up to give us a texture seam
    topSurface.generateTextureCoordinates();
    builder.addQuadsInterleaved(topSurface, bottomSurface);
    return builder;
}
