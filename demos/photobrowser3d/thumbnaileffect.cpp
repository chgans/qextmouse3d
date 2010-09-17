/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qglflattextureeffect.h"
#include "qglabstracteffect_p.h"
#include "qglext_p.h"

#include <QtOpenGL/qglshaderprogram.h>

class ThumbnailEffectPrivate
{
public:
    ThumbnailEffectPrivate()
        : program(0)
        , matrixUniform(-1)
        , thumbnailUniform(-1)
        , thumbnail(false)
    {
    }
    ~ThumbnailEffectPrivate() { delete program; }

    QGLShaderProgram *program;
    int matrixUniform;
    int thumbnailUniform;
    // true if we render a thumbnail, false do the large size
    bool thumbnail;
};

/*!
    Constructs a new flat texture effect.
*/
ThumbnailEffect::ThumbnailEffect()
    : d(new ThumbnailEffectPrivate)
{
}

/*!
    Destroys this flat texture effect.
*/
ThumbnailEffect::~ThumbnailEffect()
{
    delete d;
}

/*!
    \reimp
*/
QList<QGL::VertexAttribute> ThumbnailEffect::requiredFields() const
{
    QList<QGL::VertexAttribute> fields;
    fields += QGL::Position;
    fields += QGL::TextureCoord0;
    fields += QGL::TextureCoord1;
    return fields;
}

/*!
    \reimp
*/
void ThumbnailEffect::setActive(QGLPainter *painter, bool flag)
{

    Q_UNUSED(painter);
    Q_D(ThumbnailEffect);
    if (!d->program) {
        if (!flag)
            return;
        QGLShaderProgram *program = new QGLShaderProgram();
        program->addShaderFromSourceFile(QGLShader::Vertex, ":/shaders/replace_texture.vsh");
        program->addShaderFromSourceFile(QGLShader::Fragment, ":/shaders/replace_texture.fsh");
        program->bindAttributeLocation("vertex", QGL::Position);
        program->bindAttributeLocation("texcoord", QGL::TextureCoord0);
        program->bindAttributeLocation("thumbcoord", QGL::TextureCoord1);
        if (!program->link()) {
            qWarning("ThumbnailEffect::setActive(): could not link shader program");
            delete program;
            program = 0;
            return;
        }
        d->program = program;
        d->matrixUniform = program->uniformLocation("matrix");
        d->matrixUniform = program->uniformLocation("thumb");
        program->bind();
        program->setUniformValue("texture", 0);
        program->enableAttributeArray(QGL::Position);
        program->enableAttributeArray(QGL::TextureCoord0);
        program->enableAttributeArray(QGL::TextureCoord1);
    } else if (flag) {
        d->matrixUniform = program->uniformLocation("matrix");
        program->bind();
        program->setUniformValue("texture", 0);
        program->enableAttributeArray(QGL::Position);
        program->enableAttributeArray(QGL::TextureCoord0);
        program->enableAttributeArray(QGL::TextureCoord1);
    } else {
        program->disableAttributeArray(QGL::Position);
        program->disableAttributeArray(QGL::TextureCoord0);
        program->disableAttributeArray(QGL::TextureCoord1);
        program->release();
    }
}

/*!
    \reimp
*/
void ThumbnailEffect::update
        (QGLPainter *painter, QGLPainter::Updates updates)
{
    Q_D(ThumbnailEffect);
    if ((updates & QGLPainter::UpdateMatrices) != 0)
    {
        d->program->setUniformValue(d->matrixUniform,
                                    painter->combinedMatrix());
    }
    d->program->setUniformValue(d->thumbUniform, d->thumbnail);
}

/*!
    \reimp
*/
void ThumbnailEffect::setVertexAttribute
    (QGL::VertexAttribute attribute, const QGLAttributeValue& value)
{
    Q_D(ThumbnailEffect);
    if (attribute == QGL::Position)
        setAttributeArray(d->program, QGL::Position, value);
    else if (attribute == QGL::TextureCoord0)
        setAttributeArray(d->program, QGL::TextureCoord0, value);
    else if (attribute == QGL::TextureCoord1)
        setAttributeArray(d->program, QGL::TextureCoord1, value);
}

void ThumbnailEffect::setThumbnail(bool enable)
{
    d->thumbnail = enable;
}

bool ThumbnailEffect::thumbnail() const
{
    return d->thumbnail;
}
