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
    builder << QGLCylinder();
    QGLSceneNode *node = builder.finalizedSceneNode();

    painter.translate(10, 25, 0);
    node->draw(&painter);
    \endcode;

    Note that the bottom circle of the cylinder will always be centred at (0,0,0)
    unless otherwise transformed after cylinder creation.

    The QGLCylinder class specifies positions, normals and 2D texture
    co-ordinates for all of the vertices that make up the cylinder.

    The texture co-ordinates are fixed at construction time.  This
    is because constructing the cylinder can involve generating additional
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
    cylinder differ (ie. the cylinder forms a truncated cone).  Textures 
    begin and end at the centre points of the top and bottom end-caps of the
    cylinder.  This wrapping means that textures on either end-cap may be
    distorted.

    A single texture is used for the entire cylinder, including its end-caps as
    well as the sides.  The user may specify what fraction of the overall 
    texture is to be used for the sides of the cylinder.  This is taken from
    the middle of the texture, with the remaining strips at the top and bottom
    of the texture being used for the top and bottom endcaps respectively.

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

    \sa setDiameterTop()
*/

/*!
    \fn void QGLCylinder::setDiameterTop(qreal diameter)

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
    \fn int QGLCylinder::setLayers(int layers) const

    Sets the number of cylindrical layers the cylinder is divided into
    along its height.

    \sa layers()
*/

/*!
    \fn qreal QGLCylinder::textureFraction() const

    When texturing a cylinder we use a single texture image to cover the body
    and the end-caps of the object.  This function returns the fraction of that
    image that is to be used to cover the sides of the cylinder (with the 
    remainder divided evenly between the end-caps.

    For example, if this value is 0.8 then the middle 80% of the texture image
    is used to texture the sides of the cylinder, while the 10% strips at the 
    top and bottom are used for the top and bottom end-cap respectively.

    The default value is 0.8

    \sa setTextureFraction()
*/

/*!
    \fn int QGLCylinder::setTextureFraction(qreal fraction)

    Sets the fraction of the texture for this cylinder which will be used to 
    cover the sides of the cylinder (with the remainder being divided evenly
    between the top and the bottom end-caps).

    \sa textureFraction()
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

    qreal numSlices = qreal(cylinder.slices());
    qreal numLayers = qreal(cylinder.layers());
    qreal topRadius = cylinder.diameterTop()/2.0;
    qreal bottomRadius = cylinder.diameterBottom()/2.0;
    QGeometryData top, base;

    qDebug()<<"Making cylinder with " << numSlices << " segments and top/bottom radii of " << topRadius << "," << bottomRadius;

    qreal angle = 0;
    qreal angleIncrement = (2.0 * M_PI) / numSlices;
    qreal radius = topRadius;
    qreal radiusIncrement = qreal(bottomRadius-topRadius)/ numLayers;
    qreal height = qreal(cylinder.height());
    qreal heightDecrement = height/numLayers;
    qreal textureHeight = 1.0-((1.0-cylinder.textureFraction())/2.0); 
    qreal textureDecrement = cylinder.textureFraction()/numLayers;

    QGeometryData oldLayer;

    //Generate vertices for the next layer of cylinder
    for (int layerCount=0; layerCount<=cylinder.layers(); layerCount++) {        
        
        qDebug() << "Generating layer " << layerCount << " of " << numLayers;
        
        QGeometryData newLayer;

        //Generate a circle of vertices for this layer.
        for (int i=0; i<cylinder.slices(); i++)
        {
            newLayer.appendVertex(QVector3D(radius * qCos(angle),
                                            radius * qSin(angle),
                                            height));
            angle+=angleIncrement;
        }        
                
        // Generate texture coordinates (including an extra seam vertex for textures).
        newLayer.appendVertex(newLayer.vertex(0)); 
        newLayer.generateTextureCoordinates();        
        for (int i = 0; i < newLayer.count(); ++i)  newLayer.texCoord(i).setY(textureHeight);
        qDebug()<<"Set texture height for layer " << layerCount << " to " << textureHeight;


        //Special cases for top end-cap
        if (layerCount==0) {
            //Draw end-cap at top            
            top.appendVertex(newLayer.center());
            top.appendVertexArray(newLayer.vertices());    
            top.generateTextureCoordinates();
            top.texCoord(0).setY(1.0);
            for (int i = 1; i < top.count(); ++i)
                top.texCoord(i).setY(textureHeight);
            builder.addTriangleFan(top);
        }

        //Special cases for bottom end-cap
        if (layerCount==cylinder.layers()) {
            //Draw end-cap at bottom            
            base.appendVertex(newLayer.center());
            base.appendVertexArray(newLayer.vertices());    
            base.generateTextureCoordinates();
            base.texCoord(0).setY(0.0);
            for (int i = 1; i < base.count(); ++i)
                base.texCoord(i).setY(textureHeight); 
            base.reversed();  //we need to reverse the above to draw it properly - windings!
            builder.addTriangleFan(base);
        }

        //Add a new cylinder layer to the mesh
        if (layerCount>0)
        {
            builder.addQuadsInterleaved(oldLayer, newLayer);
        }

        //Keep the current layer for drawing the next segment of the cylinder        
        oldLayer.clear();
        oldLayer.appendGeometry(newLayer);
        radius+=radiusIncrement;
        height-=heightDecrement;
        textureHeight-=textureDecrement;
    }

    return builder;
}
