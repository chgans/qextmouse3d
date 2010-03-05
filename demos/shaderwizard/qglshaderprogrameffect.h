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

#ifndef QGLSHADERPROGRAMEFFECT_H
#define QGLSHADERPROGRAMEFFECT_H

#include <QList>
#include "qglabstracteffect.h"
class QGLShaderProgramEffectPrivate;
class QGLShaderProgram;

class QGLShaderProgramEffect : public QGLAbstractEffect
{
public:
    QGLShaderProgramEffect();
    virtual ~QGLShaderProgramEffect();
    virtual QList<QGL::VertexAttribute> requiredFields() const;
    virtual bool supportsPicking() const;
    virtual void setActive(bool flag);
    virtual bool isActive() { return currentlyActive;}
    virtual void setVertexAttribute(QGL::VertexAttribute attribute,
                            const QGLAttributeValue& value);
    virtual void update(QGLPainter *painter, QGLPainter::Updates updates);

    virtual void setVertexShader(QString const &  shader);
    virtual void setFragmentShader(QString const & shader);
    virtual void setMaterial(QGLMaterialParameters* newMaterial);
    virtual QGLMaterialParameters* material();
    virtual void setProgram(QGLShaderProgram* program);
    virtual QString vertexShader();
    virtual QString fragmentShader();

protected:
    virtual QGLShaderProgram* program();
    virtual void reloadShaders();
    virtual void bindProgramAttributes();

private:
    int colorAttribute;
    bool currentlyActive;
    bool textureAttributeSet;
    int textureId;
    QGLShaderProgramEffectPrivate *d;
};

#endif // QGLSHADERPROGRAMEFFECT_H
