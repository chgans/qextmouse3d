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
    void userMatrixStack();
    void projectionMatrixStack();
    void modelViewMatrixStack();

public slots:
    void clearPaint();
    void clearPaintQ(QPainter *painter, const QSize& size);
    void drawTrianglePaint();
    void drawTrianglePaintQ(QPainter *painter, const QSize& size);

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
    m4.ortho(2, 4, 1, 3, 10, 50);
    stack.setToIdentity();
    stack.ortho(2, 4, 1, 3, 10, 50);
    QVERIFY(qFuzzyCompare(m4, stack.top()));
    QVERIFY(qFuzzyCompare(m4, QMatrix4x4(stack)));

    m4.setToIdentity();
    m4.ortho(QRect(2, 3, 4, 5));
    stack.setToIdentity();
    stack.ortho(QRect(2, 3, 4, 5));
    QVERIFY(qFuzzyCompare(m4, stack.top()));
    QVERIFY(qFuzzyCompare(m4, QMatrix4x4(stack)));

    m4.setToIdentity();
    m4.ortho(QRectF(2, 3, 4, 5));
    stack.setToIdentity();
    stack.ortho(QRectF(2, 3, 4, 5));
    QVERIFY(qFuzzyCompare(m4, stack.top()));
    QVERIFY(qFuzzyCompare(m4, QMatrix4x4(stack)));

    m4.setToIdentity();
    m4.frustum(2, 4, 1, 3, 10, 50);
    stack.setToIdentity();
    stack.frustum(2, 4, 1, 3, 10, 50);
    QVERIFY(qFuzzyCompare(m4, stack.top()));
    QVERIFY(qFuzzyCompare(m4, QMatrix4x4(stack)));

    m4.setToIdentity();
    m4.perspective(60, 1.5, 10, 50);
    stack.setToIdentity();
    stack.perspective(60, 1.5, 10, 50);
    QVERIFY(qFuzzyCompare(m4, stack.top()));
    QVERIFY(qFuzzyCompare(m4, QMatrix4x4(stack)));

    m4.setToIdentity();
    m4.lookAt(QVector3D(0, 0, -10), QVector3D(1, 1, 3), QVector3D(0, 1, 0));
    stack.setToIdentity();
    stack.lookAt(QVector3D(0, 0, -10), QVector3D(1, 1, 3), QVector3D(0, 1, 0));
    QVERIFY(qFuzzyCompare(m4, stack.top()));
    QVERIFY(qFuzzyCompare(m4, QMatrix4x4(stack)));
}

#if defined(QT_OPENGL_ES_2)
#define QGL_NO_MATRIX_FETCH 1
#define QGL_NO_MATRIX_RESET 1
#elif defined(GL_OES_VERSION_1_0) && !defined(GL_OES_VERSION_1_1)
#define QGL_NO_MATRIX_FETCH 1
#define QGL_MATRIX_RESET_TO_IDENTITY 1
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
// OpenGL/ES 1.0 cannot fetch the server-side matrices.
// For these platforms, we stub out the checks and just hope that they work.
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
    m.frustum(2, 4, 1, 3, 10, 50);
    painter.projectionMatrix().frustum(2, 4, 1, 3, 10, 50);
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
#elif defined(QGL_MATRIX_RESET_TO_IDENTITY) // OpenGL/ES 1.0
    QVERIFY(qFuzzyCompare(QMatrix4x4(), painter.projectionMatrix().top()));
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
#elif defined(QGL_MATRIX_RESET_TO_IDENTITY) // OpenGL/ES 1.0
    QVERIFY(qFuzzyCompare(QMatrix4x4(), painter.modelViewMatrix().top()));
#else
    QVERIFY(qFuzzyCompare(m2, painter.modelViewMatrix().top()));
#endif
}

QTEST_MAIN(tst_QGLPainter)

#include "tst_qglpainter.moc"
