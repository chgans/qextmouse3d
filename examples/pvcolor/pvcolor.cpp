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

#include "pvcolor.h"
#include "qglbuilder.h"
#include "qglscenenode.h"
#include "qgllightmodel.h"

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
    scene->setParent(pvScene);

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
    node->setParent(pvScene);

    // rotate the whole scene about x-axis so that
    // q tops are visible when scene is first displayed
    pvScene->setRotation(QVector3D(20.0f, 0.0f, 0.0f));
    //! [0]
}

void PVColorView::wheelEvent(QWheelEvent *e)
{
    QVector3D viewVec = camera()->eye() - camera()->center();
    qreal zoomMag = viewVec.length();
    qreal inc = float(0.3 * e->delta()) / 50.0f;
    if (!qFuzzyIsNull(inc))
    {
        zoomMag += inc;
        if (zoomMag < 5.0f)
            zoomMag = 5.0f;
        QRay3D viewLine(camera()->center(), viewVec.normalized());
        camera()->setEye(viewLine.point(zoomMag));
        update();
    }
    e->accept();
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
    node->setRotation(QVector3D(0.0f, 0.0f, -45.0f));
    return node;
}
