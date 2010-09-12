#include <QFile>
#include "projectivetextureeffect.h"
#include <QtOpenGL/qglshaderprogram.h>

ProjectiveTextureEffect::ProjectiveTextureEffect()
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

    program()->setUniformValue("objectLinearTexgenMatrix",
                               objectLinearTexgenMatrix);
//     TODO:
//        program()->setUniformValue("inverseCameraModelViewMatrix",
//                                   inverseCameraModelViewMatrix);
}

void ProjectiveTextureEffect::setCameraModelViewMatrix(const QMatrix4x4 newCameraModelViewMatrix)
{
    cameraModelViewMatrix = newCameraModelViewMatrix;
    bool invertible;
    inverseCameraModelViewMatrix =
            newCameraModelViewMatrix.inverted(&invertible);
    Q_ASSERT(invertible);
    if(!invertible)
        qWarning() << "camera Model view matrix not invertible in ProjectiveDepthTestEffect::setCameraModelViewMatrix()";
}

void ProjectiveTextureEffect::setObjectLinearTexgenMatrix(const QMatrix4x4 newObjectLinearTexgenMatrix)
{
            objectLinearTexgenMatrix = newObjectLinearTexgenMatrix;
}

void ProjectiveTextureEffect::setupShaders()
{
        QString vertexShaderFileName = ":/shaders/objectlineartexgen.vert";
        QFile vertexShaderFile(vertexShaderFileName);
        if (vertexShaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            setVertexShader(vertexShaderFile.readAll());
        } else {
            qWarning() << "Could not open file "<<vertexShaderFileName<<", failed to load vertex shader";
        }

                QString fragmentShaderFileName = ":/shaders/objectlineartexgen.frag";
        QFile fragmentShaderFile(fragmentShaderFileName);
        if (fragmentShaderFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            setFragmentShader((fragmentShaderFile.readAll()));
        } else {
            qWarning() << "Could not open file "<<fragmentShaderFileName<<", failed to load fragment shader";
        }

}
