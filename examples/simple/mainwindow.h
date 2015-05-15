#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QExtMouse3DEventProvider;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void availableChanged();
    void filtersChanged();
    void sensitivityChanged();
    void updateFilters();
    void updateSensitivity();

private:
    Ui::MainWindow *ui;
    QExtMouse3DEventProvider *m_provider;

    // QObject interface
public:
    virtual bool event(QEvent *);

    // QWidget interface
protected:
    virtual void keyPressEvent(QKeyEvent *);
};

#endif // MAINWINDOW_H
