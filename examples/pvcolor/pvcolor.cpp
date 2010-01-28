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

#include "pvcolor.h"
#include "qgldisplaylist.h"
#include "qgloperation.h"
#include "qglscenenode.h"
#include "qgllightmodel.h"

#include <QtGui/qmatrix4x4.h>

#include <QtCore/qmath.h>

// size data for Q model
const qreal qRadius = 1.0f;
const qreal qHeight = 0.6f;
const qreal qThickness = 0.4f;
const int qNumSlices = 16;

PVColorView::PVColorView(QWidget *parent)
    : QGLView(parent)
    , pvScene(new QGLSceneNode(this))
{
    //! [0]
    QGLDisplayList *displayList = buildGeometry();
    displayList->setParent(pvScene);
    //! [0]
    {
        // rotate the q around so its label shows; and down
        // so the base is facing down
        QMatrix4x4 mat;
        QQuaternion q1 = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 270.0f);
        QQuaternion q2 = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 100.0f);
        mat.rotate(q2 * q1);
        displayList->setLocalTransform(mat);
    }
    return;  // remove me

    // display a copy of the q to the left
    QGLSceneNode *node = new QGLSceneNode(pvScene);
    node->addNode(displayList);
    {
        QMatrix4x4 mat;
        mat.translate(-2.0f, 0.0f, -2.0f);
        node->setLocalTransform(mat);
    }

    // display a copy of the q to the right
    node = new QGLSceneNode(pvScene);
    node->addNode(displayList);
    {
        QMatrix4x4 mat;
        mat.translate(2.0f, 0.0f, -2.0f);
        node->setLocalTransform(mat);
    }

    // rotate the whole scene about x-axis so that
    // q tops are visible when scene is first displayed
    {
        QMatrix4x4 mat;
        mat.rotate(1.0f, 0.0f, 0.0f, -30.0f);
        pvScene->setLocalTransform(mat);
    }
}

void PVColorView::initializeGL(QGLPainter *painter)
{
    QGLLightParameters *light0 = new QGLLightParameters(this);
    light0->setAmbientColor(Qt::white);
    light0->setDiffuseColor(Qt::white);
    light0->setDirection(QVector3D(0.0f, 0.2f, 2.0f));
    painter->setLightParameters(0, light0);
    painter->setLightEnabled(0, true);
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
                  qFuzzyIsNull(sn) ? 0.0 : orad * sn);
    if (!box.contains(opt))
        outer << opt;
    if (!box.contains(ipt))
        inner << ipt;
}

QGLDisplayList *PVColorView::buildGeometry()
{
    //! [2]
    QGLDisplayList *qList = new QGLDisplayList();

    // default effect for q where no other effect set
    qList->setEffect(QGL::FlatPerVertexColor);
    //! [2]

    const QVector3D extrudeVec(0.0f, 0.0f, qHeight);

    // defining coordinate data for q
    QVector3DArray topQOEdge;     // outer Edge of Q - top face
    QVector3DArray topQIEdge;     // inner Edge of Q - top face
    QVector3DArray bottomQOEdge;  // outer Edge of Q - bottom face
    QVector3DArray bottomQIEdge;  // inner Edge of Q - bottom face
    QVector3DArray topTailEdge;     // tail of Q - top face
    QVector3DArray bottomTailEdge;    // tail of Q - bottom face

    QColor4b innerColor(Qt::darkGray);
    QColor4b outerColor(Qt::darkGreen);
    QColor4b tailColor(Qt::darkYellow);

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
    Q_ASSERT(olap % 2 == 0);
    int lap = olap / 2;
    qList->newSection();
    {
        // create the top face of the tail of the Q - its a quad
        QGLOperation op(qList, QGL::QUAD);
        op << topTailEdge;
        op << QArray<QColor4b>(tailCnt, tailColor);
    }
    if (lap)
    {
        // if there is an overlap between outer over the inner, draw those
        // points as a fan from the first inner to the outer overlap points
        QGLOperation op(qList, QGL::TRIANGLE_FAN);
        op << topQIEdge.at(0) << innerColor;
        op << topQOEdge.left(lap);
        op << QArray<QColor4b>(lap, outerColor);
    }
    {
        // now draw all the quads making up the rest of the face of the Q
        QGLOperation op(qList, QGL::QUADS_ZIPPED);
        op << topQOEdge.mid(lap, icnt);
        op << QArray<QColor4b>(icnt, outerColor);
        op << QGL::NEXT_PRIMITIVE;
        op << topQIEdge;
        op << QArray<QColor4b>(icnt, innerColor);
    }
    if (lap)
    {
        // now draw the overlap points at the other end
        QGLOperation op(qList, QGL::TRIANGLE_FAN);
        op << topQIEdge.at(icnt - 1) << innerColor;
        op << topQOEdge.right(lap);
        op << QArray<QColor4b>(lap, outerColor);
    }

    // create the extruded sides of the q, and save the extruded values
    qList->newSection();
    {
        // outside sides
        QGLOperation op(qList, QGL::QUADS_ZIPPED);
        bottomQOEdge = topQOEdge.translated(extrudeVec);
        op << topQOEdge;
        op << QArray<QColor4b>(ocnt, outerColor);
        op << QGL::NEXT_PRIMITIVE;
        op << bottomQOEdge;
        op << QArray<QColor4b>(ocnt, outerColor);
    }
    {
        // inside sides
        QGLOperation op(qList, QGL::QUADS_ZIPPED);
        bottomQIEdge = topQIEdge.translated(extrudeVec);
        op << QGL::FACE_SENSE_REVERSED;
        op << topQIEdge;
        op << QArray<QColor4b>(icnt, innerColor);
        op << QGL::NEXT_PRIMITIVE;
        op << bottomQIEdge;
        op << QArray<QColor4b>(icnt, innerColor);
    }
    {
        // tail sides
        QGLOperation op(qList, QGL::QUADS_ZIPPED);
        bottomTailEdge = topTailEdge.translated(extrudeVec);
        op << topTailEdge;
        op << QArray<QColor4b>(tailCnt, outerColor);
        op << QGL::NEXT_PRIMITIVE;
        op << bottomTailEdge;
        op << QArray<QColor4b>(tailCnt, outerColor);
    }

    // now create the obverse faces of the Q - need to work
    // in reverse order to make the windings & normals correct
    bottomQIEdge.reverse();
    bottomQOEdge.reverse();
    bottomTailEdge.reverse();
    //! [3]
    qList->newSection();
    {
        // create the bottom face of the tail of the Q
        QGLOperation op(qList, QGL::QUAD);
        op << topTailEdge;
        op << QArray<QColor4b>(tailCnt, tailColor);
    }
    if (lap)
    {
        // draw the outer-over-inner overlap
        QGLOperation op(qList, QGL::TRIANGLE_FAN);
        op << bottomQIEdge.at(0) << innerColor;
        op << bottomQOEdge.left(lap);
        op << QArray<QColor4b>(lap, outerColor);
    }
    {
        // now draw all the quads of the bottom of the Q
        QGLOperation op(qList, QGL::QUADS_ZIPPED);
        op << bottomQOEdge.mid(lap, icnt);
        op << QArray<QColor4b>(icnt, outerColor);
        op << QGL::NEXT_PRIMITIVE;
        op << bottomQIEdge;
        op << QArray<QColor4b>(icnt, innerColor);
    }
    if (lap)
    {
        // now draw the overlap points at the other end
        QGLOperation op(qList, QGL::TRIANGLE_FAN);
        op << bottomQIEdge.at(icnt - 1) << innerColor;
        op << bottomQOEdge.right(lap);
        op << QArray<QColor4b>(lap, outerColor);
    }

    qList->finalize();
    return qList;
    //! [3]
}
