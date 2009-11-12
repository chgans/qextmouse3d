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

#ifndef QGLDEPTHBUFFEROPTIONS_H
#define QGLDEPTHBUFFEROPTIONS_H

#include "qglnamespace.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLDepthBufferOptionsPrivate;
class QGLPainter;

class Q_QT3D_EXPORT QGLDepthBufferOptions
{
public:
    QGLDepthBufferOptions();
    QGLDepthBufferOptions(const QGLDepthBufferOptions& other);
    ~QGLDepthBufferOptions();

    QGLDepthBufferOptions& operator=(const QGLDepthBufferOptions& other);

    enum Function {
        Never           = 0x0200, // GL_NEVER
        Less            = 0x0201, // GL_LESS
        Equal           = 0x0202, // GL_EQUAL
        LessOrEqual     = 0x0203, // GL_LEQUAL
        Greater         = 0x0204, // GL_GREATER
        NotEqual        = 0x0205, // GL_NOTEQUAL
        GreaterOrEqual  = 0x0206, // GL_GEQUAL
        Always          = 0x0207  // GL_ALWAYS
    };

    inline bool isNull() const { return d == 0; }

    bool isEnabled() const;
    void setEnabled(bool value);

    bool isWriteEnabled() const;
    void setWriteEnabled(bool value);

    QGLDepthBufferOptions::Function function() const;
    void setFunction(QGLDepthBufferOptions::Function value);

    qreal nearRange() const;
    qreal farRange() const;
    void setRange(qreal nearRange, qreal farRange);

    bool operator==(const QGLDepthBufferOptions& other) const;
    bool operator!=(const QGLDepthBufferOptions& other) const;

    void apply(QGLPainter *painter) const;

private:
    QGLDepthBufferOptionsPrivate *d;

    QGLDepthBufferOptionsPrivate *dwrite();
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
