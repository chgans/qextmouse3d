#include <QString>
#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>
#include <QImage>
#include "qgl.h"
#include "qgltexture2d.h"
#include "qglcolladafxeffect.h"
#include "qglcolladafxeffect_p.h"

QGLColladaFxEffect::QGLColladaFxEffect() : QGLShaderProgramEffect()
        , d( new QGLColladaFxEffectPrivate )
{
}



QGLColladaFxEffect::~QGLColladaFxEffect()
{
    delete d;
}



void QGLColladaFxEffect::update(QGLPainter *painter, QGLPainter::Updates updates)
{
    QGLShaderProgramEffect::update(painter, updates);

    if(updates && QGLPainter::UpdateMaterials)
    {
        int textureUnit = 1;

        if(!program)
        {
            qWarning() << "no program in QGLColladaFxEffect::update()";
            return;
        }

        if (d->diffuseTexture != 0)
        {
            painter->setTexture(1, d->diffuseTexture);
            program->setUniformValue("texture1", 1);
            textureUnit++;
        }
        else if( material() != 0 )
        {
            program->setUniformValue("qDiffuse", material()->diffuseColor());
        }
    }
}



void QGLColladaFxEffect::setSid(QString sid)
{
    d->sid = sid;
}



QString QGLColladaFxEffect::sid()
{
    return d->sid;
}



QGLTexture2D* QGLColladaFxEffect::diffuseTexture()
{
    return d->diffuseTexture;
}
