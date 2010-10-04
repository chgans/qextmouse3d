#ifndef QGLCOLLADAEFFECT_H
#define QGLCOLLADAEFFECT_H
#include <QStack>
#include <QList>
#include <QGLShaderProgram>
#include "qglshaderprogrameffect.h"

class QXmlStreamReader;
class QGLColladaParam;
class QGLColladaFxEffectPrivate;

class Q_QT3D_EXPORT QGLColladaFxEffect : public QGLShaderProgramEffect
{
    friend class QGLColladaFxEffectFactory;

public:
    enum Lighting
    {
        NoLighting,
        BlinnLighting,
        PhongLighting,
        ConstantLighting,
        LambertLighting,
        CustomLighting
    };

    QGLColladaFxEffect();
    ~QGLColladaFxEffect();
    void update(QGLPainter *painter, QGLPainter::Updates updates);
    void generateShaders();
    void addBlinnPhongLighting();

    void setId(QString);
    void setSid(QString);
    QString id();
    QString sid();

    void setLighting(int lighting);
    int lighting();
    QGLTexture2D* diffuseTexture();

private:
    QGLColladaFxEffectPrivate* d;
};

#endif // QGLCOLLADAEFFECT_H
