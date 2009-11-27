#include "fpswidget.h"

#include <QtCore/qtimer.h>
#include <QtCore/qdatetime.h>

#include <QDebug>

#define QGL_CUBE_SIZE (6 * 6 * 3)
static float const cubeVertices[QGL_CUBE_SIZE] = {
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, -0.5f,

    -0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, -0.5f,

    0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, -0.5f,

    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f,

    0.5f, -0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,

    0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    0.5f, 0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,
};

FPSWidget::FPSWidget(QWidget *parent)
    : QGLWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Generating Frames/Second"));
    time = new QTime();
    time->start();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),
            this, SLOT(animate()));
    timer->start(100);
    goAwayTimer = new QTimer(this);
    connect(goAwayTimer, SIGNAL(timeout()),
            this, SLOT(close()));
    goAwayTimer->start(60000);
}

FPSWidget::~FPSWidget()
{
    delete time;
}

void FPSWidget::initializeGL()
{
    glClearColor(0.9f, 0.1f, 0.1f, 1.0f);
}

void FPSWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int side = qMin(w, h);
    qreal ws = w / side;
    qreal hs = h / side;
#if defined(QT_OPENGL_ES_1)
    glOrthof(-ws, ws, -hs, hs, -1.0f, 1.0f);
#else
    glOrtho(-ws, ws, -hs, hs, -10.0f, 10.0f);
#endif
}

void FPSWidget::paintEvent(QPaintEvent * /* event */)
{
    makeCurrent();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(xrot, 1.0, 0.0, 0.0);
    glRotatef(yrot, 0.0, 1.0, 0.0);
    glRotatef(zrot, 0.0, 0.0, 1.0);

    glVertexPointer(3, GL_FLOAT, 0, cubeVertices);
    glEnableClientState(GL_VERTEX_ARRAY);

    glColor4f(0.1f, 0.85f, 0.25f, 1.0f);
    static GLfloat mat[] = { 0.1f, 0.85f, 0.25f, 1.0f };
    glMaterialfv(GL_FRONT_FACE, GL_AMBIENT, mat);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    swapBuffers();
    int elapsed = time->elapsed();
    if (elapsed > 0)
        emit fps(1000 / time->elapsed());
}

void FPSWidget::animate()
{
    xrot = (xrot + 10) % 360;
    yrot = (xrot + 10) % 360;
    zrot = (xrot + 10) % 360;
    update();
}
