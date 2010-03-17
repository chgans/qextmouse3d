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

#include "qglmaterial.h"
#include "qglmaterial_p.h"
#include "qglpainter.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLMaterial
    \brief The QGLMaterial class describes material properties for OpenGL drawing.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::painting
*/

QGLMaterialPrivate::QGLMaterialPrivate(int version)
    : QObjectPrivate(version),
      basicColor(255, 255, 255, 255),
      specularColor(0, 0, 0, 255),
      emittedLight(0, 0, 0, 255),
      shininess(0),
      collection(0),
      index(-1),
      used(false)
{
    ambientColor.setRgbF(0.2f, 0.2f, 0.2f, 1.0f);
    diffuseColor.setRgbF(0.8f, 0.8f, 0.8f, 1.0f);
}

/*!
    Constructs a QGLMaterial object with its default values,
    and attaches it to \a parent.
*/
QGLMaterial::QGLMaterial(QObject *parent)
    : QObject(*new QGLMaterialPrivate(), parent)
{
}

/*!
    Destroys this QGLMaterial object.
*/
QGLMaterial::~QGLMaterial()
{
}

/*!
    \property QGLMaterial::basicColor
    \brief the basic color of the material.  The default value
    is (1.0, 1.0, 1.0, 1.0).

    The basic color is used by effects that don't implement
    material-based lighting.  It is ignored by material-based
    lighting effects.

    \sa ambientColor(), diffuseColor(), basicColorChanged(), setColor()
*/
QColor QGLMaterial::basicColor() const
{
    Q_D(const QGLMaterial);
    return d->basicColor;
}

void QGLMaterial::setBasicColor(const QColor& value)
{
    Q_D(QGLMaterial);
    if (d->basicColor != value) {
        d->basicColor = value;
        emit basicColorChanged();
        emit materialChanged();
    }
}

/*!
    \property QGLMaterial::ambientColor
    \brief the ambient color of the material.  The default value
    is (0.2, 0.2, 0.2, 1.0).

    \sa diffuseColor(), specularColor(), ambientColorChanged()
*/
QColor QGLMaterial::ambientColor() const
{
    Q_D(const QGLMaterial);
    return d->ambientColor;
}

void QGLMaterial::setAmbientColor(const QColor& value)
{
    Q_D(QGLMaterial);
    if (d->ambientColor != value) {
        d->ambientColor = value;
        emit ambientColorChanged();
        emit materialChanged();
    }
}

/*!
    \property QGLMaterial::diffuseColor
    \brief the diffuse color of the material.  The default value
    is (0.8, 0.8, 0.8, 1.0).

    \sa ambientColor(), specularColor(), diffuseColorChanged()
*/
QColor QGLMaterial::diffuseColor() const
{
    Q_D(const QGLMaterial);
    return d->diffuseColor;
}

void QGLMaterial::setDiffuseColor(const QColor& value)
{
    Q_D(QGLMaterial);
    if (d->diffuseColor != value) {
        d->diffuseColor = value;
        emit diffuseColorChanged();
        emit materialChanged();
    }
}

/*!
    \property QGLMaterial::specularColor
    \brief the specular color of the material.  The default value
    is (0, 0, 0, 1).

    \sa ambientColor(), diffuseColor(), specularColorChanged()
*/
QColor QGLMaterial::specularColor() const
{
    Q_D(const QGLMaterial);
    return d->specularColor;
}

void QGLMaterial::setSpecularColor(const QColor& value)
{
    Q_D(QGLMaterial);
    if (d->specularColor != value) {
        d->specularColor = value;
        emit specularColorChanged();
        emit materialChanged();
    }
}

/*!
    \property QGLMaterial::emittedLight
    \brief the emitted light intensity of the material.
    The default value is (0.0, 0.0, 0.0, 1.0), which indicates
    that the material does not emit any light.

    \sa emittedLightChanged()
*/
QColor QGLMaterial::emittedLight() const
{
    Q_D(const QGLMaterial);
    return d->emittedLight;
}

void QGLMaterial::setEmittedLight(const QColor& value)
{
    Q_D(QGLMaterial);
    if (d->emittedLight != value) {
        d->emittedLight = value;
        emit emittedLightChanged();
        emit materialChanged();
    }
}

/*!
    \property QGLMaterial::color
    \brief the overall color of the material.  The default value
    is (1.0, 1.0, 1.0, 1.0).

    This is a convenience property.  When read it returns basicColor().
    When written, it sets basicColor() to the value, ambientColor()
    to 20% of the value, and diffuseColor() to 80% of the value.
    The result is that regardless of whether lighting is used or not,
    the material will appear to have a similar color.

    \sa basicColor(), ambientColor(), diffuseColor()
*/
QColor QGLMaterial::color() const
{
    Q_D(const QGLMaterial);
    return d->basicColor;
}

void QGLMaterial::setColor(const QColor& value)
{
    Q_D(QGLMaterial);
    d->basicColor = value;
    d->ambientColor.setRgbF
        (value.redF() * 0.2f, value.greenF() * 0.2f,
         value.blueF() * 0.2f, value.alphaF());
    d->diffuseColor.setRgbF
        (value.redF() * 0.8f, value.greenF() * 0.8f,
         value.blueF() * 0.8f, value.alphaF());
    emit basicColorChanged();
    emit ambientColorChanged();
    emit diffuseColorChanged();
    emit materialChanged();
}

/*!
    \property QGLMaterial::shininess
    \brief the specular exponent of the material, or how shiny it is.
    Must be between 0 and 128.  The default value is 0.  A value outside
    this range will be clamped to the range when the property is set.

    \sa shininessChanged()
*/
qreal QGLMaterial::shininess() const
{
    Q_D(const QGLMaterial);
    return d->shininess;
}

void QGLMaterial::setShininess(qreal value)
{
    Q_D(QGLMaterial);
    if (value < 0)
        value = 0;
    else if (value > 128)
        value = 128;
    if (d->shininess != value) {
        d->shininess = value;
        emit shininessChanged();
        emit materialChanged();
    }
}

/*!
    \property QGLMaterial::texture
    \brief the 2D texture associated with \a layer on this material;
    null if no texture.

    Layer 0 is normally the primary texture associated with the material.
    Multiple texture layers may be specified for materials with special
    blending effects or to specify ambient, diffuse, or specular colors
    pixel-by-pixel.

    \sa texturesChanged(), bindTextures()
*/
QGLTexture2D *QGLMaterial::texture(int layer) const
{
    Q_D(const QGLMaterial);
    return d->textures.value(layer, 0);
}

void QGLMaterial::setTexture(QGLTexture2D *value, int layer)
{
    Q_ASSERT(layer >= 0);
    Q_D(QGLMaterial);
    QGLTexture2D *prev = d->textures.value(layer, 0);
    if (prev != value) {
        d->textures[layer] = value;
        emit texturesChanged();
        emit materialChanged();
    }
}

/*!
    Returns the number of texture layers associated with this material.

    The return value may be larger than the number of actual texture
    layers if some of the intermediate layers are null.  For example,
    setting layers 0 and 2 will report textureLayerCount() as 3.
    The main use of this value is to iterate over all layers.

    \sa texture()
*/
int QGLMaterial::textureLayerCount() const
{
    Q_D(const QGLMaterial);
    int maxLayer = -1;
    if (!d->textures.isEmpty())
        maxLayer = qMax(maxLayer, (d->textures.end() - 1).key());
    return maxLayer + 1;
}

/*!
    Binds the textures for this material to the texture units on
    \a painter corresponding to their layer numbers.

    \sa texture(), releaseTextures()
*/
void QGLMaterial::bindTextures(QGLPainter *painter)
{
    Q_D(QGLMaterial);
    QMap<int, QGLTexture2D *>::ConstIterator it1;
    for (it1 = d->textures.constBegin(); it1 != d->textures.constEnd(); ++it1) {
        if (it1.value())
            painter->setTexture(it1.key(), it1.value());
    }
}

/*!
    Releases the textures for this material from the texture units
    on \a painter.

    \sa bindTextures()
*/
void QGLMaterial::releaseTextures(QGLPainter *painter)
{
    Q_D(QGLMaterial);
    QMap<int, QGLTexture2D *>::ConstIterator it1;
    for (it1 = d->textures.constBegin(); it1 != d->textures.constEnd(); ++it1) {
        if (it1.value())
            painter->setTexture(it1.key(), static_cast<QGLTexture2D *>(0));
    }
}

/*!
    \fn void QGLMaterial::basicColorChanged()

    This signal is emitted when basicColor() changes.

    \sa basicColor(), setBasicColor(), materialChanged()
*/

/*!
    \fn void QGLMaterial::ambientColorChanged()

    This signal is emitted when ambientColor() changes.

    \sa ambientColor(), setAmbientColor(), materialChanged()
*/

/*!
    \fn void QGLMaterial::diffuseColorChanged()

    This signal is emitted when diffuseColor() changes.

    \sa diffuseColor(), setDiffuseColor(), materialChanged()
*/

/*!
    \fn void QGLMaterial::specularColorChanged()

    This signal is emitted when specularColor() changes.

    \sa specularColor(), setSpecularColor(), materialChanged()
*/

/*!
    \fn void QGLMaterial::emittedLightChanged()

    This signal is emitted when emittedLight() changes.

    \sa emittedLight(), setEmittedLight(), materialChanged()
*/

/*!
    \fn void QGLMaterial::shininessChanged()

    This signal is emitted when shininess() changes.

    \sa shininess(), setShininess(), materialChanged()
*/

/*!
    \fn void QGLMaterial::texturesChanged()

    This signal is emitted when the texture layers associated with
    this material change.

    \sa texture(), setTexture(), materialChanged()
*/

/*!
    \fn void QGLMaterial::materialChanged()

    This signal is emitted when one of basicColor(), ambientColor(),
    diffuseColor(), specularColor(), emittedLight(), shiniess(),
    or texture() changes.

    \sa basicColorChanged(), ambientColorChanged(), diffuseColorChanged()
    \sa specularColorChanged(), emittedLightChanged(), shininessChanged()
    \sa texturesChanged()
*/

QT_END_NAMESPACE
