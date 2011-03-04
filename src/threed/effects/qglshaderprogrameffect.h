/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qglabstracteffect.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QGLShaderProgramEffectPrivate;
class QGLShaderProgram;

class Q_QT3D_EXPORT QGLShaderProgramEffect : public QGLAbstractEffect
{
public:
    QGLShaderProgramEffect();
    virtual ~QGLShaderProgramEffect();

    void setActive(QGLPainter *painter, bool flag);
    void update(QGLPainter *painter, QGLPainter::Updates updates);

    QByteArray vertexShader() const;
    void setVertexShader(const QByteArray &source);
    void setVertexShaderFromFile(const QString &fileName);

    QByteArray fragmentShader() const;
    void setFragmentShader(const QByteArray &source);
    void setFragmentShaderFromFile(const QString &fileName);

    int maximumLights() const;
    void setMaximumLights(int value);

    QGLShaderProgram *program() const;

protected:
    virtual bool beforeLink();
    virtual void afterLink();

private:
    QScopedPointer<QGLShaderProgramEffectPrivate> d_ptr;

    Q_DISABLE_COPY(QGLShaderProgramEffect)
    Q_DECLARE_PRIVATE(QGLShaderProgramEffect)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
