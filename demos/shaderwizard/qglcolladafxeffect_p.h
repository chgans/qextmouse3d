#ifndef QGLCOLLADAFXEFFECT_P_H
#define QGLCOLLADAFXEFFECT_P_H
#include <QColor>
#include "qgltexture2d.h"

class QGLColladaFxEffectPrivate
{
    friend class QGLColladaFxEffectFactory;
public:
    QGLColladaFxEffectPrivate() : id()
            , sid()
            , name()
            , emissiveTexture(0)
            , ambientTexture(0)
            , diffuseTexture(0)
            , specularTexture(0)
    {
    }

    ~QGLColladaFxEffectPrivate()
    {
    }

    QString id;
    QString sid;
    QString name;

    // The spec allows for 3D textures as well, but for now only 2D is
    // supported
    QGLTexture2D* emissiveTexture;
    QGLTexture2D* ambientTexture;
    QGLTexture2D* diffuseTexture;
    QGLTexture2D* specularTexture;
};

#endif // QGLCOLLADAFXEFFECT_P_H
