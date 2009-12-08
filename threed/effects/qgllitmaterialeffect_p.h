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

#ifndef QGLLITCOLOREFFECT_P_H
#define QGLLITCOLOREFFECT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qglabstracteffect.h"

QT_BEGIN_NAMESPACE

#if !defined(QGL_SHADERS_ONLY)

class QGLLitMaterialEffect : public QGLAbstractEffect
{
public:
    QGLLitMaterialEffect();
    virtual ~QGLLitMaterialEffect();

    QList<QGL::VertexAttribute> requiredFields() const;

    void setActive(bool flag);

    void update(QGLPainter *painter, QGLPainter::Updates updates);

    void setVertexArray(const QGLVertexArray& array);
};

#else // QGL_SHADERS_ONLY

class QGLShaderProgram;

class QGLLitMaterialEffect : public QGLAbstractEffect
{
public:
    QGLLitMaterialEffect(const char *vshader, const char *fshader);
    QGLLitMaterialEffect();
    virtual ~QGLLitMaterialEffect();

    QList<QGL::VertexAttribute> requiredFields() const;

    void setActive(bool flag);

    void update(QGLPainter *painter, QGLPainter::Updates updates);

    void setVertexArray(const QGLVertexArray& array);
    void setCommonNormal(const QVector3D& value);

protected:
    QGLShaderProgram *program;
    const char *vertexShader;
    const char *fragmentShader;
    int matrixUniform;
    int modelViewUniform;
    int normalMatrixUniform;
    int textureCoordsAttribute;
};

#endif // QGL_SHADERS_ONLY

QT_END_NAMESPACE

#endif
