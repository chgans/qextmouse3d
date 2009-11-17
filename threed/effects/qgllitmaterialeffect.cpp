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

#include "qgllitmaterialeffect_p.h"
#if defined(QGL_SHADERS_ONLY)
#include <QtOpenGL/qglshaderprogram.h>
#endif
#include <QtCore/qfile.h>

QT_BEGIN_NAMESPACE

QList<QGL::VertexAttribute> QGLLitMaterialEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    fields += QGL::Position;
    fields += QGL::Normal;
    return fields;
}

#if !defined(QGL_SHADERS_ONLY)

QGLLitMaterialEffect::QGLLitMaterialEffect()
{
}

QGLLitMaterialEffect::~QGLLitMaterialEffect()
{
}

void QGLLitMaterialEffect::setActive(bool flag)
{
    if (flag) {
        glEnable(GL_LIGHTING);
        glEnableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY); // Enable when normals set.
    } else {
        glDisable(GL_LIGHTING);
        glDisable(GL_FOG);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
    }
}

void QGLLitMaterialEffect::update
        (QGLPainter *painter, QGLPainter::Updates updates)
{
    updateLighting(painter, updates);
    if ((updates & QGLPainter::UpdateFog) != 0)
        updateFog(painter);
}

void QGLLitMaterialEffect::setVertexArray(const QGLVertexArray& array)
{
    if (setVertexAttribute(QGL::Position, array)) {
        // If we only have positions, then disable the normal array.
        // If the arrays are specified separately, then the positions
        // must be specified before the normals.
        if (setVertexAttribute(QGL::Normal, array))
            glEnableClientState(GL_NORMAL_ARRAY);
        else
            glDisableClientState(GL_NORMAL_ARRAY);
    } else if (setVertexAttribute(QGL::Normal, array)) {
        glEnableClientState(GL_NORMAL_ARRAY);
    }
}

#else // QGL_SHADERS_ONLY

static char const litMaterialVertexShader[] =
    "attribute highp vec4 vertex;\n"
    "attribute highp vec3 normal;\n"
    "uniform mediump mat4 matrix;\n"
    "uniform mediump mat4 modelView;\n"
    "uniform mediump mat3 normalMatrix;\n"
    "void qLightVertex(vec4 vertex, vec3 normal);\n"
    "void main(void)\n"
    "{\n"
    "    gl_Position = matrix * vertex;\n"
    "    highp vec4 vertex = modelView * vertex;\n"
    "    highp vec3 norm = normalize(normalMatrix * normal);\n"
    "    qLightVertex(vertex, norm);\n"
    "}\n";

static char const litMaterialFragmentShader[] =
    "varying mediump vec4 qColor;\n"
    "varying mediump vec4 qSecondaryColor;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "    gl_FragColor = clamp(qColor + vec4(qSecondaryColor.xyz, 0.0), 0.0, 1.0);\n"
    "}\n";

QGLLitMaterialEffect::QGLLitMaterialEffect(const char *vshader, const char *fshader)
{
    program = 0;
    vertexShader = vshader;
    fragmentShader = fshader;
    matrixUniform = -1;
    modelViewUniform = -1;
    normalMatrixUniform = -1;
    textureCoordsAttribute = -1;
}

QGLLitMaterialEffect::QGLLitMaterialEffect()
{
    program = 0;
    vertexShader = litMaterialVertexShader;
    fragmentShader = litMaterialFragmentShader;
    matrixUniform = -1;
    modelViewUniform = -1;
    normalMatrixUniform = -1;
    textureCoordsAttribute = -1;
}

QGLLitMaterialEffect::~QGLLitMaterialEffect()
{
    delete program;
}

static QByteArray createVertexSource(const QString& fileName, const char *extra)
{
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    QByteArray contents = file.readAll();
    return contents + extra;
}

void QGLLitMaterialEffect::setActive(bool flag)
{
    if (!program) {
        if (!flag)
            return;
        program = new QGLShaderProgram();
        program->addShaderFromSourceCode(QGLShader::Vertex, createVertexSource(":/QtOpenGL/shaders/lighting.vsh", vertexShader));
        program->addShaderFromSourceCode(QGLShader::Fragment, fragmentShader);
        program->bindAttributeLocation("vertex", 0);
        program->bindAttributeLocation("normal", 1);
        if (!program->link()) {
            qWarning("QGLFlatTextureEffect::setActive(): could not link shader program");
            delete program;
            program = 0;
            return;
        }
        matrixUniform = program->uniformLocation("matrix");
        modelViewUniform = program->uniformLocation("modelView");
        normalMatrixUniform = program->uniformLocation("normalMatrix");
        textureCoordsAttribute = program->attributeLocation("texcoord");
        program->bind();
        if (textureCoordsAttribute != -1) {
            program->setUniformValue("tex", 0);
            program->enableAttributeArray(textureCoordsAttribute);
        }
        program->enableAttributeArray(0);
        program->enableAttributeArray(1);
    } else if (flag) {
        program->bind();
        if (textureCoordsAttribute != -1) {
            program->setUniformValue("tex", 0);
            program->enableAttributeArray(textureCoordsAttribute);
        }
        program->enableAttributeArray(0);
        program->enableAttributeArray(1);
    } else {
        program->disableAttributeArray(0);
        program->disableAttributeArray(1);
        if (textureCoordsAttribute != -1)
            program->disableAttributeArray(textureCoordsAttribute);
        program->release();
    }
}

static inline QVector4D colorToVector4(const QColor& color)
{
    return QVector4D(color.redF(), color.greenF(),
                     color.blueF(), color.alphaF());
}

void QGLLitMaterialEffect::update(QGLPainter *painter, QGLPainter::Updates updates)
{
    if ((updates & (QGLPainter::UpdateProjectionMatrix |
                    QGLPainter::UpdateModelViewMatrix)) != 0) {
        QMatrix4x4 mv = painter->modelViewMatrix();
        program->setUniformValue(matrixUniform, painter->combinedMatrix());
        program->setUniformValue(modelViewUniform, mv);
        program->setUniformValue(normalMatrixUniform, mv.normalMatrix());
    }
    if ((updates & QGLPainter::UpdateLights) != 0) {
        // Find the first enabled light.
        const QGLLightParameters *lparams = 0;
        QMatrix4x4 ltransform;
        int count = painter->lightCount();
        for (int index = 0; index < count; ++index) {
            if (painter->isLightEnabled(index)) {
                lparams = painter->lightParameters(index);
                ltransform = painter->lightTransform(index);
                break;
            }
        }

        // If no lights are enabled, then use the parameters for light 0.
        if (!lparams)
            lparams = painter->lightParameters(0);

        // Set the uniform variables for the light.
        program->setUniformValue("acli", lparams->ambientColor());
        program->setUniformValue("dcli", lparams->diffuseColor());
        program->setUniformValue("scli", lparams->specularColor());
        program->setUniformValue
            ("sdli", lparams->eyeSpotDirection(ltransform));
        QVector4D pli = lparams->eyePosition(ltransform);
        program->setUniformValue("pli", QVector3D(pli.x(), pli.y(), pli.z()));
        program->setUniformValue("pliw", GLfloat(pli.w()));
        program->setUniformValue("srli", GLfloat(lparams->spotExponent()));
        program->setUniformValue("crli", GLfloat(lparams->spotAngle()));
        program->setUniformValue("ccrli", GLfloat(lparams->spotCosAngle()));
        program->setUniformValue("k0", GLfloat(lparams->constantAttenuation()));
        program->setUniformValue("k1", GLfloat(lparams->linearAttenuation()));
        program->setUniformValue("k2", GLfloat(lparams->quadraticAttenuation()));

        // Set the uniform variables for the light model.
        const QGLLightModel *model = painter->lightModel();
        program->setUniformValue("twoSided", (int)(model->model() == QGLLightModel::TwoSided));
        program->setUniformValue("viewerAtInfinity", (int)(model->viewerPosition() == QGLLightModel::ViewerAtInfinity));
        program->setUniformValue("separateSpecular", (int)(model->colorControl() == QGLLightModel::SeparateSpecularColor));
        program->setUniformValue("acs", model->ambientSceneColor());
    }
    if ((updates & QGLPainter::UpdateMaterials) != 0) {
        // Set the uniform variables for the front and back materials.
        static const int MaxMaterials = 2;
        QVector4D acm[MaxMaterials];
        QVector4D dcm[MaxMaterials];
        QVector4D scm[MaxMaterials];
        QVector4D ecm[MaxMaterials];
        float srm[MaxMaterials];
        const QGLMaterialParameters *mparams = painter->faceMaterial(QGL::FrontFaces);
        acm[0] = colorToVector4(mparams->ambientColor());
        dcm[0] = colorToVector4(mparams->diffuseColor());
        scm[0] = colorToVector4(mparams->specularColor());
        ecm[0] = colorToVector4(mparams->emittedLight());
        srm[0] = (float)(mparams->shininess());
        mparams = painter->faceMaterial(QGL::BackFaces);
        acm[1] = colorToVector4(mparams->ambientColor());
        dcm[1] = colorToVector4(mparams->diffuseColor());
        scm[1] = colorToVector4(mparams->specularColor());
        ecm[1] = colorToVector4(mparams->emittedLight());
        srm[1] = (float)(mparams->shininess());
        program->setUniformValueArray("acm", (const GLfloat *)acm, MaxMaterials, 4);
        program->setUniformValueArray("dcm", (const GLfloat *)dcm, MaxMaterials, 4);
        program->setUniformValueArray("scm", (const GLfloat *)scm, MaxMaterials, 4);
        program->setUniformValueArray("ecm", (const GLfloat *)ecm, MaxMaterials, 4);
        program->setUniformValueArray("srm", srm, MaxMaterials, 1);
    }
}

void QGLLitMaterialEffect::setVertexArray(const QGLVertexArray& array)
{
    QGLAttributeValue value = array.attributeValue(QGL::Position);
    if (!value.isNull()) {
        program->setAttributeArray
            (0, value.floatData(), value.size(), value.stride());
    }
    value = array.attributeValue(QGL::Normal);
    if (!value.isNull()) {
        program->setAttributeArray
            (1, value.floatData(), value.size(), value.stride());
    }
    if (textureCoordsAttribute != -1) {
        value = array.attributeValue(QGL::TextureCoord0);
        if (!value.isNull()) {
            program->setAttributeArray
                (textureCoordsAttribute, value.floatData(),
                 value.size(), value.stride());
        }
    }
}

void QGLLitMaterialEffect::setCommonNormal(const QVector3D& value)
{
    program->disableAttributeArray(1);
    program->setAttributeValue(1, value);
}

#endif // QGL_SHADERS_ONLY

QT_END_NAMESPACE
