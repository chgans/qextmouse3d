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

#include "qglflatcoloreffect_p.h"
#if defined(QGL_SHADERS_ONLY)
#include <QtOpenGL/qglshaderprogram.h>
#endif

QT_BEGIN_NAMESPACE

QList<QGL::VertexAttribute> QGLFlatColorEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    fields += QGL::Position;
    return fields;
}

QList<QGL::VertexAttribute> QGLPerVertexColorEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    fields += QGL::Position;
    fields += QGL::Color;
    return fields;
}

#if !defined(QGL_SHADERS_ONLY)

QGLFlatColorEffect::QGLFlatColorEffect()
{
}

QGLFlatColorEffect::~QGLFlatColorEffect()
{
}

bool QGLFlatColorEffect::supportsPicking() const
{
    return true;
}

void QGLFlatColorEffect::setActive(bool flag)
{
    if (flag) {
        glEnableClientState(GL_VERTEX_ARRAY);
    } else {
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_FOG);
    }
}

void QGLFlatColorEffect::update
        (QGLPainter *painter, QGLPainter::Updates updates)
{
    if ((updates & QGLPainter::UpdateColor) != 0) {
        QColor color;
        if (painter->isPicking())
            color = painter->pickColor();
        else
            color = painter->color();
        glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    }
    if ((updates & QGLPainter::UpdateFog) != 0)
        updateFog(painter);
}

QGLPerVertexColorEffect::QGLPerVertexColorEffect()
{
}

QGLPerVertexColorEffect::~QGLPerVertexColorEffect()
{
}

void QGLPerVertexColorEffect::setActive(bool flag)
{
    if (flag) {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
    } else {
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisable(GL_FOG);
    }
}

void QGLPerVertexColorEffect::update
        (QGLPainter *painter, QGLPainter::Updates updates)
{
    if ((updates & QGLPainter::UpdateFog) != 0)
        updateFog(painter);
}

#else // QGL_SHADERS_ONLY

QGLFlatColorEffect::QGLFlatColorEffect()
{
    program = 0;
    colorUniform = -1;
    matrixUniform = -1;
}

QGLFlatColorEffect::~QGLFlatColorEffect()
{
    delete program;
}

bool QGLFlatColorEffect::supportsPicking() const
{
    return true;
}

static char const flatColorVertexShader[] =
    "attribute highp vec4 vertex;\n"
    "uniform mediump mat4 matrix;\n"
    "void main(void)\n"
    "{\n"
    "    gl_Position = matrix * vertex;\n"
    "}\n";

static char const flatColorFragmentShader[] =
    "uniform mediump vec4 color;\n"
    "void main(void)\n"
    "{\n"
    "    gl_FragColor = color;\n"
    "}\n";

void QGLFlatColorEffect::setActive(bool flag)
{
    if (!program) {
        if (!flag)
            return;
        program = new QGLShaderProgram();
        program->addShaderFromSourceCode(QGLShader::Vertex, flatColorVertexShader);
        program->addShaderFromSourceCode(QGLShader::Fragment, flatColorFragmentShader);
        program->bindAttributeLocation("vertex", 0);
        if (!program->link()) {
            qWarning("QGLFlatColorEffect::setActive(): could not link shader program");
            delete program;
            program = 0;
            return;
        }
        colorUniform = program->uniformLocation("color");
        matrixUniform = program->uniformLocation("matrix");
        program->bind();
        program->enableAttributeArray(0);
    } else if (flag) {
        program->bind();
        program->enableAttributeArray(0);
    } else {
        program->disableAttributeArray(0);
        program->release();
    }
}

void QGLFlatColorEffect::update
        (QGLPainter *painter, QGLPainter::Updates updates)
{
    if ((updates & QGLPainter::UpdateColor) != 0) {
        if (painter->isPicking())
            program->setUniformValue(colorUniform, painter->pickColor());
        else
            program->setUniformValue(colorUniform, painter->color());
    }
    if ((updates & (QGLPainter::UpdateProjectionMatrix |
                    QGLPainter::UpdateModelViewMatrix)) != 0) {
        QMatrix4x4 proj = painter->projectionMatrix();
        QMatrix4x4 mv = painter->modelViewMatrix();
        program->setUniformValue(matrixUniform, proj * mv);
    }
}

void QGLFlatColorEffect::setVertexAttribute
    (QGL::VertexAttribute attribute, const QGLAttributeValue& value)
{
    if (attribute == QGL::Position)
        setAttributeArray(program, 0, value);
}

QGLPerVertexColorEffect::QGLPerVertexColorEffect()
{
    program = 0;
    matrixUniform = -1;
}

QGLPerVertexColorEffect::~QGLPerVertexColorEffect()
{
    delete program;
}

static char const pvColorVertexShader[] =
    "attribute highp vec4 vertex;\n"
    "attribute mediump vec4 color;\n"
    "uniform mediump mat4 matrix;\n"
    "varying mediump vec4 qColor;\n"
    "void main(void)\n"
    "{\n"
    "    gl_Position = matrix * vertex;\n"
    "    qColor = color;\n"
    "}\n";

static char const pvColorFragmentShader[] =
    "varying mediump vec4 qColor;\n"
    "void main(void)\n"
    "{\n"
    "    gl_FragColor = qColor;\n"
    "}\n";

void QGLPerVertexColorEffect::setActive(bool flag)
{
    if (!program) {
        if (!flag)
            return;
        program = new QGLShaderProgram();
        program->addShaderFromSourceCode(QGLShader::Vertex, pvColorVertexShader);
        program->addShaderFromSourceCode(QGLShader::Fragment, pvColorFragmentShader);
        program->bindAttributeLocation("vertex", 0);
        program->bindAttributeLocation("color", 1);
        if (!program->link()) {
            qWarning("QGLPerVertexColorEffect::setActive(): could not link shader program");
            delete program;
            program = 0;
            return;
        }
        matrixUniform = program->uniformLocation("matrix");
        program->bind();
        program->enableAttributeArray(0);
        program->enableAttributeArray(1);
    } else if (flag) {
        program->bind();
        program->enableAttributeArray(0);
        program->enableAttributeArray(1);
    } else {
        program->disableAttributeArray(0);
        program->disableAttributeArray(1);
        program->release();
    }
}

void QGLPerVertexColorEffect::update
        (QGLPainter *painter, QGLPainter::Updates updates)
{
    if ((updates & (QGLPainter::UpdateProjectionMatrix |
                    QGLPainter::UpdateModelViewMatrix)) != 0) {
        program->setUniformValue(matrixUniform, painter->combinedMatrix());
    }
}

void QGLPerVertexColorEffect::setVertexAttribute
    (QGL::VertexAttribute attribute, const QGLAttributeValue& value)
{
    if (attribute == QGL::Position)
        setAttributeArray(program, 0, value);
    else if (attribute == QGL::Color)
        setAttributeArray(program, 1, value);
}

#endif // QGL_SHADERS_ONLY

QT_END_NAMESPACE
