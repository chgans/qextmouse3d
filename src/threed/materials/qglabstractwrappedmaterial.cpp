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

#include "qglabstractwrappedmaterial.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLAbstractWrappedMaterial
    \since 4.8
    \brief The QGLAbstractWrappedMaterial class represents materials that wrap and modify other materials.
    \ingroup qt3d
    \ingroup qt3d::materials

    Some types of materials are merely modifications on other matierals,
    such as enabling the \c GL_BLEND option.  This class is an abstract
    base to simplify the implementation of such wrapped materials.
    As an example, the following code shows the implementation of
    bind() and release() for QGLBlendMaterial:

    \code
    void QGLBlendMaterial::bind(QGLPainter *painter)
    {
        glEnable(GL_BLEND);
        if (wrap())
            wrap()->bind(painter);
    }

    void QGLBlendMaterial::release(QGLPainter *painter, QGLAbstractMaterial *next)
    {
        if (wrap())
            wrap()->release(painter, next);
        glDisable(GL_BLEND);
    }
    \endcode

    As can be seen, most of the work of binding to the painter is done
    by the underlying wrap() material.

    \sa QGLBlendMaterial
*/

/*!
    Constructs a wrapped material and attaches it to \a parent.
*/
QGLAbstractWrappedMaterial::QGLAbstractWrappedMaterial(QObject *parent)
    : QGLAbstractMaterial(parent), m_wrap(0)
{
}

/*!
    Destroys this wrapped material.
*/
QGLAbstractWrappedMaterial::~QGLAbstractWrappedMaterial()
{
}

/*!
    \property QGLAbstractWrappedMaterial::wrap
    \brief the material that is being wrapped by this object.
    The default is null.

    \sa wrapChanged()
*/

void QGLAbstractWrappedMaterial::setWrap(QGLAbstractMaterial *wrap)
{
    if (m_wrap != wrap) {
        m_wrap = wrap;
        emit wrapChanged();
        emit materialChanged();
    }
}

/*!
    \reimp
*/
int QGLAbstractWrappedMaterial::compare(const QGLAbstractMaterial *other) const
{
    // Bail out if we don't have the same kind of wrapped material.
    if (metaObject() != other->metaObject())
        return QGLAbstractMaterial::compare(other);

    // Compare the wrapped materials.
    QGLAbstractMaterial *wrap1 = m_wrap;
    QGLAbstractMaterial *wrap2 =
        qobject_cast<const QGLAbstractWrappedMaterial *>(other)->m_wrap;
    if (!wrap1)
        return wrap2 ? -1 : 0;
    else if (!wrap2)
        return 1;
    else
        return wrap1->compare(wrap2);
}

/*!
    \fn void QGLAbstractWrappedMaterial::wrapChanged()

    Signal that is emitted when wrap() changes.

    \sa wrap()
*/

QT_END_NAMESPACE
