#include <QFile>
#include "projectivetextureeffect.h"
#include <QtOpenGL/qglshaderprogram.h>

static const QMatrix4x4 biasMatrix = QMatrix4x4(0.5, 0.0, 0.0, 0.5,
                                         0.0, 0.5, 0.0, 0.5,
                                         0.0, 0.0, 0.5, 0.5,
                                         0.0, 0.0, 0.0, 1.0);

/*!
  The ProjectiveTextureEffect mimics the effect of shining a projector onto
  a scene from a specific direction.  Vertex coordinates in object space are
  transformed into eye-space coordinates relative to the light direction,
  using the objectLinearTexgenMatrix.
*/
ProjectiveTextureEffect::ProjectiveTextureEffect() :
    matrixDirty(true)
{
    setupShaders();
}

void ProjectiveTextureEffect::setActive(QGLPainter *painter, bool flag)
{
    QGLShaderProgramEffect::setActive(painter, flag);
}

void ProjectiveTextureEffect::update(QGLPainter *painter, QGLPainter::Updates updates)
{
    QGLShaderProgramEffect::update(painter, updates);

    if(matrixDirty)
    {
        recalulateObjectLinearTexgenMatrix();
        matrixDirty = false;
    }

    program()->setUniformValue("objectLinearTexgenMatrix",
                               objectLinearTexgenMatrix);
    program()->setUniformValue("projectorDirection",
                               projectorDirection);
    program()->setUniformValue("qgl_NormalMatrix", painter->normalMatrix());
//     TODO:
//        program()->setUniformValue("inverseCameraModelViewMatrix",
//                                   inverseCameraModelViewMatrix);
}

void ProjectiveTextureEffect::setProjectorDirection(const QVector4D &direction)
{
    this->projectorDirection = direction;
    matrixDirty = true;
}

void ProjectiveTextureEffect::setCameraModelViewMatrix(const QMatrix4x4 &newCameraModelViewMatrix)
{
    cameraModelViewMatrix = newCameraModelViewMatrix;
    bool invertible;
    inverseCameraModelViewMatrix =
            newCameraModelViewMatrix.inverted(&invertible);
    Q_ASSERT(invertible);
    if(!invertible)
        qWarning() << "camera Model view matrix not invertible in ProjectiveDepthTestEffect::setCameraModelViewMatrix()";
    matrixDirty = true;
}

void ProjectiveTextureEffect::setProjectorProjectionMatrix(const QMatrix4x4 &newMatrix)
{
    projectorProjectionMatrix = newMatrix;
    matrixDirty = true;
}

void ProjectiveTextureEffect::setProjectorViewMatrix(const QMatrix4x4 &newMatrix)
{
    projectorViewMatrix = newMatrix;
    matrixDirty = true;
}

void ProjectiveTextureEffect::setModelMatrix(const QMatrix4x4 &newMatrix)
{
    modelMatrix = newMatrix;
    matrixDirty = true;
}

void ProjectiveTextureEffect::recalulateObjectLinearTexgenMatrix()
{
    objectLinearTexgenMatrix = biasMatrix *
                projectorProjectionMatrix *
                projectorViewMatrix *
                modelMatrix;
}

void ProjectiveTextureEffect::setupShaders()
{
    QString vertexShaderFileName = QLatin1String(":/shaders/objectlineartexgen.vert");
    QFile vertexShaderFile(vertexShaderFileName);
    if (vertexShaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray all(vertexShaderFile.readAll());
        setVertexShader(QString::fromLatin1(all.constData(), all.size()));
    } else {
        qWarning() << "Could not open file "<<vertexShaderFileName<<", failed to load vertex shader";
    }

    QString fragmentShaderFileName = QLatin1String(":/shaders/objectlineartexgen.frag");
    QFile fragmentShaderFile(fragmentShaderFileName);
    if (fragmentShaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray all(fragmentShaderFile.readAll());
        setFragmentShader(QString::fromLatin1(all.constData(), all.size()));
    } else {
        qWarning() << "Could not open file "<<fragmentShaderFileName<<", failed to load fragment shader";
    }

}
