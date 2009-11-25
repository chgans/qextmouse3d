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

#include <QApplication>
#include <QtOpenGL/qgl.h>
#include <QtOpenGL/qglpixelbuffer.h>
#include <QtOpenGL/qglframebufferobject.h>
#include <QtOpenGL/qglshaderprogram.h>
#include <stdio.h>

static void printQtGLVersionInfo()
{
    // Dump what Qt thinks the version is.
    QGLFormat::OpenGLVersionFlags flags = QGLFormat::openGLVersionFlags();
    QByteArray version;
    if ((flags & QGLFormat::OpenGL_Version_1_1) != 0)
        version += "1.1 ";
    if ((flags & QGLFormat::OpenGL_Version_1_2) != 0)
        version += "1.2 ";
    if ((flags & QGLFormat::OpenGL_Version_1_3) != 0)
        version += "1.3 ";
    if ((flags & QGLFormat::OpenGL_Version_1_4) != 0)
        version += "1.4 ";
    if ((flags & QGLFormat::OpenGL_Version_1_5) != 0)
        version += "1.5 ";
    if ((flags & QGLFormat::OpenGL_Version_2_0) != 0)
        version += "2.0 ";
    if ((flags & QGLFormat::OpenGL_Version_2_1) != 0)
        version += "2.1 ";
    if ((flags & QGLFormat::OpenGL_Version_3_0) != 0)
        version += "3.0 ";
    if ((flags & QGLFormat::OpenGL_ES_Common_Version_1_0) != 0)
        version += "ES/1.0 ";
    if ((flags & QGLFormat::OpenGL_ES_CommonLite_Version_1_0) != 0)
        version += "ES/1.0CL ";
    if ((flags & QGLFormat::OpenGL_ES_Common_Version_1_1) != 0)
        version += "ES/1.1 ";
    if ((flags & QGLFormat::OpenGL_ES_CommonLite_Version_1_1) != 0)
        version += "ES/1.1CL ";
    if ((flags & QGLFormat::OpenGL_ES_Version_2_0) != 0)
        version += "ES/2.0 ";
    flags &= ~(QGLFormat::OpenGL_Version_1_1 |
               QGLFormat::OpenGL_Version_1_2 |
               QGLFormat::OpenGL_Version_1_3 |
               QGLFormat::OpenGL_Version_1_4 |
               QGLFormat::OpenGL_Version_1_5 |
               QGLFormat::OpenGL_Version_2_0 |
               QGLFormat::OpenGL_Version_2_1 |
               QGLFormat::OpenGL_Version_3_0 |
               QGLFormat::OpenGL_ES_Common_Version_1_0 |
               QGLFormat::OpenGL_ES_CommonLite_Version_1_0 |
               QGLFormat::OpenGL_ES_Common_Version_1_1 |
               QGLFormat::OpenGL_ES_CommonLite_Version_1_1 |
               QGLFormat::OpenGL_ES_Version_2_0);
    if (flags != 0)
        version += "Other=0x" + QByteArray::number(int(flags), 16);
    printf("QGLFormat::openGLVersionFlags: %s\n", version.constData());
}

static void printBool(const char *tag, bool value)
{
    printf("%s: %s\n", tag, (value ? "yes" : "no"));
}

static void printQtGLFeatures()
{
    printBool("QGLFormat::hasOpenGL", QGLFormat::hasOpenGL());
    printBool("QGLFormat::hasOpenGLOverlays", QGLFormat::hasOpenGLOverlays());
    printBool("QGLPixelBuffer::hasOpenGLPbuffers", QGLPixelBuffer::hasOpenGLPbuffers());
    printBool("QGLFramebufferObject::hasOpenGLFramebufferObjects",
              QGLFramebufferObject::hasOpenGLFramebufferObjects());
    printBool("QGLFramebufferObject::hasOpenGLFramebufferBlit",
              QGLFramebufferObject::hasOpenGLFramebufferBlit());
#if !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
    printBool("QGLShaderProgram::hasOpenGLShaderPrograms",
              QGLShaderProgram::hasOpenGLShaderPrograms());
#endif
}

static void printGLVersionInfo()
{
    printf("\n");
    printf("OpenGL vendor string: %s\n",
           reinterpret_cast<const char *>(glGetString(GL_VENDOR)));
    printf("OpenGL renderer string: %s\n",
           reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
    printf("OpenGL version string: %s\n",
           reinterpret_cast<const char *>(glGetString(GL_VERSION)));
}

static void printGLExtensionInfo()
{
    printf("OpenGL extensions:\n");
    QByteArray extString
        (reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS)));
    QList<QByteArray> extns = extString.split(' ');
    qSort(extns);
    QByteArray line;
    foreach (QByteArray extn, extns) {
        if (extn.isEmpty())
            continue;
        if (!line.isEmpty() && (line.size() + extn.size() + 1) > 70) {
            printf("    %s\n", line.constData());
            line = QByteArray();
        }
        line += extn;
        line += char(' ');
    }
    if (!line.isEmpty())
        printf("    %s\n", line.constData());
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // We need some kind of GL context to do the querying.
    QGLWidget glWidget;
    glWidget.makeCurrent();

    printQtGLVersionInfo();
    printQtGLFeatures();
    printGLVersionInfo();
    printGLExtensionInfo();

    glWidget.doneCurrent();
    return 0;
}
