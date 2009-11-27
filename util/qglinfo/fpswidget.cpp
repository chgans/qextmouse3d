#include "fpswidget.h"

#include <QtCore/qtimer.h>
#include <QtCore/qdatetime.h>

#include <QDebug>

// dont drive the updates any faster than every this many millisecondss
#define MAX_ANIM_FREQUENCY 5

#define QGL_CUBE_SIZE (6 * 6 * (3 + 3))
static float const cubeVertices[QGL_CUBE_SIZE] = {
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,

    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,

    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,

    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
};

FPSWidget::FPSWidget(QWidget *parent)
    : QGLWidget(parent)
    , time(new QTime())
    , timer(new QTimer(this))
    , goAwayTimer(new QTimer(this))
    , xrot(0)
    , yrot(0)
    , zrot(0)
    , totElapsed(0)
    , numFrameTime(0)
    , totalFrameTime(0)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Generating Frames/Second"));
    time->start();
    connect(timer, SIGNAL(timeout()),
            this, SLOT(animate()));
    timer->start(0);   // call the animation as soon as we return to the event loop
    goAwayTimer = new QTimer(this);
    connect(goAwayTimer, SIGNAL(timeout()),
            this, SLOT(close()));
    goAwayTimer->start(30000);
}

FPSWidget::~FPSWidget()
{
    delete time;
}

void FPSWidget::initializeGL()
{
    glClearColor(0.9f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
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

void FPSWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(xrot, 1.0, 0.0, 0.0);
    glRotatef(yrot, 0.0, 1.0, 0.0);
    glRotatef(zrot, 0.0, 0.0, 1.0);

    glVertexPointer(3, GL_FLOAT, 6 * sizeof(GLfloat), cubeVertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT, 6 * sizeof(GLfloat), cubeVertices);
    glEnableClientState(GL_NORMAL_ARRAY);

    glColor4f(0.1f, 0.85f, 0.25f, 1.0f);
    static GLfloat mat[] = { 0.1f, 0.85f, 0.25f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void FPSWidget::animate()
{
    int elapsed = time->restart();
    totElapsed += elapsed;
    if (totElapsed > MAX_ANIM_FREQUENCY)
    {
        totElapsed += elapsed;
        int adv = totElapsed / 10;
        if (adv > 0)
        {
            numFrameTime += 1;
            totalFrameTime += totElapsed;
            emit fps(1000 / (totalFrameTime / numFrameTime));
            xrot = (xrot + adv) % 360;
            yrot = (xrot + adv) % 360;
            zrot = (xrot + adv) % 360;
            update();
            totElapsed = 0;
        }
    }
}
