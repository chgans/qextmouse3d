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

#include "effect.h"
#include "qglpainter.h"
#include "qglmaterial.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtDeclarative/qdeclarativeengine.h>
#include "private/qdeclarativepixmapcache_p.h"
#include <qdeclarativeinfo.h>

/*!
    \class Effect
    \brief The Effect class defines simple effects within the QML/3d environment.  Examples 
    of such effects include textures, simple material and lighting effects, and so on.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::qml3d

    \section1 Defining an Effect

    Effects can be defined within QML using the normal syntax for creating any QML/3d object.  To create a
    default effect with no extra information, for example, we can simply using the following:

    \code
    Effect {}
    \endcode

    More complex effects use the usual QML syntax for accessing and updating properties.  In order to specify
    a texture, for example, the following could be used:

    \code
    Effect {
        id: myTextureEffect
        texture: "C:\textures\texture.png"
    }
    \endcode
*/

QT_BEGIN_NAMESPACE

class EffectPrivate
{
public:
    EffectPrivate()
        : color(255, 255, 255, 255),
          useLighting(true),
          textureChanged(false),
          texture2D(0),
          material(0),
          declarativePixmap(),
          progress(0.0)
    {
    }

    ~EffectPrivate()
    {
        delete texture2D;
    }

    QColor color;
    bool useLighting;
    bool textureChanged;
    QGLTexture2D *texture2D;
    QUrl textureUrl;
    QGLMaterial *material;

    QDeclarativePixmap declarativePixmap;
    qreal progress;
};

/*!
    Constructs the Effect object with \a parent as its parent.
*/
Effect::Effect(QObject *parent)
    : QObject(parent)
{
    d = new EffectPrivate;
}

/*!
    Destroy the \l Effect object and delete any unneeded data.
*/
Effect::~Effect()
{
    delete d;
}

/*!
    \property Effect::color
    \brief The color of the object defined by a QColor object.

    The default value for this property is \c 255,255,255,255
*/
QColor Effect::color() const
{
    return d->color;
}

void Effect::setColor(const QColor& value)
{
    d->color = value;
    emit effectChanged();
}

/*!
    \property Effect::useLighting
    \brief the lighting control parameter; true if this effect should
    use lighting or false if this effect should use flat colors and
    textures.

    The default value for this true.
*/
bool Effect::useLighting() const
{
    return d->useLighting;
}

void Effect::setUseLighting(bool value)
{
    d->useLighting = value;
    emit effectChanged();
}

/*!
    \property Effect::texture
    \brief Texture effects are defined by this property.  A texture is 
    provided by means of a \l QUrl which is then used for texturing.

    Textures can also be defined directly as images using the \l textureImage
    property.

    \sa textureImage()
*/
QUrl Effect::texture() const
{
    return d->textureUrl;
}

void Effect::setTexture(const QUrl& value)
{
    if (d->textureUrl == value)
        return;

    d->textureUrl = value;
    // got a new value, so abort any in-progress request
    d->declarativePixmap.clear(this);

    if(d->progress != 0.0)
    {
        d->progress = 0.0;
        emit progressChanged(d->progress);
    }

    if (d->textureUrl.isEmpty())
    {
        d->textureChanged = true;
    }
    else
    {
        // Start loading the new texture
        d->textureUrl = value;
        emit effectChanged();

        bool async(true);
#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
        async = d->textureUrl.scheme() != QLatin1String("file");
#endif
        d->declarativePixmap.load(qmlEngine(this), d->textureUrl, async );
        if(d->declarativePixmap.isLoading())
        {
            d->declarativePixmap.connectFinished(this, SLOT(textureRequestFinished()));
            d->declarativePixmap.connectDownloadProgress(this, SLOT(textureRequestProgress(qint64,qint64)));
        }
        else
        {
            if(d->declarativePixmap.isError())
                qWarning() << "Error loading pixmap: " <<                     d->declarativePixmap.error();
            else if(d->declarativePixmap.isReady())
            {
                textureRequestFinished();
            }
        }
    }
}

/*!
    This function is used to emit the appropriate signal when the texture request has finished.

    \sa effectChanged()
*/
void Effect::textureRequestFinished()
{
    QDeclarativePixmap::Status status = d->declarativePixmap.status();
    d->textureChanged = true;
    if(status == QDeclarativePixmap::Ready)
    {
        setTextureImage(d->declarativePixmap.pixmap().toImage());
        if (d->declarativePixmap.pixmap().isNull() ) {
            qWarning() << "Could not load specified texture file";
        }
        d->progress = 1.0;
        emit progressChanged(d->progress);
        emit effectChanged();
    } else if(status != QDeclarativePixmap::Loading)
    {
        qWarning() << "Error getting texture image from cache: "
                << d->declarativePixmap.error();
    }
}

void Effect::textureRequestProgress(qint64 received, qint64 total)
{
    if (d->declarativePixmap.status() == QDeclarativePixmap::Loading && total > 0) {
        d->progress = qreal(received)/total;
        emit progressChanged(d->progress);
    }
}


/*!
    \property Effect::textureImage
    \brief This property directly defines a texture using an existing \l QImage.

    Textures can also be defined based on a URL using the \l texture property.

    \sa texture()
*/
QImage Effect::textureImage() const
{
    // Expensive
    return d->declarativePixmap.pixmap().toImage();
}

void Effect::setTextureImage(const QImage& value)
{
    d->declarativePixmap.setPixmap(QPixmap::fromImage(value));
    d->textureChanged = true;
    emit effectChanged();
}

/*!
    \property Effect::material
    \brief Material parameters are defined in an \l Effect in QML/3d via the
    \c material property.  This specifies a set of \l QGLMaterial which
    are then used when creating the effect.
*/
QGLMaterial *Effect::material() const
{
    return d->material;
}

void Effect::setMaterial(QGLMaterial *value)
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

/*!
  Returns the progress of loading the image for the effect's texture
 */
qreal Effect::progress()
{
    return d->progress;
}

/*!
    Enable the effect on for a given \a painter.
*/
void Effect::enableEffect(QGLPainter *painter)
{
    QGLTexture2D *tex = texture2D();
    if (tex == NULL && d->material)
        tex = d->material->texture();
    if (d->useLighting) {
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
}

/*!
    Disable the effect for a given \a painter.
*/
void Effect::disableEffect(QGLPainter *painter)
{
    painter->setStandardEffect(QGL::FlatColor);
    painter->setColor(Qt::white);
    painter->setTexture((QGLTexture2D *)0);
}

/*! \fn void Effect::effectChanged();

  Signals that a preoperty of the Effect has changed in some way, this may be a texture, material,
  or other parameter.
*/

/*!
    This function returns a \l QGLTexture2D based on the \c texture property of the \l Effect.
*/
QGLTexture2D *Effect::texture2D()
{
    if (d->textureChanged) {
        delete d->texture2D;
        QGLTexture2D *newtex = new QGLTexture2D();
        if (!d->declarativePixmap.pixmap().isNull())
            newtex->setPixmap(d->declarativePixmap.pixmap());
        d->texture2D = newtex;
        d->textureChanged = false;
    }
    return d->texture2D;
}

QT_END_NAMESPACE
