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

#ifndef QGLSTENCILBUFFEROPTIONS_H
#define QGLSTENCILBUFFEROPTIONS_H

#include "qglnamespace.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLStencilBufferOptionsPrivate;
class QGLPainter;

class Q_QT3D_EXPORT QGLStencilBufferOptions
{
public:
    QGLStencilBufferOptions();
    QGLStencilBufferOptions(const QGLStencilBufferOptions& other);
    ~QGLStencilBufferOptions();

    QGLStencilBufferOptions& operator=(const QGLStencilBufferOptions& other);

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

    enum Action {
        Zero            = 0x0000, // GL_ZERO
        Keep            = 0x1E00, // GL_KEEP
        Replace         = 0x1E01, // GL_REPLACE
        Increment       = 0x1E02, // GL_INCR
        Decrement       = 0x1E03, // GL_DECR
        Invert          = 0x150A, // GL_INVERT
        IncrementWrap   = 0x8507, // GL_INCR_WRAP,
        DecrementWrap   = 0x8508  // GL_DECR_WRAP
    };

    inline bool isNull() const { return d == 0; }

    bool isEnabled() const;
    void setEnabled(bool value);

    QGLStencilBufferOptions::Function frontFunction() const;
    void setFrontFunction(QGLStencilBufferOptions::Function value);

    int frontReferenceValue() const;
    void setFrontReferenceValue(int value);

    int frontReferenceMask() const;
    void setFrontReferenceMask(int value);

    int frontWriteMask() const;
    void setFrontWriteMask(int value);

    QGLStencilBufferOptions::Action frontStencilFailAction() const;
    void setFrontStencilFailAction(QGLStencilBufferOptions::Action value);

    QGLStencilBufferOptions::Action frontDepthFailAction() const;
    void setFrontDepthFailAction(QGLStencilBufferOptions::Action value);

    QGLStencilBufferOptions::Action frontPassAction() const;
    void setFrontPassAction(QGLStencilBufferOptions::Action value);

    QGLStencilBufferOptions::Function backFunction() const;
    void setBackFunction(QGLStencilBufferOptions::Function value);

    int backReferenceValue() const;
    void setBackReferenceValue(int value);

    int backReferenceMask() const;
    void setBackReferenceMask(int value);

    int backWriteMask() const;
    void setBackWriteMask(int value);

    QGLStencilBufferOptions::Action backStencilFailAction() const;
    void setBackStencilFailAction(QGLStencilBufferOptions::Action value);

    QGLStencilBufferOptions::Action backDepthFailAction() const;
    void setBackDepthFailAction(QGLStencilBufferOptions::Action value);

    QGLStencilBufferOptions::Action backPassAction() const;
    void setBackPassAction(QGLStencilBufferOptions::Action value);

    void setFunction(QGLStencilBufferOptions::Function value);
    void setReferenceValue(int value);
    void setWriteMask(int value);
    void setStencilFailAction(QGLStencilBufferOptions::Action value);
    void setDepthFailAction(QGLStencilBufferOptions::Action value);
    void setPassAction(QGLStencilBufferOptions::Action value);

    bool operator==(const QGLStencilBufferOptions& other) const;
    bool operator!=(const QGLStencilBufferOptions& other) const;

    void apply(QGLPainter *painter) const;

private:
    QGLStencilBufferOptionsPrivate *d;

    QGLStencilBufferOptionsPrivate *dwrite();
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
