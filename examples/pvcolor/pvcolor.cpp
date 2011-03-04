/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "pvcolor.h"
#include "qglbuilder.h"
#include "qglscenenode.h"
#include "qgllightmodel.h"
#include "qgraphicsrotation3d.h"

#include <QtGui/qmatrix4x4.h>
#include <QtCore/qmath.h>
#include <QWheelEvent>

// size data for Q model
const qreal qRadius = 1.0f;
const qreal qHeight = 0.6f;
const qreal qThickness = 0.4f;
const int qNumSlices = 32;

PVColorView::PVColorView(QWidget *parent)
    : QGLView(parent)
    , pvScene(new QGLSceneNode(this))
{
    //! [0]
    QGLSceneNode *scene = buildGeometry();
    pvScene->addNode(scene);

    // display a copy of the q to the left
    QGLSceneNode *node = scene->clone(pvScene);
    node->setPosition(QVector3D(-2.0f, 0.0f, -2.0f));

    // display a copy of the q to the right
    node = scene->clone(pvScene);
    node->setPosition(QVector3D(2.0f, 0.0f, -2.0f));

    // Make a nice p. v. color triangle as a back drop
    QGLBuilder builder;
    QGeometryData p;
    builder.newSection();
    p.appendVertex(QVector3D(3, 3, -3), QVector3D(-3, 3, -3),
                   QVector3D(0, -3, -3));
    p.appendColor(Qt::red, Qt::blue, Qt::yellow);
    builder.addTriangles(p);
    node = builder.finalizedSceneNode();
    node->setEffect(QGL::FlatPerVertexColor);
    pvScene->addNode(node);

    // rotate the whole scene about x-axis so that
    // q tops are visible when scene is first displayed
    QGraphicsRotation3D *rot = new QGraphicsRotation3D(pvScene);
    rot->setAxis(QVector3D(1, 0, 0));
    rot->setAngle(20.0f);
    pvScene->addTransform(rot);
    //! [0]
}

void PVColorView::initializeGL(QGLPainter *painter)
{
    QGLLightParameters *light0 = new QGLLightParameters(this);
    light0->setAmbientColor(Qt::white);
    light0->setDiffuseColor(Qt::white);
    light0->setDirection(QVector3D(0.0f, 0.2f, 2.0f));
    painter->setMainLight(light0);
    QGLLightModel *model = new QGLLightModel(this);
    model->setAmbientSceneColor(Qt::white);
    painter->setLightModel(model);
}

//! [1]
void PVColorView::paintGL(QGLPainter *painter)
{
    pvScene->draw(painter);
}
//! [1]

inline static void calculateSlice(int slice, const QBox3D &box,
                                  QVector3DArray &outer,
                                  QVector3DArray &inner)
{
    qreal t2 = qThickness / 2.0f;
    qreal irad = qRadius - t2;
    qreal orad = qRadius + t2;
    const qreal pi2 = 2.0 * M_PI;
    qreal angle = (slice * pi2) / qNumSlices;
    qreal cs = qCos(angle);
    qreal sn = qSin(angle);
    QVector2D opt(qFuzzyIsNull(cs) ? 0.0 : orad * cs,
                  qFuzzyIsNull(sn) ? 0.0 : orad * sn);
    QVector2D ipt(qFuzzyIsNull(cs) ? 0.0 : irad * cs,
                  qFuzzyIsNull(sn) ? 0.0 : irad * sn);
    if (!box.contains(opt))
        outer << opt;
    if (!box.contains(ipt))
        inner << ipt;
}

QGLSceneNode *PVColorView::buildGeometry()
{
    //! [2]
    QGLBuilder builder;

    // default effect for q where no other effect set
    builder.sceneNode()->setEffect(QGL::FlatPerVertexColor);
    builder.newSection(QGL::Smooth);
    //! [2]

    const QVector3D extrudeVec(0.0f, 0.0f, qHeight);

    // defining coordinate data for q
    QVector3DArray topQOEdge;     // outer Edge of Q - top face
    QVector3DArray topQIEdge;     // inner Edge of Q - top face
    QVector3DArray bottomQOEdge;  // outer Edge of Q - bottom face
    QVector3DArray bottomQIEdge;  // inner Edge of Q - bottom face
    QVector3DArray topTailEdge;     // tail of Q - top face
    QVector3DArray bottomTailEdge;    // tail of Q - bottom face

    QColor4ub innerColor(Qt::darkBlue);
    QColor4ub outerColor(Qt::darkGreen);
    QColor4ub tailColor(Qt::darkYellow);

    // do the math for the defining points
    qreal t2 = qThickness / 2.0f;
    qreal irad = qRadius - t2;
    qreal orad = qRadius + t2;
    qreal itail = irad - qThickness;
    qreal otail = orad + qThickness;
    topTailEdge << QVector2D(itail, -t2) << QVector2D(otail, -t2)
            << QVector2D(otail, t2) << QVector2D(itail, t2);
    QBox3D tail(topTailEdge[0], topTailEdge[2]);
    qreal xi0 = qSqrt((irad * irad) - (t2 * t2));
    qreal xo0 = qSqrt((orad * orad) - (t2 * t2));
    topQOEdge << QVector2D(xo0, t2);
    topQIEdge << QVector2D(xi0, t2);
    for (int i = 0; i < qNumSlices; ++i)
        calculateSlice(i, tail, topQOEdge, topQIEdge);
    topQOEdge << QVector2D(xo0, -t2);
    topQIEdge << QVector2D(xi0, -t2);

    // We build the geometry in 2 dimensions on the Z = 0 plane, and because
    // of the QVector3D(QVector2D) constructor these 2D coords can simply be
    // added to the data arrays as we build them up.  Z = 0 will be the top
    // of the geometry, and first we build up those top edges: the outside and
    // inside edge of the toroid of the Q, and then the tail.  Then we create
    // faces by stitching together the inside and outside edge with quads -
    // using the QUADS_ZIPPED mode, and add a face for the tail.  This gives a
    // flat Q in the Z = 0 plane.  Now we extrude down from the Z = 0 plane to
    // create the sides of the Q (extruding is a translate + QUADS_ZIPPED) and
    // finally draw the obverse (away facing) back-faces.

    // Some of the points generated for the inside and outside edges of the Q
    // fall inside the tail - so they are discarded, meaning the inner has
    // likely less points than the outer Edge, depending on thickness, number
    // of slices and so on.  Calculate this overlap of outer over inner.
    int icnt = topQIEdge.count();
    int ocnt = topQOEdge.count();
    int tailCnt = topTailEdge.count();
    int olap = ocnt - icnt;
    Q_ASSERT(olap % 2 == 0);  // this should be even
    int lap = olap / 2;
    {
        QGeometryData top;
        // create the top face of the tail of the Q - its a quad
        top.appendVertexArray(topTailEdge);
        top.appendColorArray(QArray<QColor4ub>(tailCnt, tailColor));
        builder.addQuads(top);
    }

    if (lap)
    {
        // if there is an overlap between outer over the inner, draw those
        // points as a fan from the first inner to the outer overlap points
        QGeometryData top;
        top.appendVertex(topQIEdge.at(0));
        top.appendColor(innerColor);
        top.appendVertexArray(topQOEdge.left(lap));
        top.appendColorArray(QArray<QColor4ub>(lap, outerColor));
        builder.addTriangleFan(top);
    }
    {
        // now draw all the quads making up the rest of the face of the Q
        QGeometryData in;
        QGeometryData out;
        out.appendVertexArray(topQOEdge.mid(lap, icnt));
        out.appendColorArray(QArray<QColor4ub>(icnt, outerColor));
        in.appendVertexArray(topQIEdge);
        in.appendColorArray(QArray<QColor4ub>(icnt, innerColor));
        builder.addQuadsInterleaved(in, out);
    }
    if (lap)
    {
        QGeometryData top;
        top.appendVertex(topQIEdge.at(icnt - 1));
        top.appendColor(innerColor);
        top.appendVertexArray(topQOEdge.right(lap));
        top.appendColorArray(QArray<QColor4ub>(lap, outerColor));
        qDebug() << top;
        builder.addTriangleFan(top);
    }

    // create the extruded sides of the q, and save the extruded values
    builder.newSection();
    {
        // outside sides
        QGeometryData top;
        QGeometryData bottom;
        bottomQOEdge = topQOEdge.translated(extrudeVec);
        top.appendVertexArray(topQOEdge);
        top.appendColorArray(QArray<QColor4ub>(ocnt, outerColor));
        bottom.appendVertexArray(bottomQOEdge);
        bottom.appendColorArray(QArray<QColor4ub>(ocnt, outerColor));
        builder.addQuadsInterleaved(top, bottom);
    }
    {
        // inside sides
        QGeometryData top;
        QGeometryData bottom;
        //! [translated]
        bottomQIEdge = topQIEdge.translated(extrudeVec);
        //! [translated]
        top.appendVertexArray(topQIEdge.reversed());
        top.appendColorArray(QArray<QColor4ub>(icnt, innerColor));
        bottom.appendVertexArray(bottomQIEdge.reversed());
        bottom.appendColorArray(QArray<QColor4ub>(icnt, innerColor));
        builder.addQuadsInterleaved(top, bottom);
    }
    {
        // tail sides
        QGeometryData top;
        QGeometryData bottom;
        bottomTailEdge = topTailEdge.translated(extrudeVec);
        QVector3DArray t1 = topTailEdge;
        t1.append(t1.first());
        QVector3DArray b1 = bottomTailEdge;
        b1.append(b1.first());
        top.appendVertexArray(t1);
        top.appendColorArray(QArray<QColor4ub>(tailCnt+1, outerColor));
        bottom.appendVertexArray(b1);
        bottom.appendColorArray(QArray<QColor4ub>(tailCnt+1, outerColor));
        builder.addQuadsInterleaved(top, bottom);
    }

    // now create the obverse faces of the Q - need to work
    // in reverse order to make the windings & normals correct
    bottomQIEdge.reverse();
    bottomQOEdge.reverse();
    bottomTailEdge.reverse();

    builder.newSection();
    {
        // create the bottom face of the tail of the Q
        QGeometryData bottom;
        bottom.appendVertexArray(bottomTailEdge);
        bottom.appendColorArray(QArray<QColor4ub>(tailCnt, tailColor));
        builder.addQuads(bottom);
    }
    if (lap)
    {
        // draw the outer-over-inner overlap
        QGeometryData bottom;
        bottom.appendVertex(bottomQIEdge.at(0));
        bottom.appendColor(innerColor);
        bottom.appendVertexArray(bottomQOEdge.left(lap));
        bottom.appendColorArray(QArray<QColor4ub>(lap, outerColor));
        builder.addTriangleFan(bottom);
    }
    //! [3]
    {
        // now draw all the quads of the bottom of the Q
        QGeometryData in;
        QGeometryData out;
        out.appendVertexArray(bottomQOEdge.mid(lap, icnt));
        out.appendColorArray(QArray<QColor4ub>(icnt, outerColor));
        in.appendVertexArray(bottomQIEdge);
        in.appendColorArray(QArray<QColor4ub>(icnt, innerColor));
        builder.addQuadsInterleaved(in, out);
    }
    //! [3]
    if (lap)
    {
        // now draw the overlap points at the other end
        QGeometryData bottom;
        bottom.appendVertex(bottomQIEdge.at(icnt - 1));
        bottom.appendColor(innerColor);
        bottom.appendVertexArray(bottomQOEdge.right(lap));
        bottom.appendColorArray(QArray<QColor4ub>(lap, outerColor));
        builder.addTriangleFan(bottom);
    }

    QGLSceneNode *node = builder.finalizedSceneNode();
    QGraphicsRotation3D *rot = new QGraphicsRotation3D(node);
    rot->setAxis(QVector3D(0, 0, 1));
    rot->setAngle(-45.0f);
    node->addTransform(rot);
    return node;
}
