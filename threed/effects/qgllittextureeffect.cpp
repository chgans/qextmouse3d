/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qgllittextureeffect_p.h"

QT_BEGIN_NAMESPACE

#if !defined(QGL_SHADERS_ONLY)

QGLLitTextureEffect::QGLLitTextureEffect(GLenum mode)
{
    envMode = mode;
}

QGLLitTextureEffect::~QGLLitTextureEffect()
{
}

QList<QGL::VertexAttribute> QGLLitTextureEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    fields += QGL::Position;
    fields += QGL::Normal;
    fields += QGL::TextureCoord0;
    return fields;
}

void QGLLitTextureEffect::setActive(bool flag)
{
    if (flag) {
        glEnable(GL_LIGHTING);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        enableVertexAttribute(QGL::TextureCoord0);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, envMode);
    } else {
        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        disableVertexAttribute(QGL::TextureCoord0);
    }
}

void QGLLitTextureEffect::update
    (QGLPainter *painter, QGLPainter::Updates updates)
{
    updateLighting(painter, updates);
    if ((updates & QGLPainter::UpdateFog) != 0)
        updateFog(painter);
}

QGLLitDecalTextureEffect::QGLLitDecalTextureEffect()
    : QGLLitTextureEffect(GL_DECAL)
{
}

QGLLitDecalTextureEffect::~QGLLitDecalTextureEffect()
{
}

void QGLLitDecalTextureEffect::update
    (QGLPainter *painter, QGLPainter::Updates updates)
{
    if ((updates & QGLPainter::UpdateColor) != 0) {
        QColor color = painter->color();
        glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    }
    QGLLitTextureEffect::update(painter, updates);
}

QGLLitModulateTextureEffect::QGLLitModulateTextureEffect()
    : QGLLitTextureEffect(GL_MODULATE)
{
}

QGLLitModulateTextureEffect::~QGLLitModulateTextureEffect()
{
}

void QGLLitModulateTextureEffect::update
    (QGLPainter *painter, QGLPainter::Updates updates)
{
    if ((updates & QGLPainter::UpdateColor) != 0) {
        QColor color = painter->color();
        glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    }
    QGLLitTextureEffect::update(painter, updates);
}

#else // QGL_SHADERS_ONLY

static char const litTextureVertexShader[] =
    "attribute highp vec4 vertex;\n"
    "attribute highp vec3 normal;\n"
    "attribute highp vec4 texcoord;\n"
    "uniform mediump mat4 matrix;\n"
    "uniform mediump mat4 modelView;\n"
    "uniform mediump mat3 normalMatrix;\n"
    "varying highp vec4 qTexCoord;\n"
    "void qLightVertex(vec4 vertex, vec3 normal);\n"
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
    "varying mediump vec4 qColor;\n"
    "varying mediump vec4 qSecondaryColor;\n"
    "varying highp vec4 qTexCoord;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "    mediump vec4 col = texture2D(tex, qTexCoord.st);\n"
    "    mediump vec4 lcolor = clamp(qColor + vec4(qSecondaryColor.xyz, 0.0), 0.0, 1.0);\n"
    "    gl_FragColor = vec4(clamp(lcolor.rgb * (1.0 - col.a) + col.rgb, 0.0, 1.0), 1.0);\n"
    "}\n";

static char const litModulateFragmentShader[] =
    "uniform sampler2D tex;\n"
    "varying mediump vec4 qColor;\n"
    "varying mediump vec4 qSecondaryColor;\n"
    "varying highp vec4 qTexCoord;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "    mediump vec4 col = texture2D(tex, qTexCoord.st);\n"
    "    mediump vec4 lcolor = clamp(qColor + vec4(qSecondaryColor.xyz, 0.0), 0.0, 1.0);\n"
    "    gl_FragColor = col * lcolor;\n"
    "}\n";

QGLLitDecalTextureEffect::QGLLitDecalTextureEffect()
    : QGLLitMaterialEffect(litTextureVertexShader, litDecalFragmentShader)
{
}

QGLLitDecalTextureEffect::~QGLLitDecalTextureEffect()
{
}

QList<QGL::VertexAttribute> QGLLitDecalTextureEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    fields += QGL::Position;
    fields += QGL::Normal;
    fields += QGL::TextureCoord0;
    return fields;
}

QGLLitModulateTextureEffect::QGLLitModulateTextureEffect()
    : QGLLitMaterialEffect(litTextureVertexShader, litModulateFragmentShader)
{
}

QGLLitModulateTextureEffect::~QGLLitModulateTextureEffect()
{
}

QList<QGL::VertexAttribute> QGLLitModulateTextureEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    fields += QGL::Position;
    fields += QGL::Normal;
    fields += QGL::TextureCoord0;
    return fields;
}

#endif // QGL_SHADERS_ONLY

QT_END_NAMESPACE
