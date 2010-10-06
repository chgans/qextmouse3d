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

#ifndef QGLCYLINDER_H
#define QGLCYLINDER_H

#include "qt3dglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLBuilder;
class QVector2D;

class Q_QT3D_EXPORT QGLCylinder
{
public:
    explicit QGLCylinder(qreal diameterTop = 1.0f, qreal diameterBottom = 1.0f, qreal height = 1.0f, int slices = 6, int layers = 3, qreal textureFraction=0.8)
        : m_diameterTop(diameterTop), m_diameterBottom(diameterBottom), m_height(height), m_slices(slices), m_layers(layers), m_textureFraction(textureFraction) {}
    virtual ~QGLCylinder();

    //Cylinder dimensions
    qreal diameterTop() const {return m_diameterTop;}
    void setDiameterTop(qreal diameter) {m_diameterTop=diameter;}

    qreal diameterBottom() const {return m_diameterBottom;}
    void setDiameterBottom(qreal diameter) {m_diameterBottom=diameter;}

    qreal height() const {return m_height;}
    void setHeight(qreal height) {m_height = height;}

    //Cylinder geometrical subdivisions
    int slices() const {return m_slices;}
    void setSlices(int slices) {m_slices = slices;}

    int layers() const {return m_layers;}
    void setLayers(int layers) {m_layers = layers;}

    //Texture ratio (the fraction of the texture given to the sides of the cylinder - 
    //the remaining upper and lower sections are used for the endcaps.
    qreal textureFraction() const {return m_textureFraction;}
    void setTextureFraction(qreal fraction) {m_textureFraction = fraction;}

protected:
    qreal m_diameterTop;
    qreal m_diameterBottom;
    qreal m_height;
    qreal m_textureFraction;
    int m_slices;
    int m_layers;
};

Q_QT3D_EXPORT QGLBuilder& operator<<(QGLBuilder& builder, const QGLCylinder& sphere);

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGLCYLINDER_H
