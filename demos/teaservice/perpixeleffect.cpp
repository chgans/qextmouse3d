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

#include "perpixeleffect.h"
#include <QtOpenGL/qglshaderprogram.h>
#include "qglpainter.h"

QT_BEGIN_NAMESPACE

class PerPixelEffectPrivate
{
public:
    PerPixelEffectPrivate()
    {
        program = 0;
        vertexLighting = 0;
        fragmentLighting = 0;
        modelViewUniform = 0;
        matrixUniform = 0;
        normalMatrixUniform = 0;
    }
    ~PerPixelEffectPrivate()
    {
        delete program;
        delete vertexLighting;
        delete fragmentLighting;
    }

    QGLShaderProgram *program;
    QGLShader *vertexLighting;
    QGLShader *fragmentLighting;
    int modelViewUniform;
    int matrixUniform;
    int normalMatrixUniform;
};

PerPixelEffect::PerPixelEffect()
{
    d = new PerPixelEffectPrivate();
}

PerPixelEffect::~PerPixelEffect()
{
    delete d;
}

QList<QGL::VertexAttribute> PerPixelEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    fields += QGL::Position;
    fields += QGL::Normal;
    // There are texture co-ordinates too, but we don't require them.
    return fields;
}

void PerPixelEffect::setActive(QGLPainter *painter, bool flag)
{
    Q_UNUSED(painter);
    if (!flag) {
        if (d->program) {
            d->program->disableAttributeArray(0);
            d->program->disableAttributeArray(1);
            d->program->release();
        }
        return;
    }

    if (!d->program) {
        d->vertexLighting = new QGLShader(QGLShader::Vertex);
        d->vertexLighting->compileSourceFile
            (QLatin1String(":per_pixel_lighting.vsh"));
        d->fragmentLighting = new QGLShader(QGLShader::Fragment);
        d->fragmentLighting->compileSourceFile
            (QLatin1String(":per_pixel_lighting.fsh"));
        d->program = new QGLShaderProgram();
        d->program->addShader(d->vertexLighting);
        d->program->addShader(d->fragmentLighting);
    }
    if (!d->program->isLinked()) {
        d->program->bindAttributeLocation("vertexAttr", 0);
        d->program->bindAttributeLocation("normalAttr", 1);
        d->program->link();
        d->modelViewUniform = d->program->uniformLocation("modelView");
        d->matrixUniform = d->program->uniformLocation("matrix");
        d->normalMatrixUniform = d->program->uniformLocation("normalMatrix");
    }
    d->program->bind();
    d->program->enableAttributeArray(0);
    d->program->enableAttributeArray(1);
}

void PerPixelEffect::update
        (QGLPainter *painter, QGLPainter::Updates updates)
{
    // Update the matrix uniforms.
    if ((updates & QGLPainter::UpdateMatrices) != 0) {
        d->program->setUniformValue(d->matrixUniform, painter->combinedMatrix());
        d->program->setUniformValue(d->modelViewUniform, painter->modelViewMatrix());
        d->program->setUniformValue(d->normalMatrixUniform, painter->normalMatrix());
    }

    // Bail out if the lights or materials have not changed.
    if ((updates & (QGLPainter::UpdateLights | QGLPainter::UpdateMaterials)) == 0)
        return;

    // Find the parameters for the single enabled light.
    const QGLLightParameters *lparams = 0;
    QMatrix4x4 ltransform;
    for (int index = 0; index < painter->lightCount(); ++index) {
        if (painter->isLightEnabled(index)) {
            lparams = painter->lightParameters(index);
            ltransform = painter->lightTransform(index);
            break;
        }
    }
    if (!lparams)
        lparams = painter->lightParameters(0);

    // Get the front material parameters.
    const QGLMaterial *mparams = painter->faceMaterial(QGL::FrontFaces);

    // Set the uniform variables on the shader program.
    d->program->setUniformValue("acli", lparams->ambientColor());
    d->program->setUniformValue("dcli", lparams->diffuseColor());
    d->program->setUniformValue("scli", lparams->specularColor());
    d->program->setUniformValue("sdli", lparams->eyeSpotDirection(ltransform));
    QVector4D pli = lparams->eyePosition(ltransform);
    d->program->setUniformValue("pli", QVector3D(pli.x(), pli.y(), pli.z()));
    d->program->setUniformValue("srli", (GLfloat)(lparams->spotExponent()));
    d->program->setUniformValue("crli", (GLfloat)(lparams->spotAngle()));
    d->program->setUniformValue("ccrli", (GLfloat)(lparams->spotCosAngle()));
    d->program->setUniformValue("acm", mparams->ambientColor());
    d->program->setUniformValue("dcm", mparams->diffuseColor());
    d->program->setUniformValue("scm", mparams->specularColor());
    d->program->setUniformValue("ecm", mparams->emittedLight());
    d->program->setUniformValue("srm", (GLfloat)(mparams->shininess()));
    d->program->setUniformValue("acs", painter->lightModel()->ambientSceneColor());
    d->program->setUniformValue("viewerAtInfinity", (int)(painter->lightModel()->viewerPosition() == QGLLightModel::ViewerAtInfinity));
}

void PerPixelEffect::setVertexAttribute
    (QGL::VertexAttribute attribute, const QGLAttributeValue& value)
{
    if (attribute == QGL::Position)
        setAttributeArray(d->program, 0, value);
    else if (attribute == QGL::Normal)
        setAttributeArray(d->program, 1, value);
}

QT_END_NAMESPACE
