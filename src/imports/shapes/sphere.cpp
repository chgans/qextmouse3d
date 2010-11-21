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

#include "sphere.h"
#include "qglbuilder.h"
#include "qglsphere.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass Sphere Sphere
    \brief The Sphere item represents a simple UV-sphere in 3D space.
    \since 4.8
    \ingroup qt3d::qml3d::shapes
    \inherits Item3D

    The Sphere element in QML provides a simple way to create a sphere
    object, usually for testing material effects.  For example,
    the following QML code displays a green sphere of radius 1.5,
    centered on the origin:

    \code
    Sphere {
        radius: 1.5
        effect: Effect {
            color: "#aaca00"
        }
    }
    \endcode

    The Sphere element is part of the \c{Qt3D.Shapes} namespace,
    so the following must appear at the top of any QML file that
    references it:

    \code
    import Qt3D.Shapes 1.0
    \endcode

    \sa Item3D
*/
/*!
    \internal
*/
Sphere::Sphere(QObject *parent)
    : QDeclarativeItem3D(parent)
    , m_radius(1.0f)
    , m_lod(3)
{
}

/*!
    \qmlproperty real Sphere::radius

    This property defines the radius of the sphere.  The default
    value is 1.
*/
void Sphere::setRadius(qreal radius)
{
    if (m_radius != radius) {
        m_radius = radius;
        emit radiusChanged();
    }
}

/*!
    \qmlproperty int Sphere::levelOfDetail

    This property defines the level of detail, between 1 and 5,
    which controls the number of triangles that are generated to
    render the surface of the sphere.  Values outside the range
    1 to 5 will be clamped to the range when the sphere is drawn.

    The default value is 3.  An increase in 1 in level of detail
    increases the number of triangles by a factor of 4.  The
    following picture shows the effect of level of detail values
    between 1 and 5:

    \image sphere-detail.png

    \table
    \header \o Level of Detail \o Number of Triangles
    \row \o 1 \o 64
    \row \o 2 \o 256
    \row \o 3 \o 1024
    \row \o 4 \o 4096
    \row \o 5 \o 16384
    \endtable
*/
void Sphere::setLevelOfDetail(int lod)
{
    if (m_lod != lod) {
        m_lod = lod;
        emit levelOfDetailChanged();
    }
}

/*!
    \internal
*/
void Sphere::drawItem(QGLPainter *painter)
{
    // Convert the level of detail into a depth value for QGLSphere.
    // We cache a maximum of 5 levels of detail for lod animations.
    int lod = m_lod;
    if (lod < 1)
        lod = 1;
    else if (lod > 5)
        lod = 5;

    // Create a new geometry node for this level of detail if necessary.
    QGLSceneNode *geometry = m_lodGeometry.value(lod, 0);
    if (!geometry) {
        QGLBuilder builder;
        builder.newSection(QGL::Faceted);
        builder << QGLSphere(2.0f, lod);
        geometry = builder.finalizedSceneNode();
        geometry->setParent(this);
        m_lodGeometry.insert(lod, geometry);
    }

    // Set the radius as a scale on the modelview transformation.
    // This way, we don't have to regenerate the geometry every
    // frame if the radius is being animated.
    if (m_radius != 1.0f) {
        painter->modelViewMatrix().push();
        painter->modelViewMatrix().scale(m_radius);
    }

    // Draw the geometry.
    geometry->draw(painter);

    // Restore the original modelview.
    if (m_radius != 1.0f)
        painter->modelViewMatrix().pop();
}
