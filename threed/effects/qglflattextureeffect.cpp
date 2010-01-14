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

#include "qglflattextureeffect_p.h"
#if defined(QGL_SHADERS_ONLY)
#include <QtOpenGL/qglshaderprogram.h>
#endif

QT_BEGIN_NAMESPACE

QList<QGL::VertexAttribute> QGLFlatTextureEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    fields += QGL::Position;
    fields += QGL::TextureCoord0;
    return fields;
}

QList<QGL::VertexAttribute> QGLFlatDecalTextureEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    fields += QGL::Position;
    fields += QGL::TextureCoord0;
    return fields;
}

#if !defined(QGL_SHADERS_ONLY)

QGLFlatTextureEffect::QGLFlatTextureEffect()
{
}

QGLFlatTextureEffect::~QGLFlatTextureEffect()
{
}

void QGLFlatTextureEffect::setActive(bool flag)
{
    if (flag) {
        glEnableClientState(GL_VERTEX_ARRAY);
        enableVertexAttribute(QGL::TextureCoord0);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    } else {
        glDisableClientState(GL_VERTEX_ARRAY);
        disableVertexAttribute(QGL::TextureCoord0);
    }
}

QGLFlatDecalTextureEffect::QGLFlatDecalTextureEffect()
{
}

QGLFlatDecalTextureEffect::~QGLFlatDecalTextureEffect()
{
}

void QGLFlatDecalTextureEffect::setActive(bool flag)
{
    if (flag) {
        glEnableClientState(GL_VERTEX_ARRAY);
        enableVertexAttribute(QGL::TextureCoord0);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    } else {
        glDisableClientState(GL_VERTEX_ARRAY);
        disableVertexAttribute(QGL::TextureCoord0);
    }
}

void QGLFlatDecalTextureEffect::update
    (QGLPainter *painter, QGLPainter::Updates updates)
{
    if ((updates & QGLPainter::UpdateColor) != 0) {
        QColor color = painter->color();
        glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    }
}

#else // QGL_SHADERS_ONLY

QGLFlatTextureEffect::QGLFlatTextureEffect()
{
    program = 0;
    matrixUniform = -1;
}

QGLFlatTextureEffect::~QGLFlatTextureEffect()
{
    delete program;
}

static char const flatTexVertexShader[] =
    "attribute highp vec4 vertex;\n"
    "attribute highp vec4 texcoord;\n"
    "uniform mediump mat4 matrix;\n"
    "varying highp vec4 qTexCoord;\n"
    "void main(void)\n"
    "{\n"
    "    gl_Position = matrix * vertex;\n"
    "    qTexCoord = texcoord;\n"
    "}\n";

static char const flatTexFragmentShader[] =
    "uniform sampler2D tex;\n"
    "varying highp vec4 qTexCoord;\n"
    "void main(void)\n"
    "{\n"
    "    gl_FragColor = texture2D(tex, qTexCoord.st);\n"
    "}\n";

void QGLFlatTextureEffect::setActive(bool flag)
{
    if (!program) {
        if (!flag)
            return;
        program = new QGLShaderProgram();
        program->addShaderFromSourceCode(QGLShader::Vertex, flatTexVertexShader);
        program->addShaderFromSourceCode(QGLShader::Fragment, flatTexFragmentShader);
        program->bindAttributeLocation("vertex", 0);
        program->bindAttributeLocation("texcoord", 1);
        if (!program->link()) {
            qWarning("QGLFlatTextureEffect::setActive(): could not link shader program");
            delete program;
            program = 0;
            return;
        }
        matrixUniform = program->uniformLocation("matrix");
        program->bind();
        program->setUniformValue("tex", 0);
        program->enableAttributeArray(0);
        program->enableAttributeArray(1);
    } else if (flag) {
        program->bind();
        program->setUniformValue("tex", 0);
        program->enableAttributeArray(0);
        program->enableAttributeArray(1);
    } else {
        program->disableAttributeArray(0);
        program->disableAttributeArray(1);
        program->release();
    }
}

void QGLFlatTextureEffect::update
        (QGLPainter *painter, QGLPainter::Updates updates)
{
    if ((updates & (QGLPainter::UpdateProjectionMatrix |
                    QGLPainter::UpdateModelViewMatrix)) != 0) {
        program->setUniformValue(matrixUniform, painter->combinedMatrix());
    }
}

void QGLFlatTextureEffect::setVertexArray(const QGLVertexArray& array)
{
    QGLAttributeValue value = array.attributeValue(QGL::Position);
    if (!value.isNull())
        setAttributeArray(program, 0, value);
    value = array.attributeValue(QGL::TextureCoord0);
    if (!value.isNull())
        setAttributeArray(program, 1, value);
}

QGLFlatDecalTextureEffect::QGLFlatDecalTextureEffect()
{
    colorUniform = -1;
}

QGLFlatDecalTextureEffect::~QGLFlatDecalTextureEffect()
{
}

static char const flatDecalFragmentShader[] =
    "uniform sampler2D tex;\n"
    "uniform mediump vec4 color;\n"
    "varying highp vec4 qTexCoord;\n"
    "void main(void)\n"
    "{\n"
    "    mediump vec4 col = texture2D(tex, qTexCoord.st);\n"
    "    gl_FragColor = vec4(clamp(color.rgb * (1.0 - col.a) + col.rgb, 0.0, 1.0), 1.0);\n"
    "}\n";

void QGLFlatDecalTextureEffect::setActive(bool flag)
{
    if (!program) {
        if (!flag)
            return;
        program = new QGLShaderProgram();
        program->addShaderFromSourceCode(QGLShader::Vertex, flatTexVertexShader);
        program->addShaderFromSourceCode(QGLShader::Fragment, flatDecalFragmentShader);
        program->bindAttributeLocation("vertex", 0);
        program->bindAttributeLocation("texcoord", 1);
        if (!program->link()) {
            qWarning("QGLFlatDecalTextureEffect::setActive(): could not link shader program");
            delete program;
            program = 0;
            return;
        }
        matrixUniform = program->uniformLocation("matrix");
        colorUniform = program->uniformLocation("color");
        program->bind();
        program->setUniformValue("tex", 0);
        program->enableAttributeArray(0);
        program->enableAttributeArray(1);
    } else if (flag) {
        program->bind();
        program->setUniformValue("tex", 0);
        program->enableAttributeArray(0);
        program->enableAttributeArray(1);
    } else {
        program->disableAttributeArray(0);
        program->disableAttributeArray(1);
        program->release();
    }
}

void QGLFlatDecalTextureEffect::update
        (QGLPainter *painter, QGLPainter::Updates updates)
{
    if ((updates & QGLPainter::UpdateColor) != 0)
        program->setUniformValue(colorUniform, painter->color());
    QGLFlatTextureEffect::update(painter, updates);
}

#endif // QGL_SHADERS_ONLY

QT_END_NAMESPACE
