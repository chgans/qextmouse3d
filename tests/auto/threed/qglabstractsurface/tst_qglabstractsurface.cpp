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

#include <QtTest/QtTest>
#include "qglabstractsurface.h"
#include "qglwidgetsurface.h"
#include "qglframebufferobjectsurface.h"
#include "qglpixelbuffersurface.h"
#include "qglsubsurface.h"
#include "qglfunctions.h"

class tst_QGLAbstractSurface : public QObject
{
    Q_OBJECT
public:
    tst_QGLAbstractSurface() {}
    ~tst_QGLAbstractSurface() {}

private slots:
    void glWidgetSurface();
    void fboSurface();
    void pbufferSurface();
    void subSurface();

private:
    bool isFboCurrent(QGLFramebufferObject *fbo);
    bool isViewport(int x, int y, int width, int height);
    bool isViewport(const QSize &size)
        { return isViewport(0, 0, size.width(), size.height()); }
};

bool tst_QGLAbstractSurface::isFboCurrent(QGLFramebufferObject *fbo)
{
    GLint currentFbo = -1;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFbo);
    if (fbo)
        return fbo->handle() == GLuint(currentFbo);
    else
        return currentFbo == 0;
}

bool tst_QGLAbstractSurface::isViewport(int x, int y, int width, int height)
{
    GLint view[4];
    glGetIntegerv(GL_VIEWPORT, view);
    return view[0] == x && view[1] == y && view[2] == width && view[3] == height;
}

void tst_QGLAbstractSurface::glWidgetSurface()
{
    QGLWidget glw;

    QGLWidgetSurface surface1;
    QVERIFY(surface1.surfaceType() == QGLAbstractSurface::Widget);
    QVERIFY(surface1.widget() == 0);
    surface1.setWidget(&glw);
    QVERIFY(surface1.widget() == &glw);
    QVERIFY(surface1.device() == &glw);
    QCOMPARE(surface1.viewportRect(), glw.rect());

    QVERIFY(surface1.activate());
    QVERIFY(QGLContext::currentContext() == glw.context());
    QVERIFY(isViewport(0, 0, glw.width(), glw.height()));

    surface1.deactivate();
    QVERIFY(QGLContext::currentContext() == glw.context()); // Left current.

    QGLWidget glw2;

    QGLWidgetSurface surface2(&glw2);
    QVERIFY(surface2.surfaceType() == QGLAbstractSurface::Widget);
    QVERIFY(surface2.widget() == &glw2);

    QVERIFY(surface1.activate());
    QVERIFY(surface2.activate(&surface1));

    QVERIFY(QGLContext::currentContext() == glw2.context());

    QVERIFY(surface1.activate(&surface2));
    QVERIFY(QGLContext::currentContext() == glw.context());
}

void tst_QGLAbstractSurface::fboSurface()
{
    if (!QGLFramebufferObject::hasOpenGLFramebufferObjects())
        QSKIP("fbo's are not supported", SkipSingle);

    QGLWidget glw;
    QGLWidgetSurface surface1(&glw);
    QVERIFY(surface1.activate());
    QVERIFY(isViewport(0, 0, glw.width(), glw.height()));

    QGLFramebufferObject fbo1(128, 128);
    QVERIFY(fbo1.handle() != 0);

    QGLFramebufferObjectSurface surface2(&fbo1);
    QVERIFY(surface2.surfaceType() == QGLAbstractSurface::FramebufferObject);
    QVERIFY(surface2.framebufferObject() == &fbo1);

    QVERIFY(isFboCurrent(0));

    QVERIFY(surface2.activate());
    QVERIFY(QGLContext::currentContext() == glw.context());
    QVERIFY(isFboCurrent(&fbo1));
    QVERIFY(isViewport(fbo1.size()));

    surface2.deactivate();
    QVERIFY(isFboCurrent(0));
    QVERIFY(QGLContext::currentContext() == glw.context());

    surface2.setContext(glw.context());

    QGLWidget glw2;
    QGLWidgetSurface surface3(&glw2);
    QVERIFY(surface3.activate());
    QVERIFY(QGLContext::currentContext() == glw2.context());
    QVERIFY(isViewport(0, 0, glw2.width(), glw2.height()));

    QVERIFY(surface2.activate(&surface3));
    QVERIFY(isFboCurrent(&fbo1));
    QVERIFY(QGLContext::currentContext() == glw.context());
    QVERIFY(isViewport(fbo1.size()));

    QVERIFY(surface3.activate(&surface2));
    QVERIFY(isFboCurrent(0));
    QVERIFY(QGLContext::currentContext() == glw2.context());
    QVERIFY(isViewport(0, 0, glw2.width(), glw2.height()));

    surface3.deactivate();

    surface1.activate();
    QVERIFY(isViewport(0, 0, glw.width(), glw.height()));

    QGLFramebufferObject fbo2(256, 256);
    QVERIFY(fbo2.handle() != 0);

    QGLFramebufferObjectSurface surface4;
    QVERIFY(surface4.surfaceType() == QGLAbstractSurface::FramebufferObject);
    QVERIFY(surface4.framebufferObject() == 0);
    surface4.setFramebufferObject(&fbo2);
    QVERIFY(surface4.framebufferObject() == &fbo2);
    surface4.setContext(glw.context());

    QVERIFY(surface2.activate(&surface1));
    QVERIFY(QGLContext::currentContext() == glw.context());
    QVERIFY(isFboCurrent(&fbo1));
    QVERIFY(isViewport(fbo1.size()));

    QVERIFY(surface4.activate(&surface2));
    QVERIFY(QGLContext::currentContext() == glw.context());
    QVERIFY(isFboCurrent(&fbo2));
    QVERIFY(isViewport(fbo2.size()));

    QVERIFY(surface2.activate(&surface3));
    QVERIFY(QGLContext::currentContext() == glw.context());
    QVERIFY(isFboCurrent(&fbo1));
    QVERIFY(isViewport(fbo1.size()));

    QVERIFY(surface1.activate(&surface2));
    QVERIFY(QGLContext::currentContext() == glw.context());
    QVERIFY(isFboCurrent(0));
    QVERIFY(isViewport(0, 0, glw.width(), glw.height()));
}

void tst_QGLAbstractSurface::pbufferSurface()
{
    if (!QGLPixelBuffer::hasOpenGLPbuffers())
        QSKIP("pbuffer's are not supported", SkipSingle);

    QGLWidget glw;
    QGLWidgetSurface surface1(&glw);
    QVERIFY(surface1.activate());
    QVERIFY(isViewport(0, 0, glw.width(), glw.height()));

    QGLPixelBuffer pbuffer1(128, 128);
    QVERIFY(pbuffer1.handle() != 0);

    QGLPixelBufferSurface surface2(&pbuffer1);
    QVERIFY(surface2.surfaceType() == QGLAbstractSurface::PixelBuffer);
    QVERIFY(surface2.pixelBuffer() == &pbuffer1);

    QVERIFY(surface2.activate());
    QVERIFY(QGLContext::currentContext() != glw.context());
    const QGLContext *surface2ctx = QGLContext::currentContext();
    QVERIFY(isViewport(pbuffer1.size()));

    surface2.deactivate();
    QVERIFY(QGLContext::currentContext() == surface2ctx); // Left current.

    QGLWidget glw2;
    QGLWidgetSurface surface3(&glw2);
    QVERIFY(surface3.activate());
    QVERIFY(QGLContext::currentContext() == glw2.context());
    QVERIFY(isViewport(0, 0, glw2.width(), glw2.height()));

    QVERIFY(surface2.activate(&surface3));
    QVERIFY(QGLContext::currentContext() == surface2ctx);
    QVERIFY(isViewport(pbuffer1.size()));

    QVERIFY(surface3.activate(&surface2));
    QVERIFY(QGLContext::currentContext() == glw2.context());
    QVERIFY(isViewport(0, 0, glw2.width(), glw2.height()));

    surface3.deactivate();

    surface1.activate();
    QVERIFY(isViewport(0, 0, glw.width(), glw.height()));

    QGLPixelBuffer pbuffer2(256, 256);
    QVERIFY(pbuffer2.handle() != 0);

    QGLPixelBufferSurface surface4;
    QVERIFY(surface4.surfaceType() == QGLAbstractSurface::PixelBuffer);
    QVERIFY(surface4.pixelBuffer() == 0);
    surface4.setPixelBuffer(&pbuffer2);
    QVERIFY(surface4.pixelBuffer() == &pbuffer2);

    QVERIFY(surface2.activate(&surface1));
    QVERIFY(QGLContext::currentContext() == surface2ctx);
    QVERIFY(isViewport(pbuffer1.size()));

    QVERIFY(surface4.activate(&surface2));
    const QGLContext *surface4ctx = QGLContext::currentContext();
    QVERIFY(surface4ctx != glw.context());
    QVERIFY(surface4ctx != surface2ctx);
    QVERIFY(isViewport(pbuffer2.size()));

    QVERIFY(surface2.activate(&surface3));
    QVERIFY(QGLContext::currentContext() == surface2ctx);
    QVERIFY(isViewport(pbuffer1.size()));

    QVERIFY(surface1.activate(&surface2));
    QVERIFY(QGLContext::currentContext() == glw.context());
    QVERIFY(isViewport(0, 0, glw.width(), glw.height()));
}

void tst_QGLAbstractSurface::subSurface()
{
    QGLWidget glw;
    QGLWidgetSurface surface1(&glw);

    QGLSubsurface surface2(&surface1, QRect(0, 0, 32, 16));
    QVERIFY(surface2.surfaceType() == QGLAbstractSurface::Subsurface);
    QVERIFY(surface2.surface() == &surface1);
    QCOMPARE(surface2.region(), QRect(0, 0, 32, 16));

    QVERIFY(surface2.activate());
    QVERIFY(QGLContext::currentContext() == glw.context());
    QVERIFY(isViewport(0, glw.height() - 16, 32, 16));

    QGLSubsurface surface3(&surface1, QRect(16, 8, 32, 16));
    QVERIFY(surface3.surfaceType() == QGLAbstractSurface::Subsurface);
    QVERIFY(surface3.surface() == &surface1);
    QCOMPARE(surface3.region(), QRect(16, 8, 32, 16));

    QVERIFY(surface3.activate(&surface2));
    QVERIFY(QGLContext::currentContext() == glw.context());
    QVERIFY(isViewport(16, glw.height() - (8 + 16), 32, 16));

    QGLSubsurface surface4;
    QVERIFY(surface4.surfaceType() == QGLAbstractSurface::Subsurface);
    QVERIFY(surface4.surface() == 0);
    QVERIFY(surface4.region().isNull());
    surface4.setSurface(&surface3);
    surface4.setRegion(QRect(4, 6, 12, 8));
    QVERIFY(surface4.surface() == &surface3);
    QCOMPARE(surface4.region(), QRect(4, 6, 12, 8));

    QVERIFY(surface4.activate(&surface2));
    QVERIFY(QGLContext::currentContext() == glw.context());
    QVERIFY(isViewport(16 + 4, glw.height() - (6 + 8 + 8), 12, 8));
}

QTEST_MAIN(tst_QGLAbstractSurface)

#include "tst_qglabstractsurface.moc"
