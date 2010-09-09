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

    virtual void setObjectLinearTexgenMatrix(const QMatrix4x4 newCameraModelViewMatrix);
    void setCameraModelViewMatrix(const QMatrix4x4 newCameraModelViewMatrix);

//      TODO:
//    QMatrix4x4 eyeLinearTexgenMatrix;
protected:
    virtual void setupShaders();
private:
    QMatrix4x4 objectLinearTexgenMatrix;
    QMatrix4x4 cameraModelViewMatrix;
    QMatrix4x4 inverseCameraModelViewMatrix;
};

#endif // PROJECTIVEDTEXTUREEFFECT_H
