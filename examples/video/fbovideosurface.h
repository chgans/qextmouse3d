/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef FBOVIDEOSURFACE_H
#define FBOVIDEOSURFACE_H

#include <QtMultimedia/QAbstractVideoSurface>
#include <QtMultimedia/QVideoFrame>
#include <QtOpenGL/QGLContext>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLFramebufferObject>

#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>

#ifdef Q_OS_MAC
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#ifndef GL_FRAGMENT_PROGRAM_ARB
#define GL_FRAGMENT_PROGRAM_ARB           0x8804
#define GL_PROGRAM_FORMAT_ASCII_ARB       0x8875
#endif

// ARB_fragment_program
typedef void (APIENTRY *_glProgramStringARB) (GLenum, GLenum, GLsizei, const GLvoid *);
typedef void (APIENTRY *_glBindProgramARB) (GLenum, GLuint);
typedef void (APIENTRY *_glDeleteProgramsARB) (GLsizei, const GLuint *);
typedef void (APIENTRY *_glGenProgramsARB) (GLsizei, GLuint *);
typedef void (APIENTRY *_glProgramLocalParameter4fvARB) (GLenum, GLuint, const GLfloat *);
typedef void (APIENTRY *_glActiveTexture) (GLenum);


class FBOVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    FBOVideoSurface(QGLWidget *widget);
    virtual ~FBOVideoSurface();

    QList<QVideoFrame::PixelFormat> supportedPixelFormats
        (QAbstractVideoBuffer::HandleType handleType) const;

    virtual bool start(const QVideoSurfaceFormat &format);
    virtual bool present(const QVideoFrame &frame);
    virtual void stop();

    QGLFramebufferObject *framebufferObject() const;

private:
    void paintYuvFrame(const QVideoFrame& frame, const QRectF &r);

    QGLWidget *widget;
    QGLFramebufferObject *fbo;

    GLuint programId;
    GLuint textureIds[4];
    int textureCount;
    bool yuvSupported;

    _glProgramStringARB glProgramStringARB;
    _glBindProgramARB glBindProgramARB;
    _glDeleteProgramsARB glDeleteProgramsARB;
    _glGenProgramsARB glGenProgramsARB;
    _glActiveTexture glActiveTexture;
};

#endif

