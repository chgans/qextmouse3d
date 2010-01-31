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

#ifndef CUBEVIEW_H
#define CUBEVIEW_H

#include <QtOpenGL/qgl.h>

class QGLPainter;
class QTimer;
class QGLAbstractScene;
class QGLLightParameters;
class QGLLightModel;
class QMenu;
class QGLGeometry;
class QGLSceneNode;

class CubeView : public QGLWidget
{
    Q_OBJECT
public:
    CubeView(QWidget *parent = 0);
    ~CubeView();
    void setColorMenu(QMenu *menu) { mColorMenu = menu; }
    void setComponentMenu(QMenu *menu) { mComponentMenu = menu; }
    void setInitialModel(const QString &model) { mInitialModel = model; }

public slots:
    void load();
    void zoomChanged();
    void xTiltChanged(int);
    void yTiltChanged(int);
    void zTiltChanged(int);
    void yaxChanged();
    void selectColorChanged(const QColor &);
    void enableAnimation(bool);
    void openModelFile(const QString &);

signals:
    void colorUpdate(const QColor &);
    void modelLoaded(const QString &);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int, int);

private slots:
    void animate();
    void animateColor();
    void changeColor();
    void selectComponent();

private:
    void paintGL(QGLPainter *painter);
    void initializeGL(QGLPainter *painter);
    void loadColors();
    void loadComponents();
    void makeSelectColor(QColor color = QColor());
    void setMaterial(QGLSceneNode *root, int material);
    void restoreMaterial(QGLSceneNode *root);
    void importModel(const QString &name);

    QTimer *mTimer;
    QTimer *mColorTimer;
    int mSpin;
    int mXTilt, mYTilt, mZTilt;
    int mZoom;
    int mYax;
    QString mCurrentModelName;
    QGLAbstractScene *mSceneManager;
    QGLSceneNode *mSceneRoot;
    bool mSceneInitialized;
    QGLLightParameters *mLightParameters;
    QGLLightModel *mLightModel;
    QColor mSelectColor;
    QColor mPulse;
    QMenu *mColorMenu;
    QMenu *mComponentMenu;
    int mSelectMaterial;
    QGLSceneNode *mCurrentSelected;
    int mSelectColorAnimate;
    QMap<QGLGeometry *, int> mSaveMaterials;
    bool mAnimationEnabled;
    QString mInitialModel;
    bool mWarningDisplayed;
};


#endif // CUBEVIEW_H
