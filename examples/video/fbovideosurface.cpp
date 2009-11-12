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

#include "fbovideosurface.h"

#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtMultimedia/QAbstractVideoBuffer>

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

    QList<QVideoFrame::Type> types;
    //types << QVideoFormat::GLTexture;
    if (yuvSupported)
        types << QVideoFrame::Image_YUV420P;
    types << QVideoFrame::Image_RGB32 << QVideoFrame::Image_ARGB32;
    setSupportedTypes(types);
}

FBOVideoSurface::~FBOVideoSurface()
{
}

bool FBOVideoSurface::start(const QVideoFormat &format)
{
    if (isFormatSupported(format)) {
        widget->makeCurrent();

        switch (format.frameType()) {
        case QVideoFrame::Image_YUV420P:
            textureCount = 3;
            break;
        case QVideoFrame::Image_RGB32:
        case QVideoFrame::Image_ARGB32:
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

        setFormat(format);
        setStarted(true);

        qDebug() << "FBOVideoSurface::initialize" << format;

        return true;
    } else {
        return false;
    }
}

void FBOVideoSurface::stop()
{
    qDebug() << "FBOVideoSurface::shutdown";

    setStarted(false);
    delete fbo;
    fbo = 0;
}

bool FBOVideoSurface::present(const QVideoFrame &frame)
{
    widget->makeCurrent();

    glPushMatrix();
    //QRectF fboGeometry(0, 0, currentFrame.size().width(), currentFrame.size().height());
    QRectF fboGeometry(0, 0, fbo->width(), fbo->height());
    if (fbo->bind()) {
        switch (frame.type()) {
        case QVideoFrame::GLTexture:
            //TODO: implement
            break;
        case QVideoFrame::Image_ARGB32:
        case QVideoFrame::Image_RGB32:
            {
                QPainter p(fbo);
                QImage image(
                        reinterpret_cast<uchar *>(frame.buffer()->data()),
                        frame.size().width(),
                        frame.size().height(),
                        QImage::Format_RGB32);

                p.drawImage(fboGeometry, image);
            }
            break;
        case QVideoFrame::Image_YUV420P:
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

