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

#include "qglflattextureeffect.h"
#include <QtOpenGL/qglshaderprogram.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLFlatTextureEffect
    \since 4.7
    \brief The QGLFlatTextureEffect class provides a standard effect that draws fragments with a flat unlit texture.
    \ingroup qt3d
    \ingroup qt3d::painting
*/

/*!
    \class QGLFlatDecalTextureEffect
    \since 4.7
    \brief The QGLFlatDecalTextureEffect class provides a standard effect that decals fragments with a flat unlit texture.
    \ingroup qt3d
    \ingroup qt3d::painting
*/

class QGLFlatTextureEffectPrivate
{
public:
    QGLFlatTextureEffectPrivate()
        : program(0)
        , matrixUniform(-1)
    {
    }

    QGLShaderProgram *program;
    int matrixUniform;
};

/*!
    Constructs a new flat texture effect.
*/
QGLFlatTextureEffect::QGLFlatTextureEffect()
    : d_ptr(new QGLFlatTextureEffectPrivate)
{
}

/*!
    Destroys this flat texture effect.
*/
QGLFlatTextureEffect::~QGLFlatTextureEffect()
{
}

/*!
    \reimp
*/
QList<QGL::VertexAttribute> QGLFlatTextureEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    fields += QGL::Position;
    fields += QGL::TextureCoord0;
    return fields;
}

#if defined(QGL_SHADERS_ONLY)

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

static char const flatDecalFragmentShader[] =
    "uniform sampler2D tex;\n"
    "uniform mediump vec4 color;\n"
    "varying highp vec4 qTexCoord;\n"
    "void main(void)\n"
    "{\n"
    "    mediump vec4 col = texture2D(tex, qTexCoord.st);\n"
    "    gl_FragColor = vec4(clamp(color.rgb * (1.0 - col.a) + col.rgb, 0.0, 1.0), 1.0);\n"
    "}\n";

#endif

/*!
    \reimp
*/
void QGLFlatTextureEffect::setActive(QGLPainter *painter, bool flag)
{
#if !defined(QGL_SHADERS_ONLY)
    Q_UNUSED(painter);
    if (flag) {
        glEnableClientState(GL_VERTEX_ARRAY);
        enableVertexAttribute(QGL::TextureCoord0);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    } else {
        glDisableClientState(GL_VERTEX_ARRAY);
        disableVertexAttribute(QGL::TextureCoord0);
    }
#else
    Q_D(QGLFlatTextureEffect);
    QGLShaderProgram *program =
        painter->cachedProgram(QLatin1String("qt.texture.flat.replace"));
    d->program = program;
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
        painter->setCachedProgram
            (QLatin1String("qt.texture.flat.replace"), program);
        d->program = program;
        d->matrixUniform = program->uniformLocation("matrix");
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
#endif
}

/*!
    \reimp
*/
void QGLFlatTextureEffect::update
        (QGLPainter *painter, QGLPainter::Updates updates)
{
#if !defined(QGL_SHADERS_ONLY)
    Q_UNUSED(painter);
    Q_UNUSED(updates);
#else
    Q_D(QGLFlatTextureEffect);
    if ((updates & QGLPainter::UpdateMatrices) != 0) {
        d->program->setUniformValue
            (d->matrixUniform, painter->combinedMatrix());
    }
#endif
}

/*!
    \reimp
*/
void QGLFlatTextureEffect::setVertexAttribute
    (QGL::VertexAttribute attribute, const QGLAttributeValue& value)
{
#if !defined(QGL_SHADERS_ONLY)
    QGLAbstractEffect::setVertexAttribute(attribute, value);
#else
    Q_D(QGLFlatTextureEffect);
    if (attribute == QGL::Position)
        setAttributeArray(d->program, 0, value);
    else if (attribute == QGL::TextureCoord0)
        setAttributeArray(d->program, 1, value);
#endif
}

class QGLFlatDecalTextureEffectPrivate
{
public:
    QGLFlatDecalTextureEffectPrivate()
        : program(0)
        , matrixUniform(-1)
        , colorUniform(-1)
    {
    }

    QGLShaderProgram *program;
    int matrixUniform;
    int colorUniform;
};

/*!
    Constructs a new flat decal texture effect.
*/
QGLFlatDecalTextureEffect::QGLFlatDecalTextureEffect()
{
}

/*!
    Destroys this flat decal texture effect.
*/
QGLFlatDecalTextureEffect::~QGLFlatDecalTextureEffect()
{
}

/*!
    \reimp
*/
QList<QGL::VertexAttribute> QGLFlatDecalTextureEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    fields += QGL::Position;
    fields += QGL::TextureCoord0;
    return fields;
}

/*!
    \reimp
*/
void QGLFlatDecalTextureEffect::setActive(QGLPainter *painter, bool flag)
{
#if !defined(QGL_SHADERS_ONLY)
    Q_UNUSED(painter);
    if (flag) {
        glEnableClientState(GL_VERTEX_ARRAY);
        enableVertexAttribute(QGL::TextureCoord0);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    } else {
        glDisableClientState(GL_VERTEX_ARRAY);
        disableVertexAttribute(QGL::TextureCoord0);
    }
#else
    Q_D(QGLFlatDecalTextureEffect);
    QGLShaderProgram *program =
        painter->cachedProgram(QLatin1String("qt.texture.flat.decal"));
    d->program = program;
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
        painter->setCachedProgram
            (QLatin1String("qt.texture.flat.decal"), program);
        d->program = program;
        d->matrixUniform = program->uniformLocation("matrix");
        d->colorUniform = program->uniformLocation("color");
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
#endif
}

/*!
    \reimp
*/
void QGLFlatDecalTextureEffect::update
    (QGLPainter *painter, QGLPainter::Updates updates)
{
#if !defined(QGL_SHADERS_ONLY)
    if ((updates & QGLPainter::UpdateColor) != 0) {
        QColor color = painter->color();
        glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    }
#else
    Q_D(QGLFlatDecalTextureEffect);
    if ((updates & QGLPainter::UpdateColor) != 0)
        d->program->setUniformValue(d->colorUniform, painter->color());
    if ((updates & QGLPainter::UpdateMatrices) != 0) {
        d->program->setUniformValue
            (d->matrixUniform, painter->combinedMatrix());
    }
#endif
}

/*!
    \reimp
*/
void QGLFlatDecalTextureEffect::setVertexAttribute
    (QGL::VertexAttribute attribute, const QGLAttributeValue& value)
{
#if !defined(QGL_SHADERS_ONLY)
    QGLAbstractEffect::setVertexAttribute(attribute, value);
#else
    Q_D(QGLFlatDecalTextureEffect);
    if (attribute == QGL::Position)
        setAttributeArray(d->program, 0, value);
    else if (attribute == QGL::TextureCoord0)
        setAttributeArray(d->program, 1, value);
#endif
}

QT_END_NAMESPACE
