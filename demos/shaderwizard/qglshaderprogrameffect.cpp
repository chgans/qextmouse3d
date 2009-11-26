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

#include "qglshaderprogrameffect.h"
#include "qglabstracteffect.h"
#include <QtOpenGL/qglshaderprogram.h>
#include "qgltexture2d.h"

static char const FallbackPerPixelLightingVertexShader[] =
    "// Per-pixel lighting - vertex shader side.\n"

    "attribute highp vec4 vertex;\n"
    "attribute highp vec4 normal;\n"
    "uniform highp mat4 matrix;\n"
    "uniform mediump mat3 gl_NormalMatrix;\n"

    "uniform mediump vec4 acli;      // Ambient intensity of the light\n"
    "uniform mediump vec4 dcli;      // Diffuse intensity of the light\n"
    "uniform mediump vec4 scli;      // Specular intensity of the light\n"
    "uniform mediump vec3 pli;       // Position of the light\n"
    "uniform mediump vec4 acm;       // Ambient color of the material\n"
    "uniform mediump vec4 dcm;       // Diffuse color of the material\n"
    "uniform mediump vec4 ecm;       // Emissive color of the material\n"
    "uniform mediump vec4 scm;       // specular color of the material\n"
    "uniform mediump vec4 acs;       // Light model's ambient color of the scene\n"
    "uniform bool viewerAtInfinity;  // Light model indicates viewer at infinity\n"

    "uniform float time;\n"

    "varying mediump vec4 qAmbient;\n"
    "varying mediump vec4 qDiffuse;\n"
    "varying mediump vec3 qNormal;\n"
    "varying mediump vec3 qLightDirection;\n"
    "varying mediump vec3 qHalfVector;\n"
    "varying highp vec4 qTexCoord0;\n"
    "varying highp vec4 qTexCoord1;\n"

    "void qLightVertex(vec4 vertex, vec3 normal)\n"
    "{\n"
    "    vec3 toEye;\n"
    "    qAmbient = ecm + acm * acs + acm * acli;\n"
    "    qDiffuse = dcm * dcli;\n"
    "    qLightDirection = normalize(pli);\n"
    "    if (viewerAtInfinity)\n"
    "        toEye = vec3(0, 0, 1);\n"
    "    else\n"
    "        toEye = normalize(-vertex.xyz);\n"
    "    qHalfVector = normalize(qLightDirection + toEye);\n"
    "}\n"

    "void main(void)\n"
    "{\n"
    "    qNormal = normalize(gl_NormalMatrix * vec3(normal));\n"
    "    qLightVertex(vertex, qNormal);\n"
    "    gl_Position = matrix * vertex;\n"
    "};\n";



static char const FallbackPerPixelLightingFragmentShader[] =
    "// Per-pixel lighting - fragment shader side.\n"
    "uniform mediump vec4 scli;\n"
    "uniform mediump vec4 scm;\n"
    "uniform mediump vec4 color;\n"
    "uniform float shininess;\n"
    "varying float intensity;\n"

    "varying mediump vec4 qAmbient;\n"
    "varying mediump vec4 qDiffuse;\n"
    "varying mediump vec3 qNormal;\n"
    "varying mediump vec3 qLightDirection;\n"
    "varying mediump vec3 qHalfVector;\n"
    "varying mediump vec3 qVertexToLight;\n"
    "varying highp vec4 qTexCoord0;\n"
    "varying highp vec4 qTexCoord1;\n"

    "void main(void)\n"
    "{\n"
    "    vec4 specularComponent = vec4( 0.0, 0.0, 0.0, 0.0 );\n"
    "    qNormal = normalize(qNormal);\n"
    "    float intensity =  max(dot(qNormal, qLightDirection), 0.0);\n"
    "    if(intensity > 0.0)\n"
    "    {\n"
    "        float specularIntensity = max( dot(qNormal, qHalfVector), 0.0 );\n"
    "        if(specularIntensity > 0.0)\n"
    "            specularComponent = scm * scli * pow(specularIntensity, shininess);\n"
    "    }\n"
    "    gl_FragColor = qAmbient + qDiffuse * intensity + specularComponent;\n"
    "};\n";

QGLShaderProgramEffect::QGLShaderProgramEffect() : QGLAbstractEffect()
    , program(0)
    , colorUniform(-1)
    , matrixUniform(-1)
    , lightDirectionUniform(-1)
    , textureAttributeSet(false)
    , textureId(0)
{
}

QGLShaderProgramEffect::~QGLShaderProgramEffect()
{
    if(program)
    {
        delete program;
        program = 0;
    }
}

QList<QGL::VertexAttribute> QGLShaderProgramEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    fields += QGL::Position;
    fields += QGL::Normal;
    return fields;
}

bool QGLShaderProgramEffect::supportsPicking() const
{
    return false;
}

void QGLShaderProgramEffect::setActive(bool flag)
{
    if (!program) {
        if(!flag)
            return;
        program = new QGLShaderProgram();
    }

    if(flag)
    {
        reloadShaders();
        bindProgramAttributes();
        if(!program->link())
        {
            qWarning("QGLShaderProgramEffect::setActive(): could not link shader program");
            delete program;
            program = 0;
            currentlyActive = false;
            return;
        }

        bindProgramUniforms();
        program->bind();
        program->enableAttributeArray(0);
        program->enableAttributeArray(1);
        program->enableAttributeArray(2);
        currentlyActive = true;
    } else {
        program->disableAttributeArray(0);
        program->disableAttributeArray(1);
        program->disableAttributeArray(2);
        program->release();
        currentlyActive = false;
    }
}

void QGLShaderProgramEffect::reloadShaders()
{
    if(!program)
        return;

    program->removeAllShaders();

    if(vertexShader.length() > 0)
    {
        program->addShaderFromSourceCode(QGLShader::Vertex, vertexShader);
    }
    else
    {
        program->addShaderFromSourceCode(QGLShader::Vertex, FallbackPerPixelLightingVertexShader);
    }

    if(fragmentShader.length() > 0)
    {
        program->addShaderFromSourceCode(QGLShader::Fragment, fragmentShader);
    }
    else
    {
        program->addShaderFromSourceCode(QGLShader::Fragment, FallbackPerPixelLightingFragmentShader);
    }
}

void QGLShaderProgramEffect::bindProgramAttributes()
{
    if(!program)
        return;
    program->bindAttributeLocation("vertex", 0);
    program->bindAttributeLocation("normal", 1);
    program->bindAttributeLocation("texCoords", 2);
}

void QGLShaderProgramEffect::bindProgramUniforms()
{
    if(!program)
        return;
    colorUniform = program->uniformLocation("color");
    matrixUniform = program->uniformLocation("matrix");
    lightDirectionUniform = program->uniformLocation("lightDir");
    timeUniform = program->uniformLocation("time");
}

void QGLShaderProgramEffect::update(QGLPainter *painter, QGLPainter::Updates updates)
{
    static float time = 0.0f;
    time += 3.14159 / 120; // TODO: use a clock instead of a frame counter
    if(!program)
        return;

    program->setUniformValue(timeUniform, time);

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

        if ((updates & QGLPainter::UpdateLights) != 0 || true ) { //TEMP
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

        // rotate the light for testing purposes
        // ltransform.rotate(time * 30,0.0,1.0,0.0);

        // If no lights are enabled, then use the parameters for light 0.
        if (!lparams)
            lparams = painter->lightParameters(0);

        // Set the uniform variables for the light.
        program->setUniformValue("acli", lparams->ambientColor());
        program->setUniformValue("dcli", lparams->diffuseColor());
        program->setUniformValue("scli", lparams->specularColor());
        program->setUniformValue
                ("sdli",lparams->eyeSpotDirection(ltransform));
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

        // Set the uniform variables for the material
        const QGLMaterialParameters *material = painter->faceMaterial(QGL::FrontFaces);
        program->setUniformValue("acm", material->ambientColor());
        program->setUniformValue("dcm", material->diffuseColor());
        program->setUniformValue("scm", material->specularColor());
        program->setUniformValue("ecm", material->emittedLight());
        program->setUniformValue("shininess", float(material->shininess()));
        program->setUniformValue("texture", textureId);
    }
}

void QGLShaderProgramEffect::setVertexArray(const QGLVertexArray& array)
{
    QGLAttributeValue value = array.attributeValue(QGL::Position);
    if(program && !value.isNull())
    {
        program->setAttributeArray( 0, value.floatData(), value.size(),
                                    value.stride());
    }
    value = array.attributeValue(QGL::Normal);
    if(program && !value.isNull())
    {
        program->setAttributeArray( 1, value.floatData(), value.size(),
                                    value.stride());
    }
    value = array.attributeValue(QGL::TextureCoord0);
    if(program && !value.isNull())
    {
        program->setAttributeArray( 2, value.floatData(), value.size(),
                                    value.stride());
    }
}

void QGLShaderProgramEffect::setVertexShader(QString const &shader)
{
    vertexShader = shader;
    if(shader.length() > 0)
    {
        setActive(true);
    } else {
        setActive(false);
    }
}

void QGLShaderProgramEffect::setFragmentShader(QString const &shader)
{
    fragmentShader = shader;
    if(shader.length() > 0)
    {
        setActive(true);
    } else {
        setActive(false);
    }
}
