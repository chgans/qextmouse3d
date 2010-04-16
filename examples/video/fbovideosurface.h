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

