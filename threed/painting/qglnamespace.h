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

#ifndef QGLNAMESPACE_H
#define QGLNAMESPACE_H

#include "qt3dglobal.h"

#include <QtCore/qvector.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector2d.h>
#include "qcolor4b.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

namespace QGL
{
    enum VertexAttribute {
        Position,
        Normal,
        Color,
        TextureCoord0,
        TextureCoord1,
        TextureCoord2,
        TextureCoord3,
        TextureCoord4,
        TextureCoord5,
        TextureCoord6,
        TextureCoord7,
        CustomVertex0,
        CustomVertex1,
        CustomVertex2,
        CustomVertex3,
        CustomVertex4,
        CustomVertex5,
        CustomVertex6,
        CustomVertex7
    };

    enum Face
    {
        FrontFaces                  = 0x0404, // GL_FRONT
        BackFaces                   = 0x0405, // GL_BACK
        AllFaces                    = 0x0408  // GL_FRONT_AND_BACK
    };

    enum CullFace
    {
        CullDisabled                = 0,
        CullFrontFaces              = 0x0404, // GL_FRONT
        CullBackFaces               = 0x0405, // GL_BACK
        CullAllFaces                = 0x0408, // GL_FRONT_AND_BACK
        CullClockwise               = 0x10000
    };
    Q_DECLARE_FLAGS(CullFaces, CullFace)

    enum DrawingMode
    {
        NoDrawingMode               = -1,
        Points                      = 0x0000, // GL_POINTS
        Lines                       = 0x0001, // GL_LINES
        LineLoop                    = 0x0002, // GL_LINE_LOOP
        LineStrip                   = 0x0003, // GL_LINE_STRIP
        Triangles                   = 0x0004, // GL_TRIANGLES
        TriangleStrip               = 0x0005, // GL_TRIANGLE_STRIP
        TriangleFan                 = 0x0006  // GL_TRIANGLE_FAN
    };

    enum StandardEffect
    {
        FlatColor,
        FlatPerVertexColor,
        FlatReplaceTexture2D,
        FlatDecalTexture2D,
        LitMaterial,
        LitDecalTexture2D,
        LitModulateTexture2D
    };

    enum TextureFilter
    {
        Nearest                     = 0x2600,   // GL_NEAREST
        Linear                      = 0x2601,   // GL_LINEAR
        NearestMipmapNearest        = 0x2700,   // GL_NEAREST_MIPMAP_NEAREST
        NearestMipmapLinear         = 0x2702,   // GL_NEAREST_MIPMAP_LINEAR
        LinearMipmapNearest         = 0x2701,   // GL_LINEAR_MIPMAP_NEAREST
        LinearMipmapLinear          = 0x2703    // GL_LINEAR_MIPMAP_LINEAR
    };

    enum TextureWrap
    {
        Repeat                      = 0x2901,   // GL_REPEAT
        Clamp                       = 0x2900,   // GL_CLAMP
        ClampToBorder               = 0x812D,   // GL_CLAMP_TO_BORDER
        ClampToEdge                 = 0x812F,   // GL_CLAMP_TO_EDGE
        MirroredRepeat              = 0x8370    // GL_MIRRORED_REPEAT
    };

    enum ClearBuffer
    {
        NoClearBuffers              = 0x0000,
        ClearDepthBuffer            = 0x0100,   // GL_DEPTH_BUFFER_BIT
        ClearStencilBuffer          = 0x0400,   // GL_STENCIL_BUFFER_BIT
        ClearColorBuffer            = 0x4000    // GL_COLOR_BUFFER_BIT
    };
    Q_DECLARE_FLAGS(ClearBuffers, ClearBuffer)

    enum Smoothing
    {
        NoSmoothing,
        Smooth,
        Faceted
    };

    enum Strategy
    {
        NullStrategy,
        HashLookup,
        MapLookup
    };
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGL::CullFaces)
Q_DECLARE_OPERATORS_FOR_FLAGS(QGL::ClearBuffers)

QT_END_NAMESPACE

QT_END_HEADER

#endif
