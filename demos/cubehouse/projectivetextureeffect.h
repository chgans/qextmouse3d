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

#ifndef PROJECTIVEDTEXTUREEFFECT_H
#define PROJECTIVEDTEXTUREEFFECT_H

#include "qglshaderprogrameffect.h"
//#include "deptheffect.h"
#include "qmatrix4x4.h"

class ProjectiveTextureEffect : public QGLShaderProgramEffect
{
public:
    explicit ProjectiveTextureEffect();
    virtual void setActive(QGLPainter *painter, bool flag);
    virtual void update(QGLPainter *painter, QGLPainter::Updates updates);
    virtual void setProjectorDirection(const QVector4D &direction);

    void setCameraModelViewMatrix(const QMatrix4x4 &newCameraModelViewMatrix);
    void setProjectorProjectionMatrix(const QMatrix4x4 &newMatrix);
    void setProjectorViewMatrix(const QMatrix4x4 &newMatrix);
    void setModelMatrix(const QMatrix4x4 &newMatrix);

//      TODO:
//    QMatrix4x4 eyeLinearTexgenMatrix;

protected:
    virtual void setupShaders();
private:
    virtual void recalulateObjectLinearTexgenMatrix();
    bool matrixDirty;
    QMatrix4x4 modelMatrix;
    QMatrix4x4 objectLinearTexgenMatrix;
    QMatrix4x4 cameraModelViewMatrix;
    QMatrix4x4 inverseCameraModelViewMatrix;
    QMatrix4x4 projectorProjectionMatrix;
    QMatrix4x4 projectorViewMatrix;
    QVector4D projectorDirection;
};

#endif // PROJECTIVEDTEXTUREEFFECT_H
