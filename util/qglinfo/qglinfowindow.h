#ifndef QGLINFOWINDOW_H
#define QGLINFOWINDOW_H

#include <QMainWindow>

namespace Ui {
    class QGLInfoWindow;
}

class QGLInfo;
class FPSWidget;

class QGLInfoWindow : public QMainWindow {
    Q_OBJECT
public:
    QGLInfoWindow(QWidget *parent = 0);
    ~QGLInfoWindow();
public slots:
    void on_actionQuit_triggered();
    void on_action_Save_As_triggered();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::QGLInfoWindow *ui;
    QGLInfo *info;
    FPSWidget *fps;

private slots:
    void on_actionRun_FPS_Test_triggered();
    void on_actionAbout_triggered();
};

#endif // QGLINFOWINDOW_H
