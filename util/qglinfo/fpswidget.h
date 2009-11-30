#ifndef FPSWIDGET_H
#define FPSWIDGET_H

#include <QGLWidget>
#if defined(QT_OPENGL_ES_2)
#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtOpenGL/qglshaderprogram.h>
#endif

class QTime;
class QTimer;

class FPSWidget : public QGLWidget
{
    Q_OBJECT
public:
    FPSWidget(QWidget *parent = 0);
    ~FPSWidget();
    void initializeGL();
    void resizeGL(int, int);
    void paintGL();
signals:
    void fps(int);
private slots:
    void animate();
private:
    QTime *time;
    QTimer *timer;
    QTimer *goAwayTimer;
    int xrot, yrot, zrot;
    int frameElapsed;
    int frameCount;
    int totalFrameTime;
#if defined(QT_OPENGL_ES_2)
    void setupShaders();

    QGLShaderProgram program;
    QMatrix4x4 projection;
    QMatrix4x4 modelView;
    int vertexAttr;
    int normalAttr;
    int matrixUniform;
    int materialUniform;
#endif
};

#endif // FPSWIDGET_H
