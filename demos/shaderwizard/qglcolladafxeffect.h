#ifndef QGLCOLLADAEFFECT_H
#define QGLCOLLADAEFFECT_H
#include <QStack>
#include <QList>
#include <QGLShaderProgram>
#include "qglshaderprogrameffect.h"

class QXmlStreamReader;
class QGLColladaParam;
class QGLColladaFxEffectPrivate;

typedef QStack< QList<QGLColladaParam*> > StateStack;

class QGLColladaFxEffect : public QGLShaderProgramEffect
{
    friend class QGLColladaFxEffectFactory;
public:
    QGLColladaFxEffect();
    ~QGLColladaFxEffect();
    void update(QGLPainter *painter, QGLPainter::Updates updates);
    void setSid(QString);
    QString sid();

    QGLTexture2D* diffuseTexture();

private:
    QGLColladaFxEffectPrivate* d;
};

#endif // QGLCOLLADAEFFECT_H
