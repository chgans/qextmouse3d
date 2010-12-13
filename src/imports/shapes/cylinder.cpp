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

#include "cylinder.h"
#include "qglbuilder.h"
#include "qglcylinder.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass Cylinder Cylinder
    \brief The Cylinder item represents a simple cylinder in 3D space.
    \since 4.8
    \ingroup qt3d::qml3d::shapes
    \inherits Item3D

    The Cylinder element in QML provides a way to create a simple cylinder
    object, usually for testing material effects.  For example, the
    following QML code displays a green cylinder of with a uniform
    diameter of 0.5, and a length of 3.0, centered with its base on the
    origin:

    \code
    Cylinder {
        radius: 0.5
        length: 3.0
        effect: Effect {
            color: "#aaca00"
        }
    }
    \endcode

    The Cylinder element is part of the \c{Qt3D.Shapes} namespace,
    so the following must appear at the top of any QML file that
    references it:

    \code
    import Qt3D.Shapes 1.0
    \endcode

    The cylinder shape can have differing levels of detail, allowing
    for additional slices and layers to be drawn if more detailed meshes
    are required.

    \sa Item3D
*/
/*!
    \internal
*/
Cylinder::Cylinder(QObject *parent) :
    QDeclarativeItem3D(parent)
    , m_radius(1.0)
    , m_length(1.0)
    , m_lod(1)
{

}

/*!
    \qmlproperty real Cylinder::radius

    This property defines the radius of the cylinder.
    The default value is 1.
*/
void Cylinder::setRadius(qreal radius)
{
    if (m_radius != radius) {
        m_radius = radius;
        emit radiusChanged();
        update();
    }
}

/*!
    \qmlproperty real Cylinder::length

    This property defines the length of the cylinder.
    The default value is 1.
*/
void Cylinder::setLength(qreal length)
{
    if (m_length != length) {
        m_length = length;
        emit lengthChanged();
        update();
    }
}

/*!
    \qmlproperty int Cylinder::levelOfDetail

    This property defines the level of detail, between 1 and 10,
    which controls the number of slices and layers that are used
    to generate the cylinder's mesh.

    Values outside the range 1 to 10 will be clamped to the range
    when the cylinder is generated.

    The default value is 1, which produces a cylinder with 8
    slices/facets and 2 layers.  Each successive increase in this
    level of detail doubles the number of facets and layers.  The
    following table provides information for these level of detail
    values

    \table
    \header \o Level of Detail \o Facets \o Layers
    \row \o 1 \o 8 \o 2
    \row \o 2 \o 16 \o 4
    \row \o 3 \o 32 \o 8
    \row \o 4 \o 64 \o 16
    \row \o 5 \o 128 \o 32
    \row \o 6 \o 256 \o 64
    \row \o 7 \o 512 \o 128
    \row \o 8 \o 1024 \o 256
    \row \o 9 \o 2048 \o 512
    \row \o 10 \o 4096 \o 1024
    \endtable    
*/
void Cylinder::setLevelOfDetail(int lod)
{
    if (m_lod != lod) {
        m_lod = lod;
        emit levelOfDetailChanged();
        update();
    }
}

/*!
    \internal
*/
void Cylinder::drawItem(QGLPainter *painter)
{
    // Convert the level of detail into a depth value for cylinder.
    // We cache a maximum of 5 levels of detail for lod animations.
    int lod = m_lod;
    if (lod < 1)
        lod = 1;
    else if (lod > 10)
        lod = 10;

    int facets = 4 * 1<<lod;
    int layers = 1<<lod;

    // Create a new geometry node for this level of detail if necessary.
    QGLSceneNode *geometry = m_lodGeometry.value(lod, 0);
    if (!geometry) {
        qWarning() << "Creating new geometry";
        QGLBuilder builder;
        builder << QGL::Faceted << QGLCylinder(2.0, 2.0, 1.0, facets, layers, true, true);
        geometry = builder.finalizedSceneNode();
        geometry->setParent(this);
        m_lodGeometry.insert(lod, geometry);
    }

    // Set the length as a scale on the modelview transformation.
    // This way, we don't have to regenerate the geometry every
    // frame if the height is being animated.
    if (m_length != 1.0f || m_radius!=1.0) {
        painter->modelViewMatrix().push();
        //We could do the scales separately if either height or radius
        //were !=1.0f, but the same amount of maths is required, so
        //we can just scale appropriately on the axes.
        painter->modelViewMatrix().scale(m_radius,m_radius, m_length);
    }

    // Draw the geometry.
    geometry->draw(painter);

    // Restore the original modelview if necessary
    if (m_length != 1.0f || m_radius!=1.0)
        painter->modelViewMatrix().pop();
}

