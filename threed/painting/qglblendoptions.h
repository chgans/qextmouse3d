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

#ifndef QGLBLENDOPTIONS_H
#define QGLBLENDOPTIONS_H

#include "qglnamespace.h"
#include <QtGui/qcolor.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLBlendOptionsPrivate;

class Q_QT3D_EXPORT QGLBlendOptions
{
public:
    QGLBlendOptions();
    QGLBlendOptions(const QGLBlendOptions& other);
    ~QGLBlendOptions();

    QGLBlendOptions& operator=(const QGLBlendOptions& other);

    enum BlendFactor
    {
        Zero                    = 0x0000,   // GL_ZERO
        One                     = 0x0001,   // GL_ONE
        SrcColor                = 0x0300,   // GL_SRC_COLOR
        OneMinusSrcColor        = 0x0301,   // GL_ONE_MINUS_SRC_COLOR
        SrcAlpha                = 0x0302,   // GL_SRC_ALPHA
        OneMinusSrcAlpha        = 0x0303,   // GL_ONE_MINUS_SRC_ALPHA
        DstAlpha                = 0x0304,   // GL_DST_ALPHA
        OneMinusDstAlpha        = 0x0305,   // GL_ONE_MINUS_DST_ALPHA
        DstColor                = 0x0306,   // GL_DST_COLOR
        OneMinusDstColor        = 0x0307,   // GL_ONE_MINUS_DST_COLOR
        SrcAlphaSaturate        = 0x0308,   // GL_SRC_ALPHA_SATURATE
        ConstantColor           = 0x8001,   // GL_CONSTANT_COLOR
        OneMinusConstantColor   = 0x8002,   // GL_ONE_MINUS_CONSTANT_COLOR
        ConstantAlpha           = 0x8003,   // GL_CONSTANT_ALPHA
        OneMinusConstantAlpha   = 0x8004    // GL_ONE_MINUS_CONSTANT_ALPHA
    };

    enum BlendEquation
    {
        Add                     = 0x8006,   // GL_FUNC_ADD
        Minimum                 = 0x8007,   // GL_MIN
        Maximum                 = 0x8008,   // GL_MAX
        Subtract                = 0x800A,   // GL_FUNC_SUBTRACT
        ReverseSubtract         = 0x800B    // GL_FUNC_REVERSE_SUBTRACT
    };

    inline bool isNull() const { return d == 0; }

    bool isEnabled() const;
    void setEnabled(bool value);

    QColor blendColor() const;
    void setBlendColor(const QColor& value);

    QGLBlendOptions::BlendFactor sourceColorFactor() const;
    void setSourceColorFactor(QGLBlendOptions::BlendFactor value);

    QGLBlendOptions::BlendFactor sourceAlphaFactor() const;
    void setSourceAlphaFactor(QGLBlendOptions::BlendFactor value);

    QGLBlendOptions::BlendFactor destinationColorFactor() const;
    void setDestinationColorFactor(QGLBlendOptions::BlendFactor value);

    QGLBlendOptions::BlendFactor destinationAlphaFactor() const;
    void setDestinationAlphaFactor(QGLBlendOptions::BlendFactor value);

    QGLBlendOptions::BlendEquation colorEquation() const;
    void setColorEquation(QGLBlendOptions::BlendEquation value);

    QGLBlendOptions::BlendEquation alphaEquation() const;
    void setAlphaEquation(QGLBlendOptions::BlendEquation value);

    bool operator==(const QGLBlendOptions& other) const;
    bool operator!=(const QGLBlendOptions& other) const;

    void apply() const;

private:
    QGLBlendOptionsPrivate *d;

    QGLBlendOptionsPrivate *dwrite();
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
