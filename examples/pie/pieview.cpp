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

#include "pieview.h"
#include <QtCore/qmath.h>

void PieView::initializeGL(QGLPainter *painter)
{
    const qreal qRadius = 1.0f;
    const qreal qHeight = 0.6f;
    const qreal qThickness = 0.4f;
    const int qNumSlices = 16;

    pie.newSection(QGL::Smooth);
    QVector3D ap(qRadius, 0.0f, 0.0f);
    QVector3D bp(qRadius + qThickness, 0.0f, qHeight);
    QVector3D anc(qRadius * 2.0f, qRadius *2.0f, qHeight);
    for (int slice = 1; slice < qNumSlices; ++slice)
    {
        const qreal pi2 = 2.0 * M_PI;
        qreal angle = (slice * pi2) / (4 * qNumSlices);
        qreal cs = qCos(angle);
        qreal sn = qSin(angle);
        QVector3D a(cs * qRadius, sn * qRadius, 0.0f);
        QVector3D b(cs * (qRadius + qThickness), sn * (qRadius + qThickness), qHeight);
        QGLPrimitive quad;
        quad.appendVertex(ap, bp, b, a);
        pie.addQuad(quad);
        QGLPrimitive tri;
        tri.appendVertex(b, bp, anc);
        pie.addTriangle(tri);
        ap = a;
        bp = b;
    }
    painter->setLightEnabled(0, true);
    painter->setFaceColor(QGL::AllFaces, QColor(170, 202, 0));

    painter->setStandardEffect(QGL::LitMaterial);
}

void PieView::paintGL(QGLPainter *painter)
{
    painter->modelViewMatrix().rotate(45.0f, 1.0f, 1.0f, 1.0f);
    pie.draw(painter);
}
