#ifndef PROJECTIVEDTEXTUREEFFECT_H
#define PROJECTIVEDTEXTUREEFFECT_H

//#include <QGLShaderProgramEffect>
#include "qglshaderprogrameffect.h"
//#include "deptheffect.h"
#include "qmatrix4x4.h"

class ProjectiveTextureEffect : public QGLShaderProgramEffect
{
public:
    explicit ProjectiveTextureEffect();
    virtual void setActive(QGLPainter *painter, bool flag);
    virtual void update(QGLPainter *painter, QGLPainter::Updates updates);
    virtual void setProjectorDirection(const QVector4D &direction);

    void setCameraModelViewMatrix(const QMatrix4x4 &newCameraModelViewMatrix);
    void setProjectorProjectionMatrix(const QMatrix4x4 &newMatrix);
    void setProjectorViewMatrix(const QMatrix4x4 &newMatrix);
    void setModelMatrix(const QMatrix4x4 &newMatrix);

//      TODO:
//    QMatrix4x4 eyeLinearTexgenMatrix;

protected:
    virtual void setupShaders();
private:
    virtual void recalulateObjectLinearTexgenMatrix();
    bool matrixDirty;
    QMatrix4x4 modelMatrix;
    QMatrix4x4 objectLinearTexgenMatrix;
    QMatrix4x4 cameraModelViewMatrix;
    QMatrix4x4 inverseCameraModelViewMatrix;
    QMatrix4x4 projectorProjectionMatrix;
    QMatrix4x4 projectorViewMatrix;
    QVector4D projectorDirection;
};

#endif // PROJECTIVEDTEXTUREEFFECT_H
