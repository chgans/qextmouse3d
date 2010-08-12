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

#include <QtTest/QtTest>

#include "qglcube.h"
#include "qglbuilder.h"

#include "qtest_helpers_p.h"

class tst_QGLCube : public QObject
{
    Q_OBJECT
public:
    tst_QGLCube() {}
    ~tst_QGLCube() {}

private slots:
    void create();
    void size();
    void face();
    void texCoords();
};

#define QGL_CUBE_SIZE (6 * 6 * (3 + 3 + 2))
static float const cubeVertices[QGL_CUBE_SIZE] = {
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,

    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f
};

static int findVertex(const QGeometryData &ary, const QVector3D &v, const QVector3D &n)
{
    int result = -1;
    for (int i = 0; i < ary.count() && result == -1; ++i)
    {
        if (qFuzzyCompare(v, ary.vertexAt(i)) && qFuzzyCompare(n, ary.normalAt(i)))
            result = i;
    }
    if (result == -1)
    {
        qDebug() << "Could not find expected vertex" << v << "with normal" << n;
    }
    return result;
}

static int findFace(const QVector3DArray &face, const QGeometryData &vrts)
{
    Q_ASSERT(face.count() == 3);
    int result = -1;
    int t[3] = { 0 };
    int cnt = 0;
    QGL::IndexArray indx = vrts.indices();
    for (int i = 0; i < indx.size(); ++i)
    {
        if (cnt < 3)
        {
            // accumulate triangle indexes into t
            t[cnt++] = indx[i];
        }
        else
        {
            // for each set of 3 indexes try all three orderings to see if it matches face
            for (int j = 0; j < 3; ++j)
            {
                QVector3D a = vrts.vertexAt(t[j]);
                QVector3D b = vrts.vertexAt(t[(j + 1) % 3]);
                QVector3D c = vrts.vertexAt(t[(j + 2) % 3]);
                if (qFuzzyCompare(a, face[0]) &&
                    qFuzzyCompare(b, face[1]) &&
                    qFuzzyCompare(c, face[2]))
                {
                    result = i;
                    break;
                }
            }
            cnt = 0;
        }
    }
    if (result == -1)
    {
        qDebug() << "Could not find expected face" << face;
    }
    return result;
}

void tst_QGLCube::create()
{
    QGLCube cube;
    QGLBuilder list;
    list.newSection(QGL::Faceted);
    QGLSceneNode *node = list.currentNode();
    list << cube;
    QGLSceneNode *root = list.finalizedSceneNode();
    QGeometryData ary = node->geometry();
    for (int i = 0; i < QGL_CUBE_SIZE; i += 8)
    {
        const QVector3D *vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
        const QVector2D *tdata = reinterpret_cast<const QVector2D*>(cubeVertices + i + 6);
        int ix = findVertex(ary, vdata[0], vdata[1]);
        QVERIFY(ix != -1);
        QCOMPARE(ary.texCoordAt(ix), tdata[0]);
    }
    delete root;
}

void tst_QGLCube::size()
{
    qreal size0 = 3.2f;
    qreal size1 = 5.2f;
    QGLCube cube(size0);
    QCOMPARE(cube.size(), size0);
    {
        QGLBuilder list;
        list.newSection(QGL::Faceted);
	QGLSceneNode *node = list.currentNode();
        list << cube;
        QGLSceneNode *root = list.finalizedSceneNode();
        QGeometryData ary = node->geometry();
        for (int i = 0, v = 0; i < QGL_CUBE_SIZE; ++v, i += 8)
        {
            const QVector3D *vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
            const QVector2D *tdata = reinterpret_cast<const QVector2D*>(cubeVertices + i + 6);
            int ix = findVertex(ary, vdata[0] * size0, vdata[1]);
            QVERIFY(ix != -1);
            QCOMPARE(ary.texCoordAt(ix), tdata[0]);
        }
        delete root;
    }
    cube.setSize(size1);
    {
        QGLBuilder list;
        list.newSection(QGL::Faceted);
	QGLSceneNode *node = list.currentNode();
        list << cube;
        QGLSceneNode *root = list.finalizedSceneNode();
        QGeometryData ary = node->geometry();
        for (int i = 0, v = 0; i < QGL_CUBE_SIZE; ++v, i += 8)
        {
            const QVector3D *vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
            const QVector2D *tdata = reinterpret_cast<const QVector2D*>(cubeVertices + i + 6);
            int ix = findVertex(ary, vdata[0] * size1, vdata[1]);
            QVERIFY(ix != -1);
            QCOMPARE(ary.texCoordAt(ix), tdata[0]);
        }
        delete root;
    }
}

void tst_QGLCube::face()
{
    QGLCubeFace cfL(QGLCubeFace::Left);
    QGLCubeFace cfT(QGLCubeFace::Top);
    QGLCubeFace cfR(QGLCubeFace::Right);
    QGLCubeFace cfBt(QGLCubeFace::Bottom);
    QGLCubeFace cfF(QGLCubeFace::Front);
    QGLCubeFace cfBk(QGLCubeFace::Back);
    {
        QGLBuilder list;
        list.newSection(QGL::Faceted);
	QGLSceneNode *node = list.currentNode();
        list << cfL << cfT << cfR << cfBt << cfF << cfBk;
        QGLSceneNode *root = list.finalizedSceneNode();
        QGeometryData ary = node->geometry();
        QVector3DArray fm;
        for (int i = 0; i < QGL_CUBE_SIZE; i += 8)
        {
            const QVector3D *vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
            const QVector2D *tdata = reinterpret_cast<const QVector2D*>(cubeVertices + i + 6);
            int ix = findVertex(ary, vdata[0], vdata[1]);
            QVERIFY(ix != -1);
            QCOMPARE(ary.texCoordAt(ix), tdata[0]);
            if (fm.count() == 3)
            {
                int ifx = findFace(fm, ary);
                QVERIFY(ifx != -1);
                fm.clear();
            }
            else
            {
                fm.append(vdata[0]);
            }
        }
        delete root;
    }
    qreal size0 = 3.2f;
    qreal size1 = 5.6f;
    QGLCubeFace::Face f = QGLCubeFace::Bottom;
    QGLCubeFace cubeFace(f, size0);
    int begin = f * 8 * 6;
    int end = (f + 1) * 8 * 6;
    QCOMPARE(cubeFace.size(), size0);
    {
        QGLBuilder list;
        list.newSection(QGL::Faceted);
	QGLSceneNode *node = list.currentNode();
        list << cubeFace;
        QGLSceneNode *root = list.finalizedSceneNode();
        QGeometryData ary = node->geometry();
        for (int i = begin; i < end; i += 8)
        {
            const QVector3D *vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
            const QVector2D *tdata = reinterpret_cast<const QVector2D*>(cubeVertices + i + 6);
            int ix = findVertex(ary, vdata[0] * size0, vdata[1]);
            QVERIFY(ix != -1);
            QCOMPARE(ary.texCoordAt(ix), tdata[0]);
        }
        delete root;
    }
    cubeFace.setSize(size1);
    QCOMPARE(cubeFace.size(), size1);
    {
        QGLBuilder list;
        list.newSection(QGL::Faceted);
	QGLSceneNode *node = list.currentNode();
        list << cubeFace;
        QGLSceneNode *root = list.finalizedSceneNode();
        QGeometryData ary = node->geometry();
        for (int i = begin; i < end; i += 8)
        {
            const QVector3D *vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
            const QVector2D *tdata = reinterpret_cast<const QVector2D*>(cubeVertices + i + 6);
            int ix = findVertex(ary, vdata[0] * size1, vdata[1]);
            QVERIFY(ix != -1);
            QCOMPARE(ary.texCoordAt(ix), tdata[0]);
        }
        delete root;
    }
}

void tst_QGLCube::texCoords()
{
    QGLCubeFace cfR(QGLCubeFace::Right);
    QCOMPARE(cfR.textureCoord(QGLCubeFace::TopRight), QVector2D(1.0f, 1.0f));
    QCOMPARE(cfR.textureCoord(QGLCubeFace::BottomLeft), QVector2D());

    QGLCubeFace::Face f = QGLCubeFace::Left;
    QGLCubeFace cfL(QGLCubeFace::Left);
    QVector2D leftFaceBL(0.2f, 0.2f);
    QVector2D leftFaceTR(0.8f, 0.8f);
    QVector2D leftFaceBR(0.8f, 0.2f);
    QVector2D leftFaceTL(0.2f, 0.8f);
    cfL.setTextureCoord(QGLCubeFace::BottomLeft, leftFaceBL.x(), leftFaceBL.y());
    cfL.setTextureCoord(QGLCubeFace::BottomRight, leftFaceBR.x(), leftFaceBR.y());
    cfL.setTextureCoord(QGLCubeFace::TopRight, leftFaceTR.x(), leftFaceTR.y());
    cfL.setTextureCoord(QGLCubeFace::TopLeft, leftFaceTL.x(), leftFaceTL.y());
    QCOMPARE(cfL.textureCoord(QGLCubeFace::BottomLeft), leftFaceBL);
    QCOMPARE(cfL.textureCoord(QGLCubeFace::BottomRight), leftFaceBR);
    QCOMPARE(cfL.textureCoord(QGLCubeFace::TopRight), leftFaceTR);
    QCOMPARE(cfL.textureCoord(QGLCubeFace::TopLeft), leftFaceTL);
    {
        int begin = f * 8 * 6;
        QGLBuilder list;
        list.newSection(QGL::Faceted);
	QGLSceneNode *node = list.currentNode();
        list << cfL;
        QGLSceneNode *root = list.finalizedSceneNode();
        QGeometryData ary = node->geometry();

        int i = begin;  // starts at bottom right
        const QVector3D *vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
        int ix = findVertex(ary, vdata[0], vdata[1]);
        QVERIFY(ix != -1);
        QCOMPARE(ary.texCoordAt(ix), leftFaceBR);

        i = begin + 8;  // next is top right
        vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
        ix = findVertex(ary, vdata[0], vdata[1]);
        QVERIFY(ix != -1);
        QCOMPARE(ary.texCoordAt(ix), leftFaceTR);

        i = begin + 16;  // next is top left
        vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
        ix = findVertex(ary, vdata[0], vdata[1]);
        QVERIFY(ix != -1);
        QCOMPARE(ary.texCoordAt(ix), leftFaceTL);

        i = begin + 40;  // skip two dups, next is bottom left
        vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
        ix = findVertex(ary, vdata[0], vdata[1]);
        QVERIFY(ix != -1);
        QCOMPARE(ary.texCoordAt(ix), leftFaceBL);

        delete root;
    }

    f = QGLCubeFace::Top;
    QGLCubeFace cfT(QGLCubeFace::Top);
    QVector2D topFaceBL(1.0f, 0.0f);
    QVector2D topFaceTR(0.0f, 1.0f);
    QVector2D topFaceBR(0.0, 0.0f);
    QVector2D topFaceTL(1.0f, 1.0f);
    cfT.setTextureCoord(QGLCubeFace::BottomLeft, topFaceBL);
    cfT.setTextureCoord(QGLCubeFace::BottomRight, topFaceBR);
    cfT.setTextureCoord(QGLCubeFace::TopRight, topFaceTR);
    cfT.setTextureCoord(QGLCubeFace::TopLeft, topFaceTL);
    QCOMPARE(cfT.textureCoord(QGLCubeFace::BottomLeft), topFaceBL);
    QCOMPARE(cfT.textureCoord(QGLCubeFace::BottomRight), topFaceBR);
    QCOMPARE(cfT.textureCoord(QGLCubeFace::TopRight), topFaceTR);
    QCOMPARE(cfT.textureCoord(QGLCubeFace::TopLeft), topFaceTL);
    {
        int begin = f * 8 * 6;
        QGLBuilder list;
        list.newSection(QGL::Faceted);
	QGLSceneNode *node = list.currentNode();
        list << cfT;
        QGLSceneNode *root = list.finalizedSceneNode();
        QGeometryData ary = node->geometry();

        int i = begin;  // starts at bottom right
        const QVector3D *vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
        int ix = findVertex(ary, vdata[0], vdata[1]);
        QVERIFY(ix != -1);
        QCOMPARE(ary.texCoordAt(ix), topFaceBR);

        i = begin + 8;  // next is top right
        vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
        ix = findVertex(ary, vdata[0], vdata[1]);
        QVERIFY(ix != -1);
        QCOMPARE(ary.texCoordAt(ix), topFaceTR);

        i = begin + 16;  // next is top left
        vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
        ix = findVertex(ary, vdata[0], vdata[1]);
        QVERIFY(ix != -1);
        QCOMPARE(ary.texCoordAt(ix), topFaceTL);

        i = begin + 40;  // skip two dups, next is bottom left
        vdata = reinterpret_cast<const QVector3D*>(cubeVertices + i);
        ix = findVertex(ary, vdata[0], vdata[1]);
        QVERIFY(ix != -1);
        QCOMPARE(ary.texCoordAt(ix), topFaceBL);

        delete root;
    }
}

QTEST_APPLESS_MAIN(tst_QGLCube)

#include "tst_qglcube.moc"
