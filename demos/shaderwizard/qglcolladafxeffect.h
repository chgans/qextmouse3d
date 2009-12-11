#ifndef QGLCOLLADAEFFECT_H
#define QGLCOLLADAEFFECT_H

#include "qglshaderprogrameffect.h"
class QXmlStreamReader;
class QGLColladaFXEffectPrivate;
class QGLColladaParam;

class QGLColladaFXEffect : public QGLShaderProgramEffect
{
public:
    QGLColladaFXEffect();

    static QList<QGLColladaFXEffect*> loadEffectsFromFile(const QString& fileName );
//    bool loadColladaFX(const QString& filename );

protected:
    static QList<QGLColladaFXEffect*> processLibraryEffectsElement(QXmlStreamReader& xml);
    static QList<QGLColladaFXEffect*> processEffectElement(QXmlStreamReader& xml);
    static QList<QGLColladaFXEffect*> processProfileElement(QXmlStreamReader& xml);
    static QGLColladaFXEffect* processTechniqueElement(QXmlStreamReader& xml);

    static QGLColladaParam* processNewparamElement(QXmlStreamReader& xml);
private:
    QGLColladaFXEffectPrivate* d;
};

#endif // QGLCOLLADAEFFECT_H
