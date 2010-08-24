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

#include "qgllittextureeffect.h"
#include "qglabstracteffect_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLLitTextureEffect
    \since 4.8
    \brief The QGLLitTextureEffect class provides a standard effect base class for drawing fragments with a lit texture.
    \ingroup qt3d
    \ingroup qt3d::painting

    \sa QGLLitDecalTextureEffect, QGLLitModulateTextureEffect
*/

/*!
    \class QGLLitDecalTextureEffect
    \since 4.8
    \brief The QGLLitDecalTextureEffect class provides a standard effect for drawing fragments with a texture decaled over a lit material.
    \ingroup qt3d
    \ingroup qt3d::painting
*/

/*!
    \class QGLLitModulateTextureEffect
    \since 4.8
    \brief The QGLLitModulateTextureEffect class provides a standard effect for drawing fragments with a texture modulated with a lit material.
    \ingroup qt3d
    \ingroup qt3d::painting
*/

/*!
    \internal
*/
QGLLitTextureEffect::QGLLitTextureEffect
        (GLenum mode, const char *vshader, const char *fshader,
         const QString& programName)
    : QGLLitMaterialEffect(mode, vshader, fshader, programName)
{
}

/*!
    Destroys this lit texture effect.
*/
QGLLitTextureEffect::~QGLLitTextureEffect()
{
}

/*!
    \reimp
*/
QList<QGL::VertexAttribute> QGLLitTextureEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    fields += QGL::Position;
    fields += QGL::Normal;
    fields += QGL::TextureCoord0;
    return fields;
}

#if !defined(QGL_FIXED_FUNCTION_ONLY)

static char const litTextureVertexShader[] =
    "attribute highp vec4 vertex;\n"
    "attribute highp vec3 normal;\n"
    "attribute highp vec4 texcoord;\n"
    "uniform highp mat4 matrix;\n"
    "uniform highp mat4 modelView;\n"
    "uniform highp mat3 normalMatrix;\n"
    "varying highp vec4 qTexCoord;\n"
    "void main(void)\n"
    "{\n"
    "    gl_Position = matrix * vertex;\n"
    "    highp vec4 vertex = modelView * vertex;\n"
    "    highp vec3 norm = normalize(normalMatrix * normal);\n"
    "    qLightVertex(vertex, norm);\n"
    "    qTexCoord = texcoord;\n"
    "}\n";

static char const litDecalFragmentShader[] =
    "uniform sampler2D tex;\n"
#if defined(QT_OPENGL_ES)
    "varying mediump vec4 qCombinedColor;\n"
#else
    "uniform bool separateSpecular;\n"
    "varying mediump vec4 qColor;\n"
    "varying mediump vec4 qSecondaryColor;\n"
#endif
    "varying highp vec4 qTexCoord;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "    mediump vec4 col = texture2D(tex, qTexCoord.st);\n"
#if defined(QT_OPENGL_ES)
    "    gl_FragColor = vec4(clamp(qCombinedColor.rgb * (1.0 - col.a) + col.rgb * col.a, 0.0, 1.0), qCombinedColor.a);\n"
#else
    "    if (separateSpecular) {\n"
    "        gl_FragColor = vec4(clamp(qColor.rgb * (1.0 - col.a) + col.rgb * col.a + qSecondaryColor.xyz, 0.0, 1.0), qColor.a);\n"
    "    } else {\n"
    "        mediump vec4 lcolor = clamp(qColor + vec4(qSecondaryColor.xyz, 0.0), 0.0, 1.0);\n"
    "        gl_FragColor = vec4(clamp(lcolor.rgb * (1.0 - col.a) + col.rgb * col.a, 0.0, 1.0), lcolor.a);\n"
    "    }\n"
#endif
    "}\n";

static char const litModulateFragmentShader[] =
    "uniform sampler2D tex;\n"
#if defined(QT_OPENGL_ES)
    "varying mediump vec4 qCombinedColor;\n"
#else
    "uniform bool separateSpecular;\n"
    "varying mediump vec4 qColor;\n"
    "varying mediump vec4 qSecondaryColor;\n"
#endif
    "varying highp vec4 qTexCoord;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "    mediump vec4 col = texture2D(tex, qTexCoord.st);\n"
#if defined(QT_OPENGL_ES)
    "    gl_FragColor = col * qCombinedColor;\n"
#else
    "    if (separateSpecular) {\n"
    "        gl_FragColor = clamp(col * qColor + vec4(qSecondaryColor.xyz, 0.0), 0.0, 1.0);\n"
    "    } else {\n"
    "        mediump vec4 lcolor = clamp(qColor + vec4(qSecondaryColor.xyz, 0.0), 0.0, 1.0);\n"
    "        gl_FragColor = col * lcolor;\n"
    "    }\n"
#endif
    "}\n";

#endif

#ifndef GL_MODULATE
#define GL_MODULATE 0x2100
#endif
#ifndef GL_DECAL
#define GL_DECAL 0x2101
#endif

/*!
    Constructs a new lit decal texture effect.
*/
QGLLitDecalTextureEffect::QGLLitDecalTextureEffect()
#if defined(QGL_FIXED_FUNCTION_ONLY)
    : QGLLitTextureEffect(GL_DECAL, 0, 0, QString())
#else
    : QGLLitTextureEffect(GL_DECAL,
                          litTextureVertexShader, litDecalFragmentShader,
                          QLatin1String("qt.texture.litdecal"))
#endif
{
}

/*!
    Destroys this lit decal texture effect.
*/
QGLLitDecalTextureEffect::~QGLLitDecalTextureEffect()
{
}

/*!
    Constructs a new lit modulate texture effect.
*/
QGLLitModulateTextureEffect::QGLLitModulateTextureEffect()
#if defined(QGL_FIXED_FUNCTION_ONLY)
    : QGLLitTextureEffect(GL_MODULATE, 0, 0, QString())
#else
    : QGLLitTextureEffect(GL_MODULATE,
                          litTextureVertexShader, litModulateFragmentShader,
                          QLatin1String("qt.texture.litmodulate"))
#endif
{
}

/*!
    Destroys this lit modulate texture effect.
*/
QGLLitModulateTextureEffect::~QGLLitModulateTextureEffect()
{
}

QT_END_NAMESPACE
