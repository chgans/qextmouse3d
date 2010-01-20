/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "geometry.h"
#include "qgltexture2d.h"
#include "qglmaterialcollection.h"
#include "qtriangle3d.h"
#include "qgloperation.h"

#include <QImage>
#include <QPainter>

// icosahedron is defined by phi, derived from the golden section
// http://en.wikipedia.org/wiki/Icosahedron#Cartesian_coordinates
const qreal phi = 1.618033988749894848f;

Geometry::Geometry(QObject *parent, QGLMaterialCollection *materials)
    : QGLDisplayList(parent, materials)
{
    float ico[12][3] = {
        { 0.0f, 1.0f, phi },    // A - 0
        { 0.0f, 1.0f, -phi },   // B - 1
        { 0.0f, -1.0f, phi },   // C - 2
        { 0.0f, -1.0f, -phi },  // D - 3
        { 1.0f, phi, 0.0f },    // E - 4
        { 1.0f, -phi, 0.0f },   // F - 5
        { -1.0f, phi, 0.0f },   // G - 6
        { -1.0f, -phi, 0.0f },  // H - 7
        { phi, 0.0f, 1.0f },    // I - 8
        { phi, 0.0f, -1.0f },   // J - 9
        { -phi, 0.0f, 1.0f },   // K - 10
        { -phi, 0.0f, -1.0f }   // L - 11
    };

    int face[20][3] = {
        { 4, 0, 8 },            // E-A-I
        { 6, 0, 4 },            // G-A-E
        { 6, 10, 0 },           // G-K-A
        { 11, 10, 6 },          // L-K-G
        { 0, 2, 8 },            // A-C-I
        { 10, 2, 0 },           // K-C-A
        { 10, 7, 2 },           // K-H-C
        { 11, 7, 10 },          // L-H-K
        { 2, 5, 8 },            // C-F-I
        { 7, 5, 2 },            // H-F-C
        { 7, 3, 5 },            // H-D-F
        { 11, 3, 7 },           // L-D-H
        { 5, 9, 8 },            // F-J-I
        { 3, 9, 5 },            // D-J-F
        { 3, 1, 9 },            // D-B-J
        { 11, 1, 3 },           // L-B-D
        { 9, 4, 8 },            // J-E-I
        { 1, 4, 9 },            // B-E-J
        { 1, 6, 4 },            // B-G-E
        { 11, 6, 1 }            // L-G-B
    };

    const float u0 = 0.0f;
    const float u1 = 0.173205081f;
    const float u2 = 0.346410162f;
    const float u3 = 0.519615242f;
    const float u4 = 0.692820323f;
    const float u5 = 0.866025402f;
    const float v0 = 0.0f;
    const float v1 = 0.111111111f;
    const float v2 = 0.222222222f;
    const float v3 = 0.333333333f;
    const float v4 = 0.444444444f;
    const float v5 = 0.555555555f;
    const float v6 = 0.666666666f;
    const float v7 = 0.777777777f;
    const float v8 = 0.888888888f;
    const float v9 = 1.0f;

    float tex[20][3][2] = {
        { { u0, v1 }, { u1, v2 }, { u1, v0 } }, // E-A-I
        { { u0, v3 }, { u1, v2 }, { u0, v1 } }, // G-A-E
        { { u0, v3 }, { u1, v4 }, { u1, v2 } }, // G-K-A
        { { u0, v5 }, { u1, v4 }, { u0, v3 } }, // L-K-G
        { { u1, v2 }, { u2, v3 }, { u2, v1 } }, // A-C-I
        { { u1, v4 }, { u2, v3 }, { u1, v2 } }, // K-C-A
        { { u1, v4 }, { u2, v5 }, { u2, v3 } }, // K-H-C
        { { u1, v6 }, { u2, v5 }, { u1, v4 } }, // L-H-K
        { { u2, v3 }, { u3, v4 }, { u3, v2 } }, // C-F-I
        { { u2, v5 }, { u3, v4 }, { u2, v3 } }, // H-F-C
        { { u2, v5 }, { u3, v6 }, { u3, v4 } }, // H-D-F
        { { u2, v7 }, { u3, v6 }, { u2, v5 } }, // L-D-H
        { { u3, v4 }, { u4, v5 }, { u4, v3 } }, // F-J-I
        { { u3, v6 }, { u4, v5 }, { u3, v4 } }, // D-J-F
        { { u3, v6 }, { u4, v7 }, { u4, v5 } }, // D-B-J
        { { u3, v8 }, { u4, v7 }, { u3, v6 } }, // L-B-D
        { { u4, v5 }, { u5, v6 }, { u5, v4 } }, // J-E-I
        { { u4, v7 }, { u5, v6 }, { u4, v5 } }, // B-E-J
        { { u4, v7 }, { u5, v8 }, { u5, v6 } }, // B-G-E
        { { u4, v9 }, { u5, v8 }, { u4, v7 } }  // L-G-B
    };
    QImage uv(512, 512, QImage::Format_ARGB32);
    uv.fill(qRgba(196, 196, 196, 0));
    QPainter painter;
    painter.begin(&uv);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen = painter.pen();
    pen.setWidth(2.0);
    painter.setPen(pen);
    QFont font = painter.font();
    font.setPointSize(18);
    painter.setFont(font);
    QFontMetrics metrics = painter.fontMetrics();

    newSection();
    currentNode()->setEffect(QGL::LitDecalTexture2D);
    for (int ix = 0; ix < 20; ++ix)
    {
        QVector3D v0(ico[face[ix][0]][0], ico[face[ix][0]][1], ico[face[ix][0]][2]);
        QVector3D v1(ico[face[ix][1]][0], ico[face[ix][1]][1], ico[face[ix][1]][2]);
        QVector3D v2(ico[face[ix][2]][0], ico[face[ix][2]][1], ico[face[ix][2]][2]);

        QVector2D t0(tex[ix][0][0], tex[ix][0][1]);
        QVector2D t1(tex[ix][1][0], tex[ix][1][1]);
        QVector2D t2(tex[ix][2][0], tex[ix][2][1]);

        // scale up and flip to draw the texture
        QVector2D tv0 = t0 * 512.0;
        QVector2D tv1 = t1 * 512.0;
        QVector2D tv2 = t2 * 512.0;

        painter.setPen(QColor("darkgrey"));
        painter.drawLine(tv0.toPointF(), tv1.toPointF());
        painter.drawLine(tv1.toPointF(), tv2.toPointF());
        painter.drawLine(tv2.toPointF(), tv0.toPointF());
        QTriangle3D tri(tv0, tv1, tv2);
        QVector3D cen = tri.center();
        QString n = QString::number(ix+1);

        painter.setPen(QColor("green"));
        painter.drawText(QPointF(cen.x() - metrics.width(n) / 2.0, cen.y() + 3.0), n);

        // Qt's coordinate system is upside down.  I'm ok with that.
        t0.setY(1.0f - t0.y());
        t1.setY(1.0f - t1.y());
        t2.setY(1.0f - t2.y());

        QGLOperation op(this, QGL::TRIANGLE);
        op << v0 << v1 << v2;
        op << t0 << t1 << t2;
    }

    QGLMaterialCollection *pal = geometry()->palette();
    QGLMaterialParameters *mat = new QGLMaterialParameters;
    mat->setAmbientColor(QColor(32, 64, 196));
    mat->setDiffuseColor(QColor(32, 32, 32));
    int m = pal->addMaterial(mat);

    QGLTexture2D *texture = new QGLTexture2D;
    texture->setImage(uv);
    pal->setTexture(m, texture);

    setMaterial(m);

    painter.end();
    finalize();
}
