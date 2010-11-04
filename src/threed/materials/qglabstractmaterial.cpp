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

#include "qglabstractmaterial.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLAbstractMaterial
    \since 4.8
    \brief The QGLAbstractMaterial class provides a standard interface for rendering surface materials with GL.
    \ingroup qt3d
    \ingroup qt3d::materials

    Materials are the primary method to specify the surface appearance of an
    object, as distinct from the geometry for the object.  Materials have an
    almost endless variety of parameters:

    \list
    \o Properties of the material under various lighting conditions;
       i.e., the traditional parameters for ambient, diffuse, specular, etc.
    \o Textures in multiple layers, with different combination modes;
       decal, modulate, replace, etc.
    \o Environmental conditions such as fogging.
    \o Alpha values for opacity and blending.
    \o Interpolation factors for animated surface effects.
    \o etc
    \endlist

    QGLAbstractMaterial is the base class for all such materials.
    It provides a very simple API to bind() a material to a QGLPainter
    when the material needs to be rendered, to release() a material
    from a QGLPainter when the material is no longer needed, and to
    compare() materials for sorting the render order.

    Subclasses of QGLAbstractMaterial implement specific materials.
    QGLMaterial provides the traditional ambient, diffuse, specular, etc
    parameters for lighting properties.

    Materials are distinct from \i effects, which have the base class
    QGLAbstractEffect.  Effects are typically shader programs that are
    used to render a specific type of material.  The material's bind()
    function will use QGLPainter::setStandardEffect() or
    QGLPainter::setUserEffect() to select the exact effect that is
    needed to render the material.

    It is possible that the same material may be rendered with different
    effects depending upon the material parameters.  For example, a lit
    material may select a simpler and more efficient shader program effect
    if the material has the default spotlight properties, or if the
    QGLPainter only has a single light source specified.

    \sa QGLMaterial, QGLAbstractEffect
*/

/*!
    Constructs a new material and attaches it to \a parent.
*/
QGLAbstractMaterial::QGLAbstractMaterial(QObject *parent)
    : QObject(parent)
{
}

/*!
    Destroys this material.
*/
QGLAbstractMaterial::~QGLAbstractMaterial()
{
}

/*!
    Returns the material lighting parameters for rendering the front
    faces of fragments with this abstract material.  The default
    implementation returns null.

    This function is provided as a convenience for applications that
    wish to alter the lighting parameters or textures of a material,
    without regard to any wrapping that has been performed to add
    blending or other options.

    \sa back(), QGLMaterial
*/
QGLMaterial *QGLAbstractMaterial::front() const
{
    return 0;
}

/*!
    Returns the material lighting parameters for rendering the back
    faces of fragments with this abstract material.  The default
    implementation returns null, which indicates that front()
    is also used to render back faces.

    \sa front(), QGLMaterial
*/
QGLMaterial *QGLAbstractMaterial::back() const
{
    return 0;
}

/*!
    \fn void QGLAbstractMaterial::bind(QGLPainter *painter)

    Binds resources to \a painter that are needed to render this
    material; textures, shader programs, blending modes, etc.

    In the following example, lit material parameters and a texture
    are bound to the \a painter, for rendering with the standard
    QGL::LitModulateTexture2D effect:

    \code
    void TexturedLitMaterial::bind(QGLPainter *painter)
    {
        painter->setStandardEffect(QGL::LitModulateTexture2D);
        painter->setFaceMaterial(QGL::AllFaces, material);
        painter->glActiveTexture(GL_TEXTURE0);
        texture->bind();
    }
    \endcode

    \sa release()
*/

/*!
    \fn void QGLAbstractMaterial::release(QGLPainter *painter, QGLAbstractMaterial *next)

    Releases resources from \a painter that were used to render this
    material.  The QGLPainter::effect() can be left bound to \a painter,
    but other resources such as textures and blending modes
    should be disabled.

    If \a next is not null, then it indicates the next material that
    will be bound to \a painter.  If \a next is the same type of
    material as this material, then this function can choose not to
    release resources that would be immediately re-bound to
    \a painter in the next call to bind().

    In the following example, texture unit 0 is unbound if \a painter
    is about to switch to another effect that is not an instance
    of \c{TexturedLitMaterial}:

    \code
    void TexturedLitMaterial::release(QGLPainter *painter, QGLAbstractMaterial *next)
    {
        if (!qobject_cast<TexturedLitMaterial *>(next)) {
            painter->glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    \endcode

    \sa bind()
*/

/*!
    Compares this material to \a other and returns -1, 0, or 1
    depending upon whether this material is less than, equal to,
    or greater than \a other.  This is used to sort materials
    for more efficient rendering: materials of the same type
    are rendered together to minimize GL state switching.

    The default implementation orders materials based on their
    QObject metaobject type, and orders materials of the same type
    based on their pointer.  Subclasses may override this function
    to provide better ordering for specific types of materials:

    \code
    int MyMaterial::compare(const QGLAbstractMaterial *other) const
    {
        const MyMaterial *mymat = qobject_cast<const MyMaterial *>(other);
        if (!mymat)
            return QGLAbstractMaterial::compare(other);
        if (blendFactor() < mymat->blendFactor())
            return -1;
        else if (blendFactor() > mymat->blendFactor())
            return 1;
        else
            return 0;
    }
    \endcode
*/
int QGLAbstractMaterial::compare(const QGLAbstractMaterial *other) const
{
    const QMetaObject *meta1 = metaObject();
    const QMetaObject *meta2 = other->metaObject();
    if (meta1 < meta2)
        return -1;
    else if (meta1 > meta2)
        return 1;
    if (this < other)
        return -1;
    else if (this > other)
        return 1;
    else
        return 0;
}

/*!
    \fn void QGLAbstractMaterial::materialChanged()

    Signal that is emitted when an object that is using this material
    should be redrawn because some property on the material has changed.
*/

QT_END_NAMESPACE
