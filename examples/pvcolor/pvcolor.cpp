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
    {
        // rotate the q around so its label shows; and down
        // so the base is facing down
        QMatrix4x4 mat;
        QQuaternion q1 = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 270.0f);
        QQuaternion q2 = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 100.0f);
        mat.rotate(q2 * q1);
        displayList->setLocalTransform(mat);
    }
    return;

    /*

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
    //! [0]

    */

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
                                  QGL::VectorArray &outer, QGL::VectorArray &inner)
{
    qreal t2 = qThickness / 2.0f;
    qreal irad = qRadius - t2;
    qreal orad = qRadius + t2;
    const qreal pi2 = 2.0 * M_PI;
    qreal oxc, oyc, ixc, iyc;
    qreal angle = (slice * pi2) / qNumSlices;
    qreal cs = qCos(angle);
    qreal sn = qSin(angle);
    if (qFuzzyIsNull(cs))
    {
        oxc = 0.0;
        ixc = 0.0;
    }
    else
    {
        oxc = orad * cs;
        ixc = irad * cs;
    }
    if (qFuzzyIsNull(sn))
    {
        oyc = 0.0;
        iyc = 0.0;
    }
    else
    {
        oyc = orad * sn;
        iyc = irad * sn;
    }
    QVector2D opt(oxc, oyc);
    if (!box.contains(opt))
        outer << opt;
    QVector2D ipt(ixc, iyc);
    if (!box.contains(ipt))
        inner << ipt;
}

QGLDisplayList *PVColorView::buildGeometry()
{
    //! [2]
    QGLDisplayList *qList = new QGLDisplayList();

    // default effect for q where no other effect set
    //qList->setEffect(QGL::FlatPerVertexColor);
    qList->setEffect(QGL::LitMaterial);
    //! [2]

    const QVector3D qExtrudeVec(0.0f, 0.0f, qHeight);

    // defining coordinate data for q
    QGL::VectorArray topQORim;     // outer rim of Q - top face
    QGL::VectorArray topQIRim;     // inner rim of Q - top face
    QGL::VectorArray bottomQORim;  // outer rim of Q - bottom face
    QGL::VectorArray bottomQIRim;  // inner rim of Q - bottom face
    QGL::VectorArray tailFace;     // tail of Q - top face
    QGL::VectorArray eTailFace;    // tail of Q - bottom face

    QGL::ColorArray tailColors;
    QGL::ColorArray faceColors;
    QColor4b innerColor(196, 16, 16);
    QColor4b outerColor(128, 128, 255);

    // do the math for the defining points
    qreal t2 = qThickness / 2.0f;
    qreal irad = qRadius - t2;
    qreal orad = qRadius + t2;
    qreal itail = irad - qThickness;
    qreal otail = orad + qThickness;
    tailFace << QVector2D(itail, -t2) << QVector2D(otail, -t2)
            << QVector2D(otail, t2) << QVector2D(itail, t2);
    tailColors << Qt::darkBlue << Qt::darkGray <<
            Qt::darkGreen << Qt::darkMagenta;
    QBox3D tail(tailFace[0], tailFace[2]);
    qreal xi0 = qSqrt((irad * irad) - (t2 * t2));
    qreal xo0 = qSqrt((orad * orad) - (t2 * t2));
    topQORim << QVector2D(xo0, t2);
    topQIRim << QVector2D(xi0, t2);
    for (int i = 0; i < qNumSlices; ++i)
        calculateSlice(i, tail, topQORim, topQIRim);
    topQORim << QVector2D(xo0, -t2);
    topQIRim << QVector2D(xi0, -t2);
    for (int i = 0; i < topQORim.count(); ++i)
    {
        int hue = 360 * (topQORim[i].x() / (qRadius * 3));
        int value = 255 * (topQORim[i].y() / (qRadius * 3));
        faceColors << QColor::fromHsv(hue, 212, value);
    }

    //! [3]
    // create the flat top q
    qList->newSection();
    {
        QGLOperation quad(qList, QGLDisplayList::QUAD);
        quad << tailFace;
        quad << tailColors;
    }
    // qList->addQuad(tailFace[0], tailFace[1], tailFace[2], tailFace[3]);
    int olap = topQORim.count() - topQIRim.count();
    Q_ASSERT(olap % 2 == 0);
    int lap = olap / 2;
    int iptr = 0;
    int optr = lap;

    qDebug() << "inner count:" << topQIRim.count();
    qDebug() << "outer count:" << topQORim.count();
    qDebug() << "overlap is:" << olap;
    qDebug() << "\n=== inner rim ===";
    for (int i = 0; i < topQIRim.count(); ++i)
        qDebug() << "    " << topQIRim[i];
    qDebug() << "\n=== outer rim ===";
    for (int i = 0; i < topQORim.count(); ++i)
        qDebug() << "    " << topQORim[i];

    if (lap)
    {
        QGLOperation face(qList, QGLDisplayList::TRIANGULATED_FACE);
        face.setControl(topQIRim[iptr]);
        face << topQORim.mid(0, lap);
        face << faceColors;
    }
        //qList->addTriangulatedFace(topQIRim[iptr], topQORim.mid(0, lap));
    for ( ; iptr < topQIRim.count() - 1; ++iptr, ++optr)
    {
        QGLOperation quad(qList, QGLDisplayList::QUAD);
        quad << topQIRim[iptr] << innerColor;
        quad << topQORim[optr] << outerColor;
        quad << topQORim[optr+1] << outerColor;
        quad << topQIRim[iptr+1] << innerColor;
        // qList->addQuad(topQIRim[iptr], topQORim[optr],
        //                topQORim[optr+1], topQIRim[iptr+1]);
    }
    if (lap)
    {
        QGLOperation face(qList, QGLDisplayList::TRIANGULATED_FACE);
        face.setControl(topQIRim[iptr]);
        face << topQORim.mid(optr, lap);
        face << faceColors;
    }
    // qList->addTriangulatedFace(topQIRim[iptr], topQORim.mid(optr, lap));

    // create the sides of the q, and save the extruded values
    qList->newSection();
    {
        QGLOperation outsides(qList, QGLDisplayList::EXTRUSION);
        outsides.setControl(qExtrudeVec);
        outsides << QGLDisplayList::NO_CLOSE_PATH;
        outsides << topQOrim;
        outsides << sideColors;
        outsides >> bottomQORim;
    }
    // bottomQORim = qList->extrude(topQORim, qExtrudeVec);
    {
        QGLOperation insides(qList, QGLDisplayList::EXTRUSION);
        insides.setControl(qExtrudeVec);
        insides << QGLDisplayList::FACE_SENSE_REVERSED;
        insides << QGLDisplayList::NO_CLOSE_PATH;
        insides << topQIRim;
        insides << sideColors;
        insides >> bottomQIRim;
    }
    // bottomQIRim = qList->extrude(topQIRim, qExtrudeVec, QGLTextureModel(), true);
    {
        QGLOperation tailSides(qList, QGLDisplayList::EXTRUSION);
        tailSides.setControl(qExtrudeVec);
        tailSides << tailFace;
        tailSides << tailColors;
        tailSides >> eTailFace;
    }
    // eTailFace = qList->extrude(tailFace, qExtrudeVec);

    // create the flat bottom lid of the q
    qList->newSection();
    iptr = 0;
    optr = lap;
    {
        QGLOperation quad(qList, QGLDisplayList::QUAD);
        quad << eTailFace;
        quad << tailColors;
    }
    // qList->addQuad(eTailFace[0], eTailFace[1], eTailFace[2], eTailFace[3]);
    if (lap)
    {
        QGLOperation face(qList, QGLDisplayList::TRIANGULATED_FACE);
        face.setControl(bottomQIRim[iptr]);
        face << bottomQORim.mid(0, lap);
        face << faceColors;
    }
    // qList->addTriangulatedFace(bottomQIRim[iptr], bottomQORim.mid(0, lap));
    for ( ; iptr < topQIRim.count() - 1; ++iptr, ++optr)
    {
        QGLOperation quad(qList, QGLDisplayList::QUAD);
        quad << bottomQIRim[iptr] << innerColor;
        quad << bottomQORim[optr] << outerColor;
        quad << bottomQORim[optr+1] << outerColor;
        quad << bottomQIRim[iptr+1] << innerColor;
    }
    // qList->addQuad(bottomQIRim[iptr], bottomQORim[optr],
    //                bottomQORim[optr+1], bottomQIRim[iptr+1]);
    if (lap)
    {
        QGLOperation face(qList, QGLDisplayList::TRIANGULATED_FACE);
        face.setControl(bottomQIRim[iptr]);
        face << bottomQORim.mid(optr, lap);
        face << faceColors;
    }
    // qList->addTriangulatedFace(bottomQIRim[iptr], bottomQORim.mid(optr, lap));

    qList->finalize();
    return qList;
    //! [3]
}
