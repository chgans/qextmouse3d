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
#include <QtOpenGL/qgl.h>
#include <QtGui/qpainter.h>
#include "qgltestwidget.h"
#include "qglpainter.h"
#include "qglsimulator.h"
#include "qglflatcoloreffect.h"

class tst_QGLPainter : public QObject
{
    Q_OBJECT
public:
    tst_QGLPainter() : widget(0) {}
    ~tst_QGLPainter() {}

private slots:
    void initTestCase();
    void cleanupTestCase();
    void clear();
    void drawTriangle();
    void scissor();
    void userMatrixStack();
    void projectionMatrixStack();
    void modelViewMatrixStack();

public slots:
    void clearPaint();
    void clearPaintQ(QPainter *painter, const QSize& size);
    void drawTrianglePaint();
    void drawTrianglePaintQ(QPainter *painter, const QSize& size);
    void scissorPaint();
    void scissorPaintQ(QPainter *painter, const QSize& size);

private:
    QGLTestWidget *widget;
};

void tst_QGLPainter::initTestCase()
{
    widget = new QGLTestWidget();
}

void tst_QGLPainter::cleanupTestCase()
{
    delete widget;
}

void tst_QGLPainter::clear()
{
    QVERIFY(widget->runTest(this, "clearPaint"));
}

void tst_QGLPainter::clearPaint()
{
    QGLPainter painter;
    painter.begin();
    painter.setClearColor(Qt::blue);
    painter.clear();
}

void tst_QGLPainter::clearPaintQ(QPainter *painter, const QSize& size)
{
    QGLSimulator sim(painter, size);
    sim.setClearColor(Qt::blue);
    sim.clear();
}

void tst_QGLPainter::drawTriangle()
{
    QVERIFY(widget->runTest(this, "drawTrianglePaint"));
}

void tst_QGLPainter::drawTrianglePaint()
{
    QGLPainter painter;
    painter.begin();
    painter.setClearColor(Qt::black);
    painter.clear();

    painter.projectionMatrix().setToIdentity();
    painter.projectionMatrix().ortho(widget->rect());
    painter.modelViewMatrix().setToIdentity();

    QVector2DArray vertices;
    vertices.append(10, 100);
    vertices.append(500, 100);
    vertices.append(500, 500);

    QGLFlatColorEffect effect;
    painter.setUserEffect(&effect);
    painter.setColor(Qt::green);
    painter.setVertexAttribute(QGL::Position, vertices);
    painter.draw(QGL::Triangles, 3);
    painter.setUserEffect(0);
}

void tst_QGLPainter::drawTrianglePaintQ(QPainter *painter, const QSize& size)
{
    QGLSimulator sim(painter, size);

    sim.clear();
    sim.setColor(Qt::green);

    QMatrix4x4 proj;
    proj.ortho(widget->rect());
    sim.setProjectionMatrix(proj);

    QVector2DArray vertices;
    vertices.append(10, 100);
    vertices.append(500, 100);
    vertices.append(500, 500);

    sim.drawTriangles(vertices);
}

static QRect fetchGLScissor(const QRect& windowRect)
{
    GLint scissor[4];
    glGetIntegerv(GL_SCISSOR_BOX, scissor);
    if (scissor[2] != 0 && scissor[3] != 0) {
        return QRect(scissor[0],
                     windowRect.height() - (scissor[1] + scissor[3]),
                     scissor[2], scissor[3]);
    } else {
        // Normally should be (0, 0, 0, 0) - don't adjust for window height.
        return QRect(scissor[0], scissor[1], scissor[2], scissor[3]);
    }
}

void tst_QGLPainter::scissor()
{
    // Run a painting test to check that the scissor works.
    QVERIFY(widget->runTest(this, "scissorPaint"));

    // Perform some lower level tests to ensure that the
    // GL state is updated properly as the scissor changes.
    QGLPainter painter;
    painter.begin(widget);
    QRect windowRect = widget->rect();

    QVERIFY(painter.scissor().isNull());
    QVERIFY(!glIsEnabled(GL_SCISSOR_TEST));

    painter.setScissor(windowRect);
    QCOMPARE(painter.scissor(), windowRect);
    QVERIFY(glIsEnabled(GL_SCISSOR_TEST));
    QCOMPARE(fetchGLScissor(windowRect), windowRect);

    QRect subRect(windowRect.width() / 3,
                  windowRect.height() / 3,
                  2 * windowRect.width() / 3,
                  2 * windowRect.height() / 3);
    painter.setScissor(subRect);
    QCOMPARE(painter.scissor(), subRect);
    QVERIFY(glIsEnabled(GL_SCISSOR_TEST));
    QCOMPARE(fetchGLScissor(windowRect), subRect);

    QRect leftHalf(0, 0, windowRect.width() / 2, windowRect.height());
    painter.intersectScissor(leftHalf);
    QCOMPARE(painter.scissor(), subRect.intersected(leftHalf));
    QVERIFY(glIsEnabled(GL_SCISSOR_TEST));
    QCOMPARE(fetchGLScissor(windowRect), subRect.intersected(leftHalf));

    QRect rightHalf(windowRect.width() - windowRect.width() / 2, 0,
                    windowRect.width() / 2, windowRect.height());
    QRect expandedRect(subRect.x(), 0, windowRect.width() - subRect.x(),
                       windowRect.height());
    painter.expandScissor(rightHalf);
    QCOMPARE(painter.scissor(), expandedRect);
    QVERIFY(glIsEnabled(GL_SCISSOR_TEST));
    QCOMPARE(fetchGLScissor(windowRect), expandedRect);

    // QRect(0, 0, -2, -2) is a special value indicating "clip everything".
    painter.setScissor(QRect(0, 0, -2, -2));
    QCOMPARE(painter.scissor(), QRect(0, 0, -2, -2));
    QVERIFY(glIsEnabled(GL_SCISSOR_TEST));
    QCOMPARE(fetchGLScissor(windowRect), QRect(0, 0, 0, 0));

    painter.setScissor(subRect);
    painter.setScissor(QRect());
    QCOMPARE(painter.scissor(), QRect());
    QVERIFY(!glIsEnabled(GL_SCISSOR_TEST));
    QCOMPARE(fetchGLScissor(windowRect), subRect);

    painter.setScissor(windowRect);
    glScissor(subRect.x(),
              windowRect.height() - (subRect.y() + subRect.height()),
              subRect.width(), subRect.height());
    painter.resetScissor();
    QCOMPARE(painter.scissor(), subRect);
    QVERIFY(glIsEnabled(GL_SCISSOR_TEST));
    QCOMPARE(fetchGLScissor(windowRect), subRect);

    glDisable(GL_SCISSOR_TEST);
    painter.resetScissor();
    QVERIFY(!glIsEnabled(GL_SCISSOR_TEST));
    QCOMPARE(fetchGLScissor(windowRect), subRect);
    QCOMPARE(painter.scissor(), QRect());
}

void tst_QGLPainter::scissorPaint()
{
    QGLPainter painter;
    painter.begin();
    painter.setClearColor(Qt::black);
    painter.clear();

    painter.projectionMatrix().setToIdentity();
    painter.projectionMatrix().ortho(widget->rect());
    painter.modelViewMatrix().setToIdentity();

    QVector2DArray vertices;
    vertices.append(10, 100);
    vertices.append(500, 100);
    vertices.append(500, 500);

    // Paint a green triangle.
    QGLFlatColorEffect effect;
    painter.setUserEffect(&effect);
    painter.setColor(Qt::green);
    painter.setVertexAttribute(QGL::Position, vertices);
    painter.draw(QGL::Triangles, 3);

    // Change the top part of the triangle to blue.
    painter.setScissor
        (QRect(0, 0, widget->width(), qMin(widget->height() / 2, 200)));
    painter.setColor(Qt::blue);
    painter.draw(QGL::Triangles, 3);

    // Intersect and draw red over the blue section.
    painter.intersectScissor
        (QRect(0, 0, widget->width(), qMin(widget->height() / 4, 150)));
    painter.setColor(Qt::red);
    painter.draw(QGL::Triangles, 3);

    // Change the bottom part of the triangle to yellow.
    int y = qMin(widget->height() / 2, 350);
    painter.setScissor
        (QRect(0, y, 400, widget->height() - y));
    painter.setColor(Qt::yellow);
    painter.draw(QGL::Triangles, 3);

    // Intersect and expand, to extend the yellow region.
    painter.intersectScissor
        (QRect(0, y + 20, 400, widget->height() - y - 20));
    painter.expandScissor
        (QRect(0, y + 20, 450, widget->height() - y - 20));
    painter.setColor(Qt::yellow);
    painter.draw(QGL::Triangles, 3);

    painter.setUserEffect(0);

    painter.setScissor(QRect());
}

void tst_QGLPainter::scissorPaintQ(QPainter *painter, const QSize& size)
{
    QGLSimulator sim(painter, size);

    sim.clear();

    QMatrix4x4 proj;
    proj.ortho(widget->rect());
    sim.setProjectionMatrix(proj);

    QVector2DArray vertices;
    vertices.append(10, 100);
    vertices.append(500, 100);
    vertices.append(500, 500);

    // Paint a green triangle.
    sim.setColor(Qt::green);
    sim.drawTriangles(vertices);

    // Change the top part of the triangle to blue.
    sim.setScissor
        (QRect(0, 0, widget->width(), qMin(widget->height() / 2, 200)));
    sim.setColor(Qt::blue);
    sim.drawTriangles(vertices);

    // Intersect and draw red over the blue section.
    sim.intersectScissor
        (QRect(0, 0, widget->width(), qMin(widget->height() / 4, 150)));
    sim.setColor(Qt::red);
    sim.drawTriangles(vertices);

    // Change the bottom part of the triangle to yellow.
    int y = qMin(widget->height() / 2, 350);
    sim.setScissor
        (QRect(0, y, 400, widget->height() - y));
    sim.setColor(Qt::yellow);
    sim.drawTriangles(vertices);

    // Intersect and expand, to extend the yellow region.
    sim.intersectScissor
        (QRect(0, y + 20, 400, widget->height() - y - 20));
    sim.expandScissor
        (QRect(0, y + 20, 450, widget->height() - y - 20));
    sim.setColor(Qt::yellow);
    sim.drawTriangles(vertices);
}

// Test the mathematical correctness of matrix stacks on a user stack.
// We assume that QMatrix4x4 works and we can use it as an oracle.
void tst_QGLPainter::userMatrixStack()
{
    QGLMatrixStack stack;

    QVERIFY(stack.type() == QGLMatrixStack::UserMatrix);
    QVERIFY(stack.top().isIdentity());
    QVERIFY(QMatrix4x4(stack).isIdentity());

    QMatrix4x4 m;
    m.translate(1, 2, 3);
    m.scale(4, 5, 6);
    m.rotate(90, 0, 1, 0);

    stack.translate(1, 2, 3);
    stack.scale(4, 5, 6);
    stack.rotate(90, 0, 1, 0);
    QVERIFY(qFuzzyCompare(m, stack.top()));
    QVERIFY(qFuzzyCompare(m, QMatrix4x4(stack)));

    stack.push();
    QVERIFY(qFuzzyCompare(m, stack.top()));
    QVERIFY(qFuzzyCompare(m, QMatrix4x4(stack)));

    stack.setToIdentity();
    QVERIFY(stack.top().isIdentity());

    stack.translate(QVector3D(1, 2, 3));
    stack.scale(QVector3D(4, 5, 6));
    stack.rotate(90, QVector3D(0, 1, 0));
    QVERIFY(qFuzzyCompare(m, stack.top()));
    QVERIFY(qFuzzyCompare(m, QMatrix4x4(stack)));

    QMatrix4x4 m2(m);
    m2.scale(-3);

    stack.scale(-3);
    QVERIFY(qFuzzyCompare(m2, stack.top()));
    QVERIFY(qFuzzyCompare(m2, QMatrix4x4(stack)));

    stack.pop();

    QVERIFY(qFuzzyCompare(m, stack.top()));
    QVERIFY(qFuzzyCompare(m, QMatrix4x4(stack)));

    stack.pop(); // at bottom of stack - no change

    QVERIFY(qFuzzyCompare(m, stack.top()));
    QVERIFY(qFuzzyCompare(m, QMatrix4x4(stack)));

    stack = m2;
    QVERIFY(qFuzzyCompare(m2, stack.top()));
    QVERIFY(qFuzzyCompare(m2, QMatrix4x4(stack)));

    QMatrix4x4 m3 = m2 * m;
    stack *= m;
    QVERIFY(qFuzzyCompare(m3, stack.top()));
    QVERIFY(qFuzzyCompare(m3, QMatrix4x4(stack)));

    m3.rotate(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -90));
    stack.rotate(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), -90));

    QVERIFY(qFuzzyCompare(m3, stack.top()));
    QVERIFY(qFuzzyCompare(m3, QMatrix4x4(stack)));

    QMatrix4x4 m4;

    m4.setToIdentity();
    m4.ortho(QRect(2, 3, 4, 5));
    stack.setToIdentity();
    stack.ortho(QRect(2, 3, 4, 5));
    QVERIFY(qFuzzyCompare(m4, stack.top()));
    QVERIFY(qFuzzyCompare(m4, QMatrix4x4(stack)));
}

#if defined(QT_OPENGL_ES_2)
#define QGL_NO_MATRIX_FETCH 1
#define QGL_NO_MATRIX_RESET 1
#endif

#ifndef QGL_NO_MATRIX_FETCH

static void clearGLMatrix(GLenum type)
{
    glMatrixMode(type);
    glLoadIdentity();
}

static bool checkGLMatrix(GLenum type, const QMatrix4x4& expected)
{
    QMatrix4x4 actual;
    int index;
    GLfloat mat[16];
    glGetFloatv(type, mat);
    qreal *m = actual.data();
    for (index = 0; index < 16; ++index)
        m[index] = mat[index];
    for (index = 0; index < 16; ++index) {
        // Introduce some fuzziness for GL servers that
        // use a different precision from QMatrix4x4.
        if (qAbs(actual.constData()[index] - expected.constData()[index])
                >= 0.001)
            return false;
    }
    return true;
}

static void setGLMatrix(GLenum type, const QMatrix4x4& matrix)
{
    glMatrixMode(type);
    GLfloat mat[16];
    const qreal *m = matrix.constData();
    for (int index = 0; index < 16; ++index)
        mat[index] = m[index];
    glLoadMatrixf(mat);
}

#else

#ifndef GL_PROJECTION
#define GL_PROJECTION           0
#endif
#ifndef GL_PROJECTION_MATRIX
#define GL_PROJECTION_MATRIX    0
#endif
#ifndef GL_MODELVIEW
#define GL_MODELVIEW            0
#endif
#ifndef GL_MODELVIEW_MATRIX
#define GL_MODELVIEW_MATRIX     0
#endif

// OpenGL/ES 2.0 does not have server-side matrices.
// For such platforms, we stub out the checks and just hope that they work.
static void clearGLMatrix(GLenum) {}
static bool checkGLMatrix(GLenum, const QMatrix4x4&) { return true; }
static void setGLMatrix(GLenum, const QMatrix4x4&) {}

#endif

void tst_QGLPainter::projectionMatrixStack()
{
    QGLPainter painter(widget);

    painter.projectionMatrix().setToIdentity();
    QVERIFY(painter.projectionMatrix().type() ==
                QGLMatrixStack::ProjectionMatrix);
    QVERIFY(painter.projectionMatrix().top().isIdentity());
    QVERIFY(QMatrix4x4(painter.projectionMatrix()).isIdentity());

    // Clear the matrix in the GL server.
    clearGLMatrix(GL_PROJECTION);

    QMatrix4x4 m;
    m.ortho(2, 4, 3, 1, 10, 50);
    painter.projectionMatrix().ortho(QRect(2, 1, 2, 2), 10, 50);
    QVERIFY(qFuzzyCompare(m, painter.projectionMatrix().top()));
    QVERIFY(qFuzzyCompare(m, QMatrix4x4(painter.projectionMatrix())));

    // The matrix in the GL server should still be the identity.
    QVERIFY(checkGLMatrix(GL_PROJECTION_MATRIX, QMatrix4x4()));

    // Force an update to the GL server.
    painter.update();

    // Check that the server received the value we set.
    QVERIFY(checkGLMatrix(GL_PROJECTION_MATRIX, m));

    // Write an explict value to the GL server and reset the client-side copy.
    QMatrix4x4 m2;
    m2.ortho(widget->rect());
    setGLMatrix(GL_PROJECTION, m2);
    painter.projectionMatrix().reset();

    // Read back the explicitly set value from the GL server.
#if defined(QGL_NO_MATRIX_RESET) // OpenGL/ES 2.0
    QVERIFY(qFuzzyCompare(m, painter.projectionMatrix().top()));
#else
    QVERIFY(qFuzzyCompare(m2, painter.projectionMatrix().top()));
#endif
}

void tst_QGLPainter::modelViewMatrixStack()
{
    QGLPainter painter(widget);

    painter.modelViewMatrix().setToIdentity();
    QVERIFY(painter.modelViewMatrix().type() ==
                QGLMatrixStack::ModelViewMatrix);
    QVERIFY(painter.modelViewMatrix().top().isIdentity());
    QVERIFY(QMatrix4x4(painter.modelViewMatrix()).isIdentity());

    // Clear the matrix in the GL server.
    clearGLMatrix(GL_MODELVIEW);

    QMatrix4x4 m;
    m.translate(1, 2, 3);
    m.scale(4, 5, 6);
    m.rotate(90, 0, 1, 0);
    painter.modelViewMatrix().translate(1, 2, 3);
    painter.modelViewMatrix().scale(4, 5, 6);
    painter.modelViewMatrix().rotate(90, 0, 1, 0);
    QVERIFY(qFuzzyCompare(m, painter.modelViewMatrix().top()));
    QVERIFY(qFuzzyCompare(m, QMatrix4x4(painter.modelViewMatrix())));

    // The matrix in the GL server should still be the identity.
    QVERIFY(checkGLMatrix(GL_MODELVIEW_MATRIX, QMatrix4x4()));

    // Force an update to the GL server.
    painter.update();

    // Check that the server received the value we set.
    QVERIFY(checkGLMatrix(GL_MODELVIEW_MATRIX, m));

    // Write an explict value to the GL server and reset the client-side copy.
    QMatrix4x4 m2;
    m2.translate(5, 6, 7);
    setGLMatrix(GL_MODELVIEW, m2);
    painter.modelViewMatrix().reset();

    // Read back the explicitly set value from the GL server.
#if defined(QGL_NO_MATRIX_RESET) // OpenGL/ES 2.0
    QVERIFY(qFuzzyCompare(m, painter.modelViewMatrix().top()));
#else
    QVERIFY(qFuzzyCompare(m2, painter.modelViewMatrix().top()));
#endif
}

QTEST_MAIN(tst_QGLPainter)

#include "tst_qglpainter.moc"
