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

#include "qgllitmaterialeffect.h"
#include "qglabstracteffect_p.h"
#include <QtOpenGL/qglshaderprogram.h>
#include <QtCore/qfile.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLLitMaterialEffect
    \since 4.7
    \brief The QGLLitMaterialEffect class provides a standard effect that draws fragments with a lit material.
    \ingroup qt3d
    \ingroup qt3d::painting
*/

#if !defined(QGL_FIXED_FUNCTION_ONLY)

static char const litMaterialVertexShader[] =
    "attribute highp vec4 vertex;\n"
    "attribute highp vec3 normal;\n"
    "uniform mediump mat4 matrix;\n"
    "uniform mediump mat4 modelView;\n"
    "uniform mediump mat3 normalMatrix;\n"
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

static QByteArray createVertexSource(const QString& fileName, const char *extra)
{
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    QByteArray contents = file.readAll();
    return contents + extra;
}

static inline QVector4D colorToVector4(const QColor& color)
{
    return QVector4D(color.redF(), color.greenF(),
                     color.blueF(), color.alphaF());
}

// Combine a material and light color into a single color.
static inline QVector4D colorToVector4
    (const QColor &color, const QColor &lightColor)
{
    return QVector4D(color.redF() * lightColor.redF(),
                     color.greenF() * lightColor.greenF(),
                     color.blueF() * lightColor.blueF(),
                     color.alphaF() * lightColor.alphaF());
}

#endif

class QGLLitMaterialEffectPrivate
{
public:
    QGLLitMaterialEffectPrivate()
        : program(0)
        , matrixUniform(-1)
        , modelViewUniform(-1)
        , normalMatrixUniform(-1)
        , textureMode(0)
#if !defined(QGL_FIXED_FUNCTION_ONLY)
        , vertexShader(litMaterialVertexShader)
        , fragmentShader(litMaterialFragmentShader)
#else
        , vertexShader(0)
        , fragmentShader(0)
#endif
        , programName(QLatin1String("qt.color.material"))
        , isFixedFunction(false)
    {
    }

    QGLShaderProgram *program;
    int matrixUniform;
    int modelViewUniform;
    int normalMatrixUniform;
    GLenum textureMode;
    const char *vertexShader;
    const char *fragmentShader;
    QString programName;
    bool isFixedFunction;
};

/*!
    Constructs a new lit material effect.
*/
QGLLitMaterialEffect::QGLLitMaterialEffect()
    : d_ptr(new QGLLitMaterialEffectPrivate)
{
}

/*!
    \internal
*/
QGLLitMaterialEffect::QGLLitMaterialEffect
        (GLenum mode, const char *vshader, const char *fshader,
         const QString& programName)
    : d_ptr(new QGLLitMaterialEffectPrivate)
{
    Q_D(QGLLitMaterialEffect);
    d->textureMode = mode;
    d->vertexShader = vshader;
    d->fragmentShader = fshader;
    d->programName = programName;
}

/*!
    Destroys this lit material effect.
*/
QGLLitMaterialEffect::~QGLLitMaterialEffect()
{
}

/*!
    \reimp
*/
QList<QGL::VertexAttribute> QGLLitMaterialEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    fields += QGL::Position;
    fields += QGL::Normal;
    return fields;
}

/*!
    \reimp
*/
void QGLLitMaterialEffect::setActive(QGLPainter *painter, bool flag)
{
#if defined(QGL_FIXED_FUNCTION_ONLY)
    Q_UNUSED(painter);
    Q_D(QGLLitMaterialEffect);
    if (flag) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY); // Enable when normals set.
        if (d->textureMode) {
            qt_gl_ClientActiveTexture(GL_TEXTURE0);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, d->textureMode);
        }
    } else {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        if (d->textureMode) {
            qt_gl_ClientActiveTexture(GL_TEXTURE0);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }
    }
#else
    Q_UNUSED(painter);
    Q_D(QGLLitMaterialEffect);
#if !defined(QGL_SHADERS_ONLY)
    if (painter->isFixedFunction()) {
        d->isFixedFunction = true;
        if (flag) {
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);
            glEnableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY); // Enable when normals set.
            if (d->textureMode) {
                qt_gl_ClientActiveTexture(GL_TEXTURE0);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, d->textureMode);
            }
        } else {
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
            if (d->textureMode) {
                qt_gl_ClientActiveTexture(GL_TEXTURE0);
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            }
        }
        return;
    }
#endif
    QGLShaderProgram *program = painter->cachedProgram(d->programName);
    d->program = program;
    if (!program) {
        if (!flag)
            return;
        program = new QGLShaderProgram();
#if defined(QT_OPENGL_ES)
        program->addShaderFromSourceCode(QGLShader::Vertex, createVertexSource(":/QtOpenGL/shaders/lighting-embedded.vsh", d->vertexShader));
#else
        program->addShaderFromSourceCode(QGLShader::Vertex, createVertexSource(":/QtOpenGL/shaders/lighting.vsh", d->vertexShader));
#endif
        program->addShaderFromSourceCode(QGLShader::Fragment, d->fragmentShader);
        program->bindAttributeLocation("vertex", QGL::Position);
        program->bindAttributeLocation("normal", QGL::Normal);
        if (d->textureMode != 0)
            program->bindAttributeLocation("texcoord", QGL::TextureCoord0);
        if (!program->link()) {
            qWarning("QGLLitMaterialEffect::setActive(): could not link shader program");
            delete program;
            program = 0;
            return;
        }
        painter->setCachedProgram(d->programName, program);
        d->program = program;
        d->matrixUniform = program->uniformLocation("matrix");
        d->modelViewUniform = program->uniformLocation("modelView");
        d->normalMatrixUniform = program->uniformLocation("normalMatrix");
        program->bind();
        if (d->textureMode != 0) {
            program->setUniformValue("tex", 0);
            program->enableAttributeArray(QGL::TextureCoord0);
        }
        program->enableAttributeArray(QGL::Position);
        program->enableAttributeArray(QGL::Normal);
    } else if (flag) {
        d->matrixUniform = program->uniformLocation("matrix");
        d->modelViewUniform = program->uniformLocation("modelView");
        d->normalMatrixUniform = program->uniformLocation("normalMatrix");
        program->bind();
        if (d->textureMode != 0) {
            program->setUniformValue("tex", 0);
            program->enableAttributeArray(QGL::TextureCoord0);
        }
        program->enableAttributeArray(QGL::Position);
        program->enableAttributeArray(QGL::Normal);
    } else {
        program->disableAttributeArray(QGL::Position);
        program->disableAttributeArray(QGL::Normal);
        if (d->textureMode != 0)
            program->disableAttributeArray(QGL::TextureCoord0);
        program->release();
    }
#endif
}

/*!
    \reimp
*/
void QGLLitMaterialEffect::update
        (QGLPainter *painter, QGLPainter::Updates updates)
{
#if defined(QGL_FIXED_FUNCTION_ONLY)
    painter->updateFixedFunction
        (updates & (QGLPainter::UpdateMatrices |
                    QGLPainter::UpdateLights |
                    QGLPainter::UpdateMaterials));
#else
    Q_D(QGLLitMaterialEffect);
#if !defined(QGL_SHADERS_ONLY)
    if (d->isFixedFunction) {
        painter->updateFixedFunction
            (updates & (QGLPainter::UpdateMatrices |
                        QGLPainter::UpdateLights |
                        QGLPainter::UpdateMaterials));
        return;
    }
#endif
    QGLShaderProgram *program = d->program;
    if ((updates & QGLPainter::UpdateMatrices) != 0) {
        program->setUniformValue(d->matrixUniform, painter->combinedMatrix());
        program->setUniformValue(d->modelViewUniform, painter->modelViewMatrix());
        program->setUniformValue(d->normalMatrixUniform, painter->normalMatrix());
    }
    const QGLLightParameters *lparams = painter->mainLight();
    QMatrix4x4 ltransform = painter->mainLightTransform();
    const QGLLightModel *model = painter->lightModel();
    if ((updates & (QGLPainter::UpdateLights | QGLPainter::UpdateMaterials)) != 0) {
        // Set the uniform variables for the light.
        program->setUniformValue
            ("sdli", lparams->eyeSpotDirection(ltransform).normalized());
        QVector4D pli = lparams->eyePosition(ltransform);
        program->setUniformValue("pli", QVector3D(pli.x(), pli.y(), pli.z()));
        program->setUniformValue("pliw", GLfloat(pli.w()));
        program->setUniformValue("srli", GLfloat(lparams->spotExponent()));
        program->setUniformValue("crli", GLfloat(lparams->spotAngle()));
        program->setUniformValue("ccrli", GLfloat(lparams->spotCosAngle()));
#if !defined(QT_OPENGL_ES)
        // Attenuation is not supported under ES, for performance.
        program->setUniformValue("k0", GLfloat(lparams->constantAttenuation()));
        program->setUniformValue("k1", GLfloat(lparams->linearAttenuation()));
        program->setUniformValue("k2", GLfloat(lparams->quadraticAttenuation()));
#endif

        // Set the uniform variables for the light model.
#if !defined(QT_OPENGL_ES)
        program->setUniformValue("twoSided", (int)(model->model() == QGLLightModel::TwoSided));
#endif
        program->setUniformValue("viewerAtInfinity", (int)(model->viewerPosition() == QGLLightModel::ViewerAtInfinity));

        // Set the uniform variables for the front and back materials.
#if defined(QT_OPENGL_ES)
        static const int MaxMaterials = 1;
#else
        static const int MaxMaterials = 2;
#endif
        QVector4D acm[MaxMaterials];
        QVector4D dcm[MaxMaterials];
        QVector4D scm[MaxMaterials];
        QVector4D ecm[MaxMaterials];
        float srm[MaxMaterials];
        const QGLMaterial *mparams = painter->faceMaterial(QGL::FrontFaces);
        acm[0] = colorToVector4(mparams->ambientColor(), lparams->ambientColor());
        dcm[0] = colorToVector4(mparams->diffuseColor(), lparams->diffuseColor());
        scm[0] = colorToVector4(mparams->specularColor(), lparams->specularColor());
        ecm[0] = colorToVector4(mparams->emittedLight()) +
                 colorToVector4(mparams->ambientColor(),
                                model->ambientSceneColor());
        srm[0] = (float)(mparams->shininess());
#if !defined(QT_OPENGL_ES)
        mparams = painter->faceMaterial(QGL::BackFaces);
        acm[1] = colorToVector4(mparams->ambientColor(), lparams->ambientColor());
        dcm[1] = colorToVector4(mparams->diffuseColor(), lparams->diffuseColor());
        scm[1] = colorToVector4(mparams->specularColor(), lparams->specularColor());
        ecm[1] = colorToVector4(mparams->emittedLight()) +
                 colorToVector4(mparams->ambientColor(),
                                model->ambientSceneColor());
        srm[1] = (float)(mparams->shininess());
#endif
        program->setUniformValueArray("acm", (const GLfloat *)acm, MaxMaterials, 4);
        program->setUniformValueArray("dcm", (const GLfloat *)dcm, MaxMaterials, 4);
        program->setUniformValueArray("scm", (const GLfloat *)scm, MaxMaterials, 4);
        program->setUniformValueArray("ecm", (const GLfloat *)ecm, MaxMaterials, 4);
        program->setUniformValueArray("srm", srm, MaxMaterials, 1);
    }
#endif
}

/*!
    \reimp
*/
void QGLLitMaterialEffect::setVertexAttribute
    (QGL::VertexAttribute attribute, const QGLAttributeValue& value)
{
#if defined(QGL_FIXED_FUNCTION_ONLY)
    if (attribute == QGL::Position) {
        // Disable the normal array when the positions are set because
        // we may need to use the common normal.  Positions always need
        // to be specified before an array of normals.
        glDisableClientState(GL_NORMAL_ARRAY);
    } else if (attribute == QGL::Normal) {
        glEnableClientState(GL_NORMAL_ARRAY);
    }
    QGLAbstractEffect::setVertexAttribute(attribute, value);
#else
    Q_D(QGLLitMaterialEffect);
#if !defined(QGL_SHADERS_ONLY)
    if (d->isFixedFunction) {
        if (attribute == QGL::Position) {
            // Disable the normal array when the positions are set because
            // we may need to use the common normal.  Positions always need
            // to be specified before an array of normals.
            glDisableClientState(GL_NORMAL_ARRAY);
        } else if (attribute == QGL::Normal) {
            glEnableClientState(GL_NORMAL_ARRAY);
        }
        QGLAbstractEffect::setVertexAttribute(attribute, value);
        return;
    }
#endif
    if (attribute == QGL::Position) {
        setAttributeArray(d->program, QGL::Position, value);
    } else if (attribute == QGL::Normal) {
        d->program->enableAttributeArray(QGL::Normal);
        setAttributeArray(d->program, QGL::Normal, value);
    } else if (attribute == QGL::TextureCoord0 && d->textureMode != 0) {
        setAttributeArray(d->program, QGL::TextureCoord0, value);
    }
#endif
}

/*!
    \reimp
*/
void QGLLitMaterialEffect::setCommonNormal(const QVector3D& value)
{
#if defined(QGL_FIXED_FUNCTION_ONLY)
    QGLAbstractEffect::setCommonNormal(value);
#else
    Q_D(QGLLitMaterialEffect);
#if !defined(QGL_SHADERS_ONLY)
    if (d->isFixedFunction) {
        QGLAbstractEffect::setCommonNormal(value);
        return;
    }
#endif
    d->program->disableAttributeArray(QGL::Normal);
    d->program->setAttributeValue(QGL::Normal, value);
#endif
}

QT_END_NAMESPACE
