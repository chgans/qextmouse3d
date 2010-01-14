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

#include "shaderprogram.h"
#include "qglabstracteffect.h"
#include <QtOpenGL/qglshaderprogram.h>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,ShaderProgram,ShaderProgram)

class ShaderProgramEffect : public QGLAbstractEffect
{
public:
    ShaderProgramEffect();
    ~ShaderProgramEffect();

    void create(const QString& vertexShader, const QString& fragmentShader);

    QList<QGL::VertexAttribute> requiredFields() const;

    void setActive(bool flag);

    void update(QGLPainter *painter, QGLPainter::Updates updates);

    void setVertexArray(const QGLVertexArray& array);
    void setCommonNormal(const QVector3D& value);

private:
    QGLShaderProgram *program;
    int vertexAttr;
    int normalAttr;
    int colorAttr;
    int texCoord0Attr;
    int texCoord1Attr;
    int matrixUniform;
    int modelViewMatrixUniform;
    int normalMatrixUniform;
    int texture0;
    int texture1;
    int colorUniform;
};

ShaderProgramEffect::ShaderProgramEffect()
{
    program = 0;
    vertexAttr = -1;
    normalAttr = -1;
    colorAttr = -1;
    texCoord0Attr = -1;
    texCoord1Attr = -1;
    matrixUniform = -1;
    modelViewMatrixUniform = -1;
    normalMatrixUniform = -1;
    texture0 = -1;
    texture1 = -1;
    colorUniform = -1;
}

ShaderProgramEffect::~ShaderProgramEffect()
{
    delete program;
}

void ShaderProgramEffect::create
    (const QString& vertexShader, const QString& fragmentShader)
{
    delete program;
    program = new QGLShaderProgram();
    if (!program->addShaderFromSourceCode(QGLShader::Vertex, vertexShader))
        qWarning("Could not compile vertex shader");
    if (!program->addShaderFromSourceCode(QGLShader::Fragment, fragmentShader))
        qWarning("Could not compile fragment shader");
    if (!program->link())
        qWarning("Could not link shader program");
    vertexAttr = program->attributeLocation("qgl_Vertex");
    normalAttr = program->attributeLocation("qgl_Normal");
    colorAttr = program->attributeLocation("qgl_Color");
    texCoord0Attr = program->attributeLocation("qgl_TexCoord0");
    texCoord1Attr = program->attributeLocation("qgl_TexCoord1");
    matrixUniform = program->uniformLocation("qgl_ModelViewProjectionMatrix");
    modelViewMatrixUniform = program->uniformLocation("qgl_ModelViewMatrix");
    normalMatrixUniform = program->uniformLocation("qgl_NormalMatrix");
    texture0 = program->uniformLocation("qgl_Texture0");
    texture1 = program->uniformLocation("qgl_Texture1");
    colorUniform = program->uniformLocation("qgl_Color");
}

QList<QGL::VertexAttribute> ShaderProgramEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    if (vertexAttr != -1)
        fields.append(QGL::Position);
    if (normalAttr != -1)
        fields.append(QGL::Normal);
    if (colorAttr != -1)
        fields.append(QGL::Color);
    if (texCoord0Attr != -1)
        fields.append(QGL::TextureCoord0);
    if (texCoord1Attr != -1)
        fields.append(QGL::TextureCoord1);
    return fields;
}

void ShaderProgramEffect::setActive(bool flag)
{
    if (flag) {
        program->bind();
        if (texture0 != -1)
            program->setUniformValue(texture0, 0);
        if (texture1 != -1)
            program->setUniformValue(texture1, 1);
    } else {
        program->release();
    }
}

void ShaderProgramEffect::update
    (QGLPainter *painter, QGLPainter::Updates updates)
{
    // Update the matrix uniforms.
    if ((updates & (QGLPainter::UpdateProjectionMatrix |
                    QGLPainter::UpdateModelViewMatrix)) != 0) {
        QMatrix4x4 mv = painter->modelViewMatrix();
        if (matrixUniform != -1)
            program->setUniformValue(matrixUniform, painter->combinedMatrix());
        if (modelViewMatrixUniform != -1)
            program->setUniformValue(modelViewMatrixUniform, mv);
        if (normalMatrixUniform != -1)
            program->setUniformValue(normalMatrixUniform, mv.normalMatrix());
    }

    // Update the static color in if "qgl_Color" is a uniform.
    if ((updates & QGLPainter::UpdateColor) != 0 && colorUniform != -1) {
        program->setUniformValue(colorUniform, painter->color());
    }

    // TODO: lighting and material parameters.
}

void ShaderProgramEffect::setVertexArray(const QGLVertexArray& array)
{
    QGLAttributeValue value = array.attributeValue(QGL::Position);
    if (!value.isNull() && vertexAttr != -1) {
        setAttributeArray(program, vertexAttr, value);
        program->enableAttributeArray(vertexAttr);
    }
    value = array.attributeValue(QGL::Normal);
    if (!value.isNull() && normalAttr != -1) {
        setAttributeArray(program, normalAttr, value);
        program->enableAttributeArray(normalAttr);
    }
    value = array.attributeValue(QGL::Color);
    if (!value.isNull() && colorAttr != -1) {
        setAttributeArray(program, colorAttr, value);
        program->enableAttributeArray(colorAttr);
    }
    value = array.attributeValue(QGL::TextureCoord0);
    if (!value.isNull() && texCoord0Attr != -1) {
        setAttributeArray(program, texCoord0Attr, value);
        program->enableAttributeArray(texCoord0Attr);
    }
    value = array.attributeValue(QGL::TextureCoord1);
    if (!value.isNull() && texCoord1Attr != -1) {
        setAttributeArray(program, texCoord1Attr, value);
        program->enableAttributeArray(texCoord1Attr);
    }
}

void ShaderProgramEffect::setCommonNormal(const QVector3D& value)
{
    if (normalAttr != -1) {
        program->disableAttributeArray(normalAttr);
        program->setAttributeValue(normalAttr, value);
    }
}

class ShaderProgramPrivate
{
public:
    ShaderProgramPrivate() : regenerate(false), effect(0) {}

    QString vertexShader;
    QString fragmentShader;
    bool regenerate;
    ShaderProgramEffect *effect;
};

ShaderProgram::ShaderProgram(QObject *parent)
    : Effect(parent)
{
    d = new ShaderProgramPrivate();
}

ShaderProgram::~ShaderProgram()
{
    delete d->effect;
    delete d;
}

QString ShaderProgram::vertexShader() const
{
    return d->vertexShader;
}

void ShaderProgram::setVertexShader(const QString& value)
{
    d->vertexShader = value;
    d->regenerate = true;
    emit effectChanged();
}

QString ShaderProgram::fragmentShader() const
{
    return d->fragmentShader;
}

void ShaderProgram::setFragmentShader(const QString& value)
{
    d->fragmentShader = value;
    d->regenerate = true;
    emit effectChanged();
}

void ShaderProgram::enableEffect(QGLPainter *painter)
{
    if (!d->effect) {
        d->effect = new ShaderProgramEffect();
        d->effect->create(d->vertexShader, d->fragmentShader);
    } else if (d->regenerate) {
        d->effect->create(d->vertexShader, d->fragmentShader);
    }
    d->regenerate = false;
    painter->setUserEffect(d->effect);
    painter->setTexture(texture2D());
    painter->setColor(color());
}

QT_END_NAMESPACE
