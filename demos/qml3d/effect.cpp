/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "effect.h"
#include "qglpainter.h"
#include "qglmaterialparameters.h"
#include "qglfogparameters.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtDeclarative/qmlengine.h>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,Effect,Effect)
QML_DEFINE_TYPE(Qt,4,6,Material,QGLMaterialParameters)
QML_DEFINE_TYPE(Qt,4,6,Fog,QGLFogParameters)

class EffectPrivate
{
public:
    EffectPrivate()
        : color(255, 255, 255, 255),
          textureChanged(false),
          texture2D(0),
          material(0),
          fog(0),
          textureReply(0) {}
    ~EffectPrivate()
    {
        delete texture2D;
    }

    QColor color;
    bool textureChanged;
    QImage texture;
    QGLTexture2D *texture2D;
    QUrl textureUrl;
    QGLMaterialParameters *material;
    QGLFogParameters *fog;
    QNetworkReply *textureReply;
};

Effect::Effect(QObject *parent)
    : QObject(parent)
{
    d = new EffectPrivate;
}

Effect::~Effect()
{
    delete d;
}

QColor Effect::color() const
{
    return d->color;
}

void Effect::setColor(const QColor& value)
{
    d->color = value;
    emit effectChanged();
}

QUrl Effect::texture() const
{
    return d->textureUrl;
}

void Effect::setTexture(const QUrl& value)
{
    if (d->textureUrl == value)
        return;
    d->textureUrl = value;

    if (d->textureUrl.isEmpty()) {
        d->texture = QImage();
        d->textureChanged = true;
    } else {

//#define QT_NO_LOCALFILE_OPTIMIZED_QML
#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
        if (d->textureUrl.scheme() == QLatin1String("file")) {
            QString fileName = value.path();
            d->texture = QImage(fileName);
            d->textureChanged = true;
            emit effectChanged();
        } else
#endif
        {
            if (d->textureReply)
                d->textureReply->deleteLater();
            QNetworkRequest req(d->textureUrl);
            req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
            d->textureReply = qmlEngine(this)->networkAccessManager()->get(req);
            QObject::connect(d->textureReply, SIGNAL(finished()),
                             this, SLOT(textureRequestFinished()));
        }
    }
}

void Effect::textureRequestFinished()
{
    d->texture.load(d->textureReply,0);
    d->textureChanged = true;
    d->textureReply->deleteLater();
    d->textureReply = 0;
    emit effectChanged();
}

QImage Effect::textureImage() const
{
    return d->texture;
}

void Effect::setTextureImage(const QImage& value)
{
    d->texture = value;
    d->textureChanged = true;
    emit effectChanged();
}

QGLMaterialParameters *Effect::material() const
{
    return d->material;
}

void Effect::setMaterial(QGLMaterialParameters *value)
{
    if (d->material != value) {
        if (d->material) {
            disconnect(d->material, SIGNAL(materialChanged()),
                       this, SIGNAL(effectChanged()));
        }
        d->material = value;
        if (d->material) {
            connect(d->material, SIGNAL(materialChanged()),
                    this, SIGNAL(effectChanged()));
        }
        emit effectChanged();
    }
}

QGLFogParameters *Effect::fog() const
{
    return d->fog;
}

void Effect::setFog(QGLFogParameters *value)
{
    if (d->fog != value) {
        if (d->fog) {
            disconnect(d->fog, SIGNAL(fogChanged()),
                       this, SIGNAL(effectChanged()));
        }
        d->fog = value;
        if (d->fog) {
            connect(d->fog, SIGNAL(fogChanged()),
                    this, SIGNAL(effectChanged()));
        }
        emit effectChanged();
    }
}

void Effect::enableEffect(QGLPainter *painter)
{
    QGLTexture2D *tex = texture2D();
    if (painter->hasEnabledLights()) {
        if (tex && !tex->isNull()) {
            painter->setStandardEffect(QGL::LitDecalTexture2D);
            painter->setTexture(tex);
        } else {
            painter->setStandardEffect(QGL::LitMaterial);
        }
        if (d->material)
            painter->setFaceMaterial(QGL::AllFaces, d->material);
        else
            painter->setFaceColor(QGL::AllFaces, d->color);
    } else {
        if (tex && !tex->isNull()) {
            painter->setStandardEffect(QGL::FlatDecalTexture2D);
            painter->setTexture(tex);
            painter->setColor(d->color);
        } else {
            painter->setStandardEffect(QGL::FlatColor);
            painter->setColor(d->color);
        }
    }
    painter->setFogParameters(d->fog);
}

void Effect::disableEffect(QGLPainter *painter)
{
    painter->setStandardEffect(QGL::FlatColor);
    painter->setColor(Qt::white);
    painter->setTexture((QGLTexture2D *)0);
    painter->setFogParameters(0);
}

QGLTexture2D *Effect::texture2D()
{
    if (d->textureChanged) {
        delete d->texture2D;
        QGLTexture2D *newtex = new QGLTexture2D();
        if (!d->texture.isNull())
            newtex->setImage(d->texture);
        d->texture2D = newtex;
        d->textureChanged = false;
    }
    return d->texture2D;
}

QT_END_NAMESPACE
