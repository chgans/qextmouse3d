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

#include "qglflatcoloreffect.h"
#include <QtOpenGL/qglshaderprogram.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLFlatColorEffect
    \since 4.7
    \brief The QGLFlatColorEffect class provides a standard effect that draws fragments with a flat unlit color.
    \ingroup qt3d
    \ingroup qt3d::painting
*/

/*!
    \class QGLPerVertexColorEffect
    \since 4.7
    \brief The QGLPerVertexColorEffect class provides a standard effect that draws fragments with a per-vertex unlit color.
    \ingroup qt3d
    \ingroup qt3d::painting
*/

class QGLFlatColorEffectPrivate
{
public:
    QGLFlatColorEffectPrivate()
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
    Constructs a new flat color effect.
*/
QGLFlatColorEffect::QGLFlatColorEffect()
    : d_ptr(new QGLFlatColorEffectPrivate)
{
}

/*!
    Destroys this flat color effect.
*/
QGLFlatColorEffect::~QGLFlatColorEffect()
{
}

/*!
    \reimp
*/
QList<QGL::VertexAttribute> QGLFlatColorEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    fields += QGL::Position;
    return fields;
}

/*!
    \reimp
*/
bool QGLFlatColorEffect::supportsPicking() const
{
    return true;
}

/*!
    \reimp
*/
void QGLFlatColorEffect::setActive(QGLPainter *painter, bool flag)
{
#if !defined(QGL_SHADERS_ONLY)
    Q_UNUSED(painter);
    if (flag)
        glEnableClientState(GL_VERTEX_ARRAY);
    else
        glDisableClientState(GL_VERTEX_ARRAY);
#else
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

    Q_D(QGLFlatColorEffect);
    QGLShaderProgram *program =
        painter->cachedProgram(QLatin1String("qt.color.flat"));
    d->program = program;
    if (!program) {
        if (!flag)
            return;
        program = new QGLShaderProgram();
        program->addShaderFromSourceCode(QGLShader::Vertex, flatColorVertexShader);
        program->addShaderFromSourceCode(QGLShader::Fragment, flatColorFragmentShader);
        program->bindAttributeLocation("vertex", QGL::Position);
        if (!program->link()) {
            qWarning("QGLFlatColorEffect::setActive(): could not link shader program");
            delete program;
            return;
        }
        painter->setCachedProgram(QLatin1String("qt.color.flat"), program);
        d->program = program;
        d->colorUniform = program->uniformLocation("color");
        d->matrixUniform = program->uniformLocation("matrix");
        program->bind();
        program->enableAttributeArray(QGL::Position);
    } else if (flag) {
        program->bind();
        program->enableAttributeArray(QGL::Position);
    } else {
        program->disableAttributeArray(QGL::Position);
        program->release();
    }
#endif
}

/*!
    \reimp
*/
void QGLFlatColorEffect::update
        (QGLPainter *painter, QGLPainter::Updates updates)
{
#if !defined(QGL_SHADERS_ONLY)
    if ((updates & QGLPainter::UpdateColor) != 0) {
        QColor color;
        if (painter->isPicking())
            color = painter->pickColor();
        else
            color = painter->color();
        glColor4f(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    }
#else
    Q_D(QGLFlatColorEffect);
    if ((updates & QGLPainter::UpdateColor) != 0) {
        if (painter->isPicking())
            d->program->setUniformValue(d->colorUniform, painter->pickColor());
        else
            d->program->setUniformValue(d->colorUniform, painter->color());
    }
    if ((updates & QGLPainter::UpdateMatrices) != 0) {
        QMatrix4x4 proj = painter->projectionMatrix();
        QMatrix4x4 mv = painter->modelViewMatrix();
        d->program->setUniformValue(d->matrixUniform, proj * mv);
    }
#endif
}

/*!
    \reimp
*/
void QGLFlatColorEffect::setVertexAttribute
    (QGL::VertexAttribute attribute, const QGLAttributeValue& value)
{
#if !defined(QGL_SHADERS_ONLY)
    QGLAbstractEffect::setVertexAttribute(attribute, value);
#else
    Q_D(QGLFlatColorEffect);
    if (attribute == QGL::Position)
        setAttributeArray(d->program, QGL::Position, value);
#endif
}

class QGLPerVertexColorEffectPrivate
{
public:
    QGLPerVertexColorEffectPrivate()
        : program(0)
        , matrixUniform(-1)
    {
    }

    QGLShaderProgram *program;
    int matrixUniform;
};

/*!
    Constructs a new per-vertex color effect.
*/
QGLPerVertexColorEffect::QGLPerVertexColorEffect()
    : d_ptr(new QGLPerVertexColorEffectPrivate)
{
}

/*!
    Destroys this per-vertex color effect.
*/
QGLPerVertexColorEffect::~QGLPerVertexColorEffect()
{
}

/*!
    \reimp
*/
QList<QGL::VertexAttribute> QGLPerVertexColorEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    fields += QGL::Position;
    fields += QGL::Color;
    return fields;
}

/*!
    \reimp
*/
void QGLPerVertexColorEffect::setActive(QGLPainter *painter, bool flag)
{
#if !defined(QGL_SHADERS_ONLY)
    Q_UNUSED(painter);
    if (flag) {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
    } else {
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }
#else
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

    Q_D(QGLPerVertexColorEffect);
    QGLShaderProgram *program =
        painter->cachedProgram(QLatin1String("qt.color.pervertex"));
    d->program = program;
    if (!program) {
        if (!flag)
            return;
        program = new QGLShaderProgram();
        program->addShaderFromSourceCode(QGLShader::Vertex, pvColorVertexShader);
        program->addShaderFromSourceCode(QGLShader::Fragment, pvColorFragmentShader);
        program->bindAttributeLocation("vertex", QGL::Position);
        program->bindAttributeLocation("color", QGL::Color);
        if (!program->link()) {
            qWarning("QGLPerVertexColorEffect::setActive(): could not link shader program");
            delete program;
            program = 0;
            return;
        }
        painter->setCachedProgram(QLatin1String("qt.color.pervertex"), program);
        d->program = program;
        d->matrixUniform = program->uniformLocation("matrix");
        program->bind();
        program->enableAttributeArray(QGL::Position);
        program->enableAttributeArray(QGL::Color);
    } else if (flag) {
        program->bind();
        program->enableAttributeArray(QGL::Position);
        program->enableAttributeArray(QGL::Color);
    } else {
        program->disableAttributeArray(QGL::Position);
        program->disableAttributeArray(QGL::Color);
        program->release();
    }
#endif
}

/*!
    \reimp
*/
void QGLPerVertexColorEffect::update
        (QGLPainter *painter, QGLPainter::Updates updates)
{
#if !defined(QGL_SHADERS_ONLY)
    Q_UNUSED(painter);
    Q_UNUSED(updates);
#else
    Q_UNUSED(painter);
    Q_D(QGLPerVertexColorEffect);
    if ((updates & QGLPainter::UpdateMatrices) != 0) {
        d->program->setUniformValue
            (d->matrixUniform, painter->combinedMatrix());
    }
#endif
}

/*!
    \reimp
*/
void QGLPerVertexColorEffect::setVertexAttribute
    (QGL::VertexAttribute attribute, const QGLAttributeValue& value)
{
#if !defined(QGL_SHADERS_ONLY)
    QGLAbstractEffect::setVertexAttribute(attribute, value);
#else
    Q_D(QGLPerVertexColorEffect);
    if (attribute == QGL::Position)
        setAttributeArray(d->program, QGL::Position, value);
    else if (attribute == QGL::Color)
        setAttributeArray(d->program, QGL::Color, value);
#endif
}

QT_END_NAMESPACE
