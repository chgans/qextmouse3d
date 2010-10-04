/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt scene graph research project.
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

#ifndef PHOTOBROWSER3DVIEW_H
#define PHOTOBROWSER3DVIEW_H

#include "qglview.h"

class ImageDisplay;
class SkyBox;
class QGLMaterialCollection;
class QGLSceneNode;
class ImageManager;
class QState;
class QStateMachine;
class QFocusAdaptor;
class QAtlas;

class PhotoBrowser3DView : public QGLView
{
    Q_OBJECT
public:
    PhotoBrowser3DView();
    ~PhotoBrowser3DView();
    void initializeGL(QGLPainter *);
signals:
    void done();
    void zoom();
    void pan();
protected:
    void earlyPaintGL(QGLPainter *);
    void paintGL(QGLPainter *);
    void wheelEvent(QWheelEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void closeEvent(QCloseEvent *e);
    void mousePressEvent(QMouseEvent *e);
private slots:
    void initialise();
    void keyTimeOut();
    void zoomImage();
    void framesDirty();
    void stateEnter();
    void stateExit();
    void waitForExit();
private:
    void registerFrames();
    void setupStates();

    ImageDisplay *m_scene;
    ImageManager *m_images;
    SkyBox *m_skybox;
    QGLMaterialCollection *m_palette;
    qreal m_velocity;
    QTimer *m_keyTimer;
    QTime *m_panTime;
    QStateMachine *m_state;
    QState *m_app;
    QState *m_zoomed;
    QState *m_browse;
    QState *m_pan;
    QFocusAdaptor *m_fa;
    bool m_framesDirty;
    qreal m_displaySize;
    bool m_done;
    bool m_closing;
};

#endif // PHOTOBROWSER3DVIEW_H
