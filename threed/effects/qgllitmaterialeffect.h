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

#ifndef QGLLITCOLOREFFECT_H
#define QGLLITCOLOREFFECT_H

#include "qglabstracteffect.h"

QT_BEGIN_NAMESPACE

class QGLLitMaterialEffectPrivate;

class Q_QT3D_EXPORT QGLLitMaterialEffect : public QGLAbstractEffect
{
public:
    QGLLitMaterialEffect();
    virtual ~QGLLitMaterialEffect();
   
    QList<QGL::VertexAttribute> requiredFields() const;
    void setActive(QGLPainter *painter, bool flag);
    void update(QGLPainter *painter, QGLPainter::Updates updates);

    void setVertexAttribute
        (QGL::VertexAttribute attribute, const QGLAttributeValue& value);
    void setCommonNormal(const QVector3D& value);

protected:
    QGLLitMaterialEffect
        (GLenum mode, const char *vshader, const char *fshader,
         const QString& programName);

private:
    QScopedPointer<QGLLitMaterialEffectPrivate> d_ptr;

    Q_DECLARE_PRIVATE(QGLLitMaterialEffect)
    Q_DISABLE_COPY(QGLLitMaterialEffect)
};

QT_END_NAMESPACE

#endif
