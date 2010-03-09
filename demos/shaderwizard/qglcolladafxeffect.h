#ifndef QGLCOLLADAEFFECT_H
#define QGLCOLLADAEFFECT_H
#include <QStack>
#include <QList>
#include <QGLShaderProgram>
#include "qglcolladafxeffect_p.h"
#include "qglshaderprogrameffect.h"

class QXmlStreamReader;
class QGLColladaParam;
class QGLColladaFxEffectPrivate;

class QGLColladaFxEffect : public QGLShaderProgramEffect
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

    void setVertexShader(QString const & shader);
    void setFragmentShader(QString const & shader);

    void setLighting(int lighting);
    int lighting();
    QGLTexture2D* diffuseTexture();

private:
    QGLColladaFxEffectPrivate* d;
};

#endif // QGLCOLLADAEFFECT_H
