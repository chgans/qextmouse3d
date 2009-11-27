#ifndef FPSWIDGET_H
#define FPSWIDGET_H

#include <QGLWidget>

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
    void paintEvent(QPaintEvent *);
signals:
    void fps(int);
private slots:
    void animate();
private:
    QTime *time;
    QTimer *timer;
    QTimer *goAwayTimer;
    int xrot, yrot, zrot;
};

#endif // FPSWIDGET_H
