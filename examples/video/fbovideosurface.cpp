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

#include "fbovideosurface.h"

#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtMultimedia/QAbstractVideoBuffer>
#include <QtMultimedia/QVideoSurfaceFormat>

#ifndef GL_TEXTURE0
# define GL_TEXTURE0    0x84C0
# define GL_TEXTURE1    0x84C1
# define GL_TEXTURE2    0x84C2
#endif

#define Q_ALIGN_VALUE(value, boundary) \
    ((value+boundary-1)/boundary)*boundary

// arbfp1 fragment program for converting yuv to rgb
const char *const yuvToRgb =
    "!!ARBfp1.0"
    "PARAM c[3] = { { 0.5, 0.0625 },"
    "{ 1.164, 0, 1.596, 2.0179999 },"
    "{ 1.164, -0.391, -0.81300002 } };"
    "TEMP R0;"
    "TEMP R1;"
    "TEX R0.x, fragment.texcoord[0], texture[2], 2D;"
    "ADD R1.z, R0.x, -c[0].x;"
    "TEX R1.x, fragment.texcoord[0], texture[0], 2D;"
    "TEX R0.x, fragment.texcoord[0], texture[1], 2D;"
    "ADD R1.x, R1, -c[0].y;"
    "ADD R1.y, R0.x, -c[0].x;"
    "DP3 result.color.x, R1, c[1];"
    "DP3 result.color.y, R1, c[2];"
    "DP3 result.color.z, R1, c[1].xwyw;"
    "END";


FBOVideoSurface::FBOVideoSurface(QGLWidget *glWidget)
    : QAbstractVideoSurface(glWidget)
    , widget(glWidget)
    , fbo(0)
{
    yuvSupported = false;

    glProgramStringARB = reinterpret_cast<_glProgramStringARB>(
            widget->context()->getProcAddress(QLatin1String("glProgramStringARB")));
    glBindProgramARB = reinterpret_cast<_glBindProgramARB>(
            widget->context()->getProcAddress(QLatin1String("glBindProgramARB")));
    glDeleteProgramsARB = reinterpret_cast<_glDeleteProgramsARB>(
            widget->context()->getProcAddress(QLatin1String("glDeleteProgramsARB")));
    glGenProgramsARB = reinterpret_cast<_glGenProgramsARB>(
            widget->context()->getProcAddress(QLatin1String("glGenProgramsARB")));
    glActiveTexture = reinterpret_cast<_glActiveTexture>(
            widget->context()->getProcAddress(QLatin1String("glActiveTexture")));

    bool hasPrograms = glProgramStringARB
            && glBindProgramARB
            && glDeleteProgramsARB
            && glGenProgramsARB
            && glActiveTexture;

    if (hasPrograms) {
        glGenProgramsARB(1, &programId);
        glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, programId);

        glProgramStringARB(
                GL_FRAGMENT_PROGRAM_ARB,
                GL_PROGRAM_FORMAT_ASCII_ARB,
                int(qstrlen(yuvToRgb)),
                reinterpret_cast<const GLbyte *>(yuvToRgb));

        if (glGetError() != GL_NO_ERROR) {
            glDeleteProgramsARB(1, &programId);
        } else {
            yuvSupported = true;
        }
    }
}

FBOVideoSurface::~FBOVideoSurface()
{
}

QList<QVideoFrame::PixelFormat> FBOVideoSurface::supportedPixelFormats
        (QAbstractVideoBuffer::HandleType handleType) const
{
    Q_UNUSED(handleType);
    QList<QVideoFrame::PixelFormat> formats;
    if (yuvSupported)
        formats << QVideoFrame::Format_YUV420P;
    formats << QVideoFrame::Format_RGB32 << QVideoFrame::Format_ARGB32;
    return formats;
}

bool FBOVideoSurface::start(const QVideoSurfaceFormat &format)
{
    if (isFormatSupported(format)) {
        widget->makeCurrent();

        switch (format.pixelFormat()) {
        case QVideoFrame::Format_YUV420P:
            textureCount = 3;
            break;
        case QVideoFrame::Format_RGB32:
        case QVideoFrame::Format_ARGB32:
            textureCount = 1;
            break;
        default:
            return false;
        }

        glGenTextures(textureCount, textureIds);
        int width = 64;
        int height = 64;
        while (width < format.frameWidth())
            width *= 2;
        while (height < format.frameHeight())
            height *= 2;

        fbo = new QGLFramebufferObject(QSize(width, height));

        return QAbstractVideoSurface::start(format);
    } else {
        return false;
    }
}

void FBOVideoSurface::stop()
{
    delete fbo;
    fbo = 0;
    QAbstractVideoSurface::stop();
}

bool FBOVideoSurface::present(const QVideoFrame &frame)
{
    widget->makeCurrent();

    glPushMatrix();
    //QRectF fboGeometry(0, 0, currentFrame.size().width(), currentFrame.size().height());
    QRectF fboGeometry(0, 0, fbo->width(), fbo->height());
    if (fbo->bind()) {
        switch (frame.pixelFormat()) {
        case QVideoFrame::Format_ARGB32:
        case QVideoFrame::Format_RGB32:
            {
                QPainter p(fbo);
                QImage image(frame.bits(),
                             frame.size().width(),
                             frame.size().height(),
                             frame.bytesPerLine(),
                             QImage::Format_RGB32);

                p.drawImage(fboGeometry, image);
            }
            break;
        case QVideoFrame::Format_YUV420P:
            {
                QPainter p(fbo);
                p.fillRect(fboGeometry, Qt::black);
                paintYuvFrame(frame, fboGeometry);
            }
            break;
        default:
            qDebug() << "unsupported color format";
            break;
        }
        fbo->release();
    }

    glPopMatrix();

    widget->update();

    return true;
}

QGLFramebufferObject *FBOVideoSurface::framebufferObject() const
{
    return fbo;
}

void FBOVideoSurface::paintYuvFrame(const QVideoFrame& frame, const QRectF &r)
{
    Q_ASSERT(textureCount == 3);

    const int width = frame.width();
    const int height = frame.height();
    const int bytesPerLine = frame.bytesPerLine();
    const int widths[] = { bytesPerLine, bytesPerLine / 2, bytesPerLine / 2 };
    const int heights[] = { height, height / 2, height / 2 };
    const uchar *data[] = {
            frame.bits(),
            frame.bits() + bytesPerLine * height,
            frame.bits() + bytesPerLine * height * 5 / 4 };

    for (int i = 0; i < 3; ++i) {

        glBindTexture(GL_TEXTURE_2D, textureIds[i]);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_LUMINANCE,
                widths[i],
                heights[i],
                0,
                GL_LUMINANCE,
                GL_UNSIGNED_BYTE,
                data[i]);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

    float txWidth = float(width) / float(Q_ALIGN_VALUE(width, 4));
    float txHeight = float(height) / float(Q_ALIGN_VALUE(height, 4));

    const float tx_array[] = { 0, 0, txWidth, 0, txWidth, txHeight, 0, txHeight};
    const float v_array[] =
        { r.left(), r.top(), r.right(), r.top(), r.right(), r.bottom(), r.left(), r.bottom() };

    glEnable(GL_FRAGMENT_PROGRAM_ARB);
    glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, programId);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureIds[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureIds[1]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureIds[2]);
    glActiveTexture(GL_TEXTURE0);

    glVertexPointer(2, GL_FLOAT, 0, v_array);
    glTexCoordPointer(2, GL_FLOAT, 0, tx_array);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glDisable(GL_FRAGMENT_PROGRAM_ARB);
}

